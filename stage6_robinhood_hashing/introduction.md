# Robin Hood Hashing

## Introduction
- all previous open addressing strategies (linear, quadratic, double hashing) treat every key equally — a key that lands far from home just stays far from home
- Robin Hood hashing uses the same linear probe sequence, but rebalances keys during insert by **stealing slots from rich keys and giving them to poor keys**
- "rich" = close to home (small DIB), "poor" = far from home (large DIB)
- the goal is to minimize **variance** in probe distances — no key suffers disproportionately

## DIB (Distance from Initial Bucket)
- every slot tracks how far its key has traveled from its home slot
- `DIB = (current_index - home_index + table_size) % table_size`
- wraps correctly around the table — a key at index 1 with home 6 (table size 7) has DIB = 2, not -5

## Collision handling (insert)
- probe linearly like stage 3
- at each occupied slot, compare DIB of inserting key vs DIB of sitting key
- if inserting key has traveled **more** (higher DIB) → swap, continue probing with the displaced key
- this keeps the table sorted by DIB within each cluster

## Deletion — backward shift (no tombstones needed)
- setting a deleted slot to EMPTY would break the probe chain for displaced keys
- instead: **shift subsequent keys backward** one slot until hitting an EMPTY slot or a key with DIB = 0
- a key with DIB = 0 is already at its home — shifting it back would be wrong
- result: the table stays clean with no wasted tombstone slots accumulating over time

## Complexity
- **insert, get, remove**: O(1) average, O(n) worst case
- **space**: O(n) — no extra memory per element
- **remove**: O(cluster length) in worst case, but clusters are short due to DIB balancing

---

# More Information

## Usecase

### 1. Rust's `HashMap` (std)
- Rust's standard library uses Robin Hood hashing as its default hash map
- chosen specifically for its variance reduction and cache-friendly behavior
- switched to SwissTable (hashbrown) in Rust 1.36, but Robin Hood was the default for years

### 2. Abseil `flat_hash_map` (Google)
- Google's open-source C++ hash map library uses a Robin Hood variant with SIMD probing
- used extensively inside Google's production infrastructure

### 3. `Bytell Hash Map` by Malte Skarupke
- one of the fastest open-source hash maps benchmarked, based on Robin Hood hashing
- widely cited in hash map performance comparisons

### 4. Various game engines
- game engines favor Robin Hood hashing because worst-case lookup is bounded and predictable
- real-time systems can't tolerate the occasional O(n) probe of unconstrained linear probing

---

## Facts

### Robin Hood reduces worst-case lookup, not just average
- because keys are sorted by DIB within a cluster, you can **stop a lookup early**
- if your probe distance exceeds the DIB of the key you're looking at, the key doesn't exist
- this turns failed lookups from "probe until EMPTY" into "probe until DIB mismatch" — much faster

### Backward shift is O(cluster length) but pays for itself
- tombstone-based deletion poisons the table permanently — every lookup and insert pays extra forever
- backward shift is a one-time cost at delete time, but every future operation on that cluster is faster
- at high load factors, this tradeoff strongly favors backward shift

### Variance reduction is the core insight
- Robin Hood doesn't reduce the **average** probe length compared to plain linear probing
- it reduces the **variance** — the gap between best-case and worst-case probes
- this makes performance more **predictable**, which matters as much as raw speed in production systems

### The name
- coined by Pedro Celis in his 1986 PhD thesis at the University of Waterloo
- the metaphor: just like Robin Hood takes from the rich to give to the poor, the algorithm displaces keys that are close to home in favor of keys that have traveled far

---

## Tradeoffs vs previous stages

| | Linear Probing | Quadratic Probing | Double Hashing | Robin Hood |
|---|---|---|---|---|
| Clustering | Primary (bad) | Secondary (mild) | None | Primary (balanced) |
| Cache performance | Best | Good | Poor | Best |
| Max safe load | ~0.7 | ~0.5 | ~0.7 | ~0.9 |
| Worst-case lookup | Poor | Medium | Medium | Early termination |
| Tombstones needed | Yes | Yes | Yes | **No** |
| Deletion cost | O(1) | O(1) | O(1) | O(cluster) |
| Table size constraint | None | Prime | Prime | None |
