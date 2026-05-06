# Hopscotch Hashing

## Introduction
- like linear probing, but each key must live within a **neighborhood** of H slots from its home bucket
- a bitmap (`hop_info`) on each slot tracks which nearby slots contain entries that hash to it
- lookup is always **O(H) worst case** — bounded, cache-friendly, never chases a long probe chain
- named after the children's game — a key hops to a nearby slot, always staying within reach of home

## How it works
- each slot has a `hop_info` bitmap: bit `i` set means slot `(home + i)` contains a key that hashes to this slot
- on insert: find an empty slot, displace entries inward until the empty slot is within the neighborhood
- on get: read `hop_info[home]`, check only the bits that are set — at most H comparisons
- on remove: clear `is_occupied` and clear the bit in `hop_info[home]`

## Displacement algorithm
```
insert key X, home = h:
  scan slots h..h+H-1: if X exists → overwrite; track first empty slot
  if empty slot found in neighborhood → place X there, set hop_info bit, done

  linear probe forward to find any empty slot (beyond neighborhood)
  while empty slot is outside neighborhood:
    scan H slots before empty: find an entry whose home is within H of empty
    move that entry to empty slot, update hop_info
    empty slot moves closer to home
  place X at empty slot, set hop_info bit
```

## When displacement gets stuck
- if no nearby entry can move into the empty slot → resize and retry
- after resize the table is larger, displacement succeeds
- retry counter prevents infinite recursion if pathological keys keep causing stuck loops

## Deletion
- no tombstones needed — hop_info tracks ownership exactly
- clear `is_occupied`, clear the bit in `hop_info[home]` → O(H) to find, O(1) to delete

## Complexity
- **get, remove**: O(H) worst case — only bits set in hop_info are checked
- **insert**: O(H²) worst case — displacement loop × reverse scan
- **space**: one flat array + one `uint8_t` per slot for hop_info

---

# More Information

## Usecase

### 1. Intel TBB `concurrent_hash_map`
- Intel's Threading Building Blocks library uses hopscotch hashing for its concurrent hash map
- the neighborhood guarantee means reads only touch H slots — fits in 1–2 cache lines, enabling lock-free reads
- used in production across Intel's tools, game engines, and HPC workloads

### 2. Java `ConcurrentHashMap` (Java 8+)
- Java 8 rewrote `ConcurrentHashMap` with ideas derived from hopscotch — segment-local probing with bounded scan width
- the bounded neighborhood means readers can safely snapshot a small window without locking the whole table

### 3. Google Abseil / Swiss Tables
- Abseil studied hopscotch before designing Swiss Tables (used in Chrome, TensorFlow, Abseil)
- Swiss Tables kept the "metadata bitmap per group" idea but replaced hop_info with a SIMD-friendly byte array
- hopscotch is the direct ancestor of the Swiss Table design

### 4. Key-value stores (Redis, LMDB derivatives)
- several embedded key-value engines use neighborhood-bounded probing for cache-efficient lookups
- hopscotch's guarantee that all candidates fit in a cache line is the key selling point for latency-sensitive storage

### 5. Network packet classification
- like cuckoo hashing, hopscotch is used in software-defined networking for flow table lookup
- O(H) worst-case lookup with a small fixed H gives deterministic latency, critical for packet forwarding SLAs

---

## Facts

### Invented in 2008 — designed for concurrency from day one
- invented by Maurice Herlihy, Nir Shavit, and Moran Tzafrir (Brown University / Tel Aviv University)
- paper: *"Hopscotch Hashing"*, DISC 2008
- unlike every previous hash table, it was designed specifically for **concurrent multi-threaded access**, not retrofitted for it
- Nir Shavit later won the Dijkstra Prize for concurrent data structures work

### The real innovation is lock-free reads
- because a lookup reads at most H slots and H fits in 1–2 cache lines, a reader can snapshot `hop_info[home]` once and check only those slots
- even if a writer is mid-displacement, the reader sees either the old or new state — both valid
- this makes **lock-free reads** natural; no mutex or RCU needed for reads

### hop_info belongs to the slot, not the entry
- this is the subtlest design point: `hop_info` is metadata about a *slot* (which entries hash to it), not about the *entry* sitting in the slot
- when an entry is displaced to a new physical slot, its `hop_info` is NOT copied — only key/value move
- if you copy `hop_info` during displacement you corrupt the table silently (a common bug)

### Works well up to ~90% load factor
- linear probing degrades badly above 70%, quadratic probing must stay below 50%
- hopscotch works reliably up to 80–90% because displacement keeps entries tight near their home
- the failure mode (stuck displacement) is rare below 90% with a good hash function

### Larger H = better load tolerance, worse cache behavior
- H=8 fits in one `uint8_t` and all 8 candidates likely in one cache line → fastest
- H=32 uses `uint32_t`, tolerates higher load, but 32 slots may span 4+ cache lines
- most implementations settle on H=8 or H=16 as the sweet spot

---

## Tradeoffs vs previous stages

| | Robin Hood (stage 6) | Cuckoo (stage 7) | Hopscotch (stage 8) |
|---|---|---|---|
| Lookup worst case | O(n) | **O(1)** | O(H) ≈ O(8) |
| Insert worst case | O(n) | O(n) rehash | O(H²) displacement |
| Cache performance | Excellent | Good (2 accesses) | **Excellent** |
| Max safe load | ~80% | ~49% per table | **~90%** |
| Tombstones needed | No | No | **No** |
| Concurrent reads | Hard | Hard | **Easy (lock-free)** |
| Memory overhead | 1 array + DIB | 2 arrays | 1 array + 1 byte/slot |
| Implementation complexity | Medium | High | Medium-High |
