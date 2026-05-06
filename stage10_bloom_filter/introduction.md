# Bloom Filter

## Introduction
- all previous stages store the actual key and value — a Bloom filter stores **neither**
- it only answers one question: "have I seen this key before?" with a probabilistic yes/no
- **no false negatives** — if a key was inserted, `check` always returns true
- **false positives are possible** — `check` may return true for a key that was never inserted
- uses a fixed amount of memory regardless of how many keys are inserted — no resizing

## How it works
- internally just a **bit array** of size `m`, all initialized to 0
- on `insert(key)`: compute K bit positions from the key, set all K bits to 1
- on `check(key)`: compute the same K bit positions, return true only if **all** K bits are 1
- false positives happen when all K positions of an unseen key happen to be set by other keys

```
insert("alice"):  hash positions → 2, 7, 14  → set bits 2, 7, 14
insert("bob"):    hash positions → 5, 7, 19  → set bits 5, 7, 19

check("alice"):   positions 2, 7, 14 → all 1 → true  ✓
check("carol"):   positions 3, 7, 14 → bit 3 is 0 → false ✓ (definitely not inserted)
check("dave"):    positions 5, 7, 14 → all 1 → true ✗ (false positive — never inserted)
```

## Optimal parameters
- **m** (bit array size): `m = -(n × ln(p)) / (ln2)²`
- **k** (number of hash functions): `k = (m/n) × ln2`
- where `n` = expected number of elements, `p` = desired false positive rate
- rule of thumb: **10 bits per element → ~1% false positive rate**

## Double hashing for K positions
- computing K truly independent hash functions is expensive
- Kirsch-Mitzenmacher (2006) proved you only need two: derive all K positions as `h1 + i×h2`
- `h1` = FNV-1a hash of the key
- `h2` = derived from `h1` by mixing bits: `h1 >> 33 ^ h1`
- same false positive rate as K independent hash functions

## Why deletion is impossible
- you cannot unset a bit on `remove` — that bit might be shared by another key
- removing "alice" would clear bit 7, but "bob" also uses bit 7 → `check("bob")` breaks
- solution: **Counting Bloom Filter** — store counts instead of bits, decrement on remove (uses more memory)

## Complexity
- **insert**: O(k) — compute k positions, set k bits
- **check**: O(k) — compute k positions, check k bits
- **space**: O(m) bits — fixed, independent of number of insertions

---

# More Information

## Usecase

### 1. Google Bigtable / LevelDB / RocksDB
- every SSTable (on-disk sorted file) has a Bloom filter
- before reading a file from disk to find a key, check the Bloom filter first
- if the filter says "no" → skip the disk read entirely (saves expensive I/O)
- if the filter says "yes" → do the disk read (might be a false positive, but rare)
- used by Google, Facebook, Meta, and every major database engine

### 2. Google Chrome — Safe Browsing
- Chrome maintains a local Bloom filter of millions of malicious URLs
- when you visit a URL, Chrome checks the local filter first — O(1), no network call
- if the filter says "maybe malicious" → Chrome makes a real network check to confirm
- the filter saves the network round-trip for 99%+ of safe URLs

### 3. Akamai CDN — one-hit wonders
- Akamai found that ~75% of objects in their CDN cache were only requested once ("one-hit wonders")
- caching them wastes memory and evicts more useful objects
- solution: only cache an object if it's been requested before — check a Bloom filter first
- reduced cache misses by 15% in production

### 4. Bitcoin / Ethereum — SPV wallets
- lightweight wallets can't store the full blockchain
- they send a Bloom filter of their addresses to full nodes
- the full node filters transactions and only sends ones that match
- reduces bandwidth by orders of magnitude for mobile wallets

### 5. Apache Cassandra — partition filtering
- Cassandra stores a Bloom filter per SSTable for each partition key
- before seeking on disk, check all Bloom filters to find which SSTables might contain the key
- dramatically reduces unnecessary disk seeks in read-heavy workloads

---

## Facts

### Invented in 1970 — one of the oldest probabilistic data structures
- invented by Burton Howard Bloom in his 1970 paper *"Space/Time Trade-offs in Hash Coding with Allowable Errors"*
- predates most of the internet, personal computers, and modern databases
- the insight was radical at the time: deliberately allowing errors to save memory

### The false positive rate formula is exact
- `p = (1 - e^(-kn/m))^k` is the exact false positive probability
- the simplified rule "10 bits per element → 1% FPR" comes directly from plugging the optimal k into this formula
- the optimal k that minimizes false positives for given m and n is exactly `k = (m/n) × ln2 ≈ 0.693 × m/n`

### Deletion is the classic limitation — but it's been solved
- **Counting Bloom Filter** (Fan et al., 1998): replace each bit with a small counter, decrement on delete — 3-4x more memory
- **Cuckoo Filter** (Fan et al., 2014): supports deletion, better cache performance, similar false positive rate — preferred in modern systems
- Redis uses Cuckoo Filters (via the RedisBloom module) precisely because they support deletion

### False positive rate grows as you overfill
- the formulas assume you insert exactly `n` elements
- if you insert 2× the expected elements, the false positive rate explodes — can exceed 50%
- unlike a HashMap which just resizes, a Bloom filter cannot resize without rebuilding from scratch

### Used in every major programming language's standard library — indirectly
- Python's `set` membership test is O(1) but stores full keys
- databases like PostgreSQL, MySQL, and SQLite use Bloom filters internally for query optimization
- you interact with Bloom filters every time you use Google Chrome, visit a CDN-served website, or use a database

---

## Tradeoffs vs previous stages

| | HashMap (stages 1–8) | Consistent Hashing (stage 9) | Bloom Filter (stage 10) |
|---|---|---|---|
| Stores values | Yes | No (routes to server) | **No** |
| Stores keys | Yes | Server names only | **No** |
| Exact answers | Yes | Yes | **No — probabilistic** |
| False negatives | No | No | **Never** |
| False positives | No | No | **Possible** |
| Deletion | Yes | Yes | **Not directly** |
| Memory | O(n) grows with n | O(V×N) | **O(m) fixed** |
| Lookup | O(1) average | O(log V×N) | **O(k) constant** |
| Use case | Store and retrieve data | Route keys to machines | **Test set membership cheaply** |
