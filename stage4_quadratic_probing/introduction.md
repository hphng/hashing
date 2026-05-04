# Quadratic Probing

## Introduction
- linear probing fixes the heap allocation problem of chaining, but creates **primary clustering** — long consecutive runs of occupied slots that slow down every operation
- quadratic probing uses the same flat array idea, but instead of stepping by 1, it steps by 1², 2², 3²...
- the non-linear jump breaks up clusters at the cost of a stricter table size requirement

## Collision handling
- if there is a collision, probe at offsets `h(k) + 1²`, `h(k) + 2²`, `h(k) + 3²`... (mod table size)
- insert into the **earliest tombstone** encountered, or the first **empty slot** if no tombstone found
- table size **must be prime** and load factor must stay **below 0.5** — otherwise the probe sequence may not reach all slots (see `proof.md`)

## Tombstone
- same as linear probing: deleted slots become tombstones, not empty
- removing a slot and making it empty would break the probe chain for keys that were displaced past it
- tombstones are cleaned up during **resize** (full rehash)

## Complexity
- **insert, get, remove**: O(1) average, O(n) worst case
- **space**: O(n) — no extra memory per element

---

# More Information

## Usecase

### 1. Java `IdentityHashMap`
- uses quadratic probing (step size 2) internally in the JVM
- used for serialization and object graph traversal where reference equality matters, not `.equals()`

### 2. GPU hash tables
- quadratic probing maps well to SIMD execution on GPUs
- all threads in a warp follow the same deterministic probe pattern → minimal branch divergence
- used in CUDA-based data processing pipelines

### 3. CPython `dict` (indirect influence)
- CPython uses a perturbation-based probe sequence inspired by quadratic probing
- pure quadratic was considered but the perturbation variant handles non-prime sizes better in practice

---

## Facts

### Prime table size is required
- with a prime table size, the first ⌊n/2⌋+1 probes are guaranteed to visit distinct slots
- this means max safe load factor is just under **0.5** — compared to 0.7+ for linear probing
- see `proof.md` for the full mathematical proof

### Primary clustering is gone, secondary clustering appears
- **primary clustering** (linear probing): long consecutive runs grow as they merge — nearby keys pollute each other's probe paths
- **secondary clustering** (quadratic probing): two keys with the same initial hash follow the **exact same probe sequence** — they always compete, just not in a long run
- double hashing (stage 5) eliminates secondary clustering by making the step size key-dependent

### The triangular number trick — no multiplication needed
- computing i² at each step looks like it needs a multiply, but there's a smarter way:

```
i² = (i-1)² + (2i - 1)
```

- each next offset = previous offset + next odd number:

```cpp
size_t offset = 0, step = 1;
// each probe:
offset += step;   // 0 → 1 → 4 → 9 → 16...
step += 2;        // 1 → 3 → 5 → 7...
```

- no multiplication at all — many real implementations use this trick

---

## Tradeoffs vs previous stages

| | Chaining (Stage 1/2) | Linear Probing (Stage 3) | Quadratic Probing (Stage 4) |
|---|---|---|---|
| Memory layout | Scattered (heap) | Flat array | Flat array |
| Cache performance | Poor | Best | Good |
| Clustering | None | Primary (bad) | Secondary (mild) |
| Max safe load | ~0.9 | ~0.7 | ~0.5 |
| Table size constraint | None | None | Must be prime |
| Deletion | Simple | Tombstone | Tombstone |
