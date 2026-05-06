# Hashing Strategies

Implementing every major hashing strategy used in real-world systems, from scratch in C++. Code written by me. I use [Claude Code](https://claude.ai/code) as a reviewer: it explains concepts, reviews my code, and points out bugs.

## Build & Run

```bash
g++ -fdiagnostics-color=always -g -std=c++20 tests/test_stageN.cpp -o tests/test_stageN.exe && ./tests/test_stageN.exe
```

## Stages

### Stage 1: Chaining with Linked List
Each bucket holds a linked list. The simplest collision resolution strategy and the basis for `std::unordered_map` in most standard library implementations. Worst case O(n) when all keys collide: vulnerable to HashDoS attacks where an attacker crafts keys that all land in the same bucket.

### Stage 2: Chaining with Dynamic Array
Replaces linked list nodes with a contiguous array per bucket. Linked lists cause cache misses (~100 CPU cycles each vs ~5 for a cache hit) because nodes are scattered in heap memory. The tradeoff: an empty `std::vector` costs 24 bytes, so 100k buckets = 24MB of overhead even with no entries. Used by Google's `flat_hash_map` and game engines like Unreal and Unity.

### Stage 3: Linear Probing
Eliminates separate allocations by storing everything in one flat array. On collision, probe the next slot linearly. Requires **tombstones** for deleted slots: setting a slot to empty would break probe chains for keys that collided past it. The best cache performance of any open addressing scheme. Used by CPython's dict (3.6+), PHP arrays, V8 (JavaScript), and Ruby 2.4+. Load factor 0.7 → ~2.2 probes on average (Knuth, 1963).

### Stage 4: Quadratic Probing
Probes by 1², 2², 3² instead of 1, 2, 3 to break up primary clustering (consecutive occupied slots creating long chains). Requires a prime table size and load factor below 0.5 to guarantee every slot is reachable. Reduces clustering but can't run as full as linear probing. Used by Java's `IdentityHashMap` and GPU hash tables (SIMD-friendly non-sequential access).

### Stage 5: Double Hashing
Uses a second hash function to determine step size: `next = (h1 + i × h2) % n`. Eliminates both primary and secondary clustering: every key gets a unique probe sequence. The cost: non-sequential memory access kills cache performance. Used by pre-3.6 Python sets and Google's `sparse_hash_map`. Step size must be coprime with table size (guaranteed if table size is prime or step size is odd).

### Stage 6: Robin Hood Hashing
Same linear probe sequence as stage 3, but steals slots from "rich" keys (close to home) to give to "poor" keys (far from home). Tracks each key's **DIB** (distance from initial bucket). Reduces variance in probe lengths: worst-case lookup is much closer to average. Deletion uses backward shift instead of tombstones, keeping the table permanently clean. Coined by Pedro Celis in his 1986 PhD thesis. Used by Rust's standard `HashMap` (before SwissTable).

### Stage 7: Cuckoo Hashing
Every key has exactly **two possible slots** across two hash tables. Lookup and delete are always **O(1) worst case**: just check two positions, no loop. On insert, evict-and-displace until both slots are free. Uses a cycle detector (max eviction count → rehash with new seeds). Load factor must stay below 50% per table: above that, eviction cycles become near-certain (proven via graph theory). Used by Intel DPDK for packet forwarding, NVIDIA GPU memory management, and TikTok's recommendation model embedding table.

### Stage 8: Hopscotch Hashing
Each key must live within a **neighborhood** of H slots from its home bucket. A per-slot bitmap (`hop_info`) tracks which nearby slots belong to it. Lookup checks only H slots: O(H) worst case with all candidates likely in one cache line. Tolerates load up to ~90%. Designed from the ground up for concurrent multi-threaded access (Brown University / Tel Aviv, 2008): lock-free reads are natural because a reader only needs to snapshot a fixed-size window. Direct ancestor of Google's Swiss Tables (used in Chrome, TensorFlow, Abseil).

### Stage 9: Consistent Hashing
Fundamentally different from stages 1–8: routes keys to **machines**, not slots. Normal `key % N` remaps ~99% of keys when one server is added or removed. Consistent hashing maps keys and servers onto a ring: only `1/N` of keys move when the cluster changes. Each server gets ~100 virtual nodes for even load distribution. Invented at MIT in 1997; co-inventor Daniel Lewin later founded Akamai. Used by Amazon Dynamo, Apache Cassandra (Apple runs 75,000+ nodes), Memcached, Redis Cluster, and every major CDN.

### Stage 10: Bloom Filter
A probabilistic set: answers "have I seen this key?" using a bit array and K hash functions. Stores **neither keys nor values**: only bits. No false negatives (an inserted key is always found). False positives are possible. Memory is fixed regardless of how many items are inserted. Rule of thumb: 10 bits per element → ~1% false positive rate. Invented by Burton Howard Bloom in 1970. Used by RocksDB/LevelDB (skip disk reads), Google Chrome Safe Browsing (check URLs locally before any network call), Akamai (avoid caching one-hit wonders), and Bitcoin SPV wallets.

### Stage 11: Count-Min Sketch
A probabilistic counter: answers "how many times have I seen this key?" using a d×w grid of integers. Never undercounts: estimates are always ≥ true count. Error is bounded by `ε × N` (total insertions) with probability `1 - δ`. Best for **heavy hitter detection**: identifying items with frequency far above the noise floor. For rare events, the additive error dominates. Invented by Graham Cormode and S. Muthukrishnan in 2003. Used by AT&T for network traffic analysis (the original use case), Twitter for trending topics, Redis (native CMS module), and Apache Flink/Spark for streaming analytics.

## At a Glance

| Stage | Strategy | Lookup | Max Load | Tombstones | Real-world user |
|---|---|---|---|---|---|
| 1 | Chaining (linked list) | O(1) avg | ~75% | No | `std::unordered_map` |
| 2 | Chaining (dynamic array) | O(1) avg | ~75% | No | Google `flat_hash_map` |
| 3 | Linear probing | O(1) avg | ~70% | Yes | CPython dict, V8 |
| 4 | Quadratic probing | O(1) avg | ~50% | Yes | Java `IdentityHashMap` |
| 5 | Double hashing | O(1) avg | ~70% | Yes | Google `sparse_hash_map` |
| 6 | Robin Hood | O(1) avg | ~90% | No | Rust `HashMap` |
| 7 | Cuckoo hashing | **O(1) worst** | ~49% | No | Intel DPDK, TikTok |
| 8 | Hopscotch hashing | O(H) worst | ~90% | No | Intel TBB, Swiss Tables |
| 9 | Consistent hashing | O(log VN) |: |: | Cassandra, Dynamo |
| 10 | Bloom filter | O(k) | fixed |: | RocksDB, Chrome |
| 11 | Count-Min sketch | O(d) | fixed |: | Twitter, Redis CMS |
