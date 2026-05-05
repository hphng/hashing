# Cuckoo Hashing

## Introduction
- all previous open addressing strategies probe through a sequence of slots — worst case lookup is O(n)
- cuckoo hashing eliminates probing entirely: every key has **exactly 2 possible slots**, one in each table
- get and remove are always **O(1) worst case** — just two lookups, no loop needed
- named after the cuckoo bird, which lays its eggs in other birds' nests and evicts the existing eggs

## How it works
- two separate hash tables: `bucket1` and `bucket2`, each with its own independent hash function
- a key `k` can only ever live at `bucket1[h1(k)]` or `bucket2[h2(k)]` — nowhere else
- on insert: try both slots, if both occupied → evict one key to its other table, repeat

## Eviction algorithm
```
insert key X:
  if bucket1[h1(X)] is empty → place X there, done
  if bucket2[h2(X)] is empty → place X there, done

  evict key from bucket1[h1(X)], call it Y → place X there
  Y is now homeless → try bucket2[h2(Y)]
  evict key from bucket2[h2(Y)], call it Z → place Y there
  Z is now homeless → try bucket1[h1(Z)]
  ... repeat, alternating tables
```

## Cycle detection
- eviction can loop forever: X evicts Y → Y evicts Z → Z evicts X → ...
- solution: track eviction count, if it exceeds MAX_EVICTIONS → **rehash** with new seeds
- rehash: same table size, new hash functions (new seeds) → keys map to different slots → cycle broken

## Deletion
- no tombstones needed — unlike open addressing, there are no probe chains to break
- just mark the slot EMPTY directly → O(1) worst case

## Complexity
- **get, remove**: O(1) worst case — only 2 slots to check
- **insert**: O(1) amortized — eviction chains are short on average
- **space**: O(n) across two tables

---

# More Information

## Usecase

### 1. Network routers (Cisco, Intel DPDK)
- router lookup tables need guaranteed O(1) lookup — a packet must be forwarded in nanoseconds
- cuckoo hashing is the standard choice for hardware and software packet forwarding tables
- Intel's DPDK (Data Plane Development Kit) includes a cuckoo hash implementation used in high-speed networking

### 2. GPU memory deduplication (NVIDIA)
- NVIDIA uses cuckoo hashing in GPU memory management for page deduplication
- two-slot lookup maps perfectly to parallel memory access patterns on GPU hardware

### 3. Database join acceleration (academic + industry)
- cuckoo hashing is used in hash join operators in analytical databases
- the guaranteed O(1) probe makes query plan cost estimation precise — no worst-case surprises

### 4. Akamai CDN
- Akamai's distributed cache uses a variant of cuckoo hashing for consistent key placement across servers

### 5. TikTok Monolith (ByteDance, 2022)
- TikTok's real-time recommendation engine, described in the paper "Monolith: Real Time Recommendation System With Collisionless Embedding Table", uses cuckoo hashing for its sparse feature embedding table
- the problem: a recommendation model stores billions of feature IDs (user IDs, video IDs, hashtags) mapped to embedding vectors — a hash collision means two different IDs share the same embedding slot, corrupting model training
- cuckoo hashing gives a **collisionless** guarantee: every feature ID has exactly one slot, no two IDs ever map to the same embedding
- the two-slot lookup also fits naturally with TikTok's need for O(1) feature lookup during inference, where serving latency is measured in milliseconds

---

## Facts

### Invented in 2001 — relatively modern
- cuckoo hashing was invented by Rasmus Pagh and Flemming Friche Rodler in their 2001 paper "Cuckoo Hashing"
- compared to linear probing (1950s) and double hashing (1957), it's a recent invention
- the original paper proved the O(1) worst-case guarantee mathematically

### The graph theory connection
- model the hash table as a graph: each key = an edge, each slot = a node
- an edge connects `h1(k)` and `h2(k)` for key `k`
- a cycle in this graph = an infinite eviction loop
- when load factor exceeds 0.5, the graph has more edges than nodes → cycles become nearly certain (pigeonhole principle)
- this is why load factor must stay below 0.5

### O(1) worst case vs O(1) amortized
- "O(1) worst case" for get/remove is a strong guarantee — most hash maps only promise O(1) amortized
- this matters in real-time systems where a single slow operation can miss a deadline
- insert is O(1) amortized (occasional rehash), but get and remove never slow down

### Two tables vs one table with two hash functions
- some implementations use a single flat array with two hash functions pointing into it
- two separate tables make the alternation logic cleaner and avoid edge cases where h1(k) == h2(k)
- with two tables, h1 and h2 index into different spaces → no accidental same-slot collision

### Cuckoo filters (2014)
- a modern variant called **Cuckoo Filter** replaces values with fingerprints (short hash signatures)
- used as a probabilistic set membership structure — like a Bloom filter but supports deletion
- Facebook, Redis, and many CDN systems use cuckoo filters for cache admission policies

---

## Tradeoffs vs previous stages

| | Linear Probing | Double Hashing | Robin Hood | Cuckoo Hashing |
|---|---|---|---|---|
| Lookup worst case | O(n) | O(n) | O(n) early exit | **O(1)** |
| Insert worst case | O(n) | O(n) | O(n) | O(n) rehash |
| Cache performance | Best | Poor | Best | Good (2 accesses) |
| Max safe load | ~0.7 | ~0.7 | ~0.9 | ~0.5 |
| Tombstones needed | Yes | Yes | No | **No** |
| Memory overhead | 1 table | 1 table | 1 table | **2 tables** |
| Hash functions needed | 1 | 2 | 1 | **2** |
