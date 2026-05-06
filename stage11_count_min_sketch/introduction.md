# Count-Min Sketch

## Introduction
- the Bloom filter (stage 10) answers "have I seen this key?" — Count-Min sketch answers "**how many times** have I seen this key?"
- it returns an **approximate frequency count**, not an exact one
- **no undercounting** — the estimate is always ≥ the true count
- **overcounting is possible** — hash collisions inflate counts, but the error is bounded
- uses a fixed 2D grid of counters — memory never grows even on an infinite stream of data

## How it works
- internally a **d × w counter grid**, all initialized to 0
- on `insert(key)`: for each row `d`, compute a column index and increment `grid[d][index]`
- on `estimate(key)`: compute the same column indices, return the **minimum** across all rows

```
grid (3 rows × 8 columns):

         0    1    2    3    4    5    6    7
row 0: [ 0,   0,   1,   0,   0,   1,   0,   0 ]   ← insert("alice") hit col 2, insert("bob") hit col 5
row 1: [ 0,   1,   0,   0,   1,   0,   0,   0 ]   ← insert("alice") hit col 1, insert("bob") hit col 4
row 2: [ 0,   0,   0,   1,   0,   0,   1,   0 ]   ← insert("alice") hit col 3, insert("bob") hit col 6

estimate("alice") → cols: 2, 1, 3 → values: 1, 1, 1 → min = 1 ✓
estimate("carol") → cols: 2, 4, 6 → values: 1, 1, 1 → min = 1 ✗ (overcount — never inserted)
```

**Why minimum?** Collisions can only inflate a bucket (someone else also hashes there), never deflate it. The true count is always ≤ any single row's value. The minimum across rows is the tightest estimate available.

## Parameters: epsilon and delta

The sketch is built from two accuracy parameters:

- **epsilon (ε)** — maximum error as a fraction of total insertions `N`
  - guarantee: `estimate ≤ true_count + ε × N`
  - ε = 0.01 with N = 1,000,000 → error at most 10,000
- **delta (δ)** — probability the error bound is violated
  - guarantee holds with probability `1 - δ`
  - δ = 0.01 → 99% of queries are within the error bound

These drive the grid dimensions:

```
width  (w) = ceil(e / ε)         e = Euler's number ≈ 2.718
depth  (d) = ceil(ln(1 / δ))     also = number of hash functions
```

With defaults ε = 0.01, δ = 0.01:
```
w = ceil(2.718 / 0.01) = 272 columns
d = ceil(ln(100))      = 5  rows
```
A 5 × 272 grid of counters — 1,360 integers total, regardless of how many items are inserted.

## Double hashing (same trick as stage 10)

Each row needs its own hash function. Using the Kirsch-Mitzenmacher approach, two base hashes produce all `d` column indices:

```
h1 = FNV-1a(key)
h2 = (h1 >> 33) ^ h1         // bit-mix of h1

column for row i = (h1 + i × h2) % w
```

Same formula as the Bloom filter — but here each `i` selects a **different row**, whereas in the Bloom filter each `i` selects a different bit position in one flat array.

## The additive error trap

The error bound `ε × N` is **additive**, not relative. This is critical:

- if a key appears 1,000,000 times out of N = 10,000,000 total: error ≤ 100,000 → relative error ≤ 10% (fine)
- if a key appears 5 times out of N = 1,000,000 total: error ≤ 10,000 → estimate could be 10,005 (useless)

Count-Min sketch is designed for **heavy hitters** — items with counts much larger than `ε × N`. For rare events, the noise floor dominates the signal.

## Complexity

- **insert**: O(d) — increment one counter per row
- **estimate**: O(d) — read one counter per row, take minimum
- **space**: O(w × d) counters — fixed, independent of number of insertions

---

# More Information

## Usecase

### 1. AT&T — network traffic analysis (the origin use case)
- Cormode worked at AT&T Research when he invented Count-Min sketch
- network routers see billions of packets per second — you cannot store exact counts for every IP/port pair
- Count-Min sketch identifies "elephant flows" (heavy hitters consuming most of the bandwidth) in real time
- fixed memory means it runs on router hardware with no dynamic allocation

### 2. Twitter — trending topics
- Twitter processes hundreds of thousands of tweets per second
- to find trending hashtags, you need approximate frequency counts across the stream
- Count-Min sketch tracks how often each hashtag appears in a sliding time window
- anything with count >> ε × N is a genuine trending topic, not noise

### 3. Apache Flink / Spark Streaming — real-time analytics
- both frameworks include Count-Min sketch in their streaming libraries
- used for: counting distinct events, identifying top-K items, rate limiting by user/IP
- the fixed memory footprint is the key advantage — stream processing jobs run for days/weeks

### 4. Redis — CMS module
- Redis ships a native `CMS` data type (via RedisBloom): `CMS.INCRBY`, `CMS.QUERY`, `CMS.MERGE`
- used for rate limiting, leaderboard approximation, and fraud detection at request time
- `CMS.MERGE` combines two sketches — you can split a stream across shards and merge the results

### 5. Databases — query optimization
- PostgreSQL and other databases use frequency sketches to estimate how often values appear in a column
- the query planner uses these estimates to choose join order and index strategy
- exact histograms would be too expensive to maintain; Count-Min sketch is updated incrementally

---

## Facts

### Invented in 2003 — and the name is not what you think
- invented by Graham Cormode and S. Muthu Muthukrishnan in their 2003 paper *"An Improved Data Stream Summary: The Count-Min Sketch and its Applications"*
- the name does not mean "count the minimum" — it means: **count** (the operation to insert) and **min** (the operation to query)
- Cormode was 25 when he published it; it is now one of the most cited papers in streaming algorithms

### It solves a problem that HashMaps cannot
- a HashMap gives exact counts but requires O(n) memory and cannot handle an unbounded stream
- Count-Min sketch gives approximate counts with O(1) memory — it never grows
- for systems that must run forever (network routers, ad servers, real-time dashboards) this is the only viable approach

### The sketch can be merged
- two Count-Min sketches built with the same parameters can be added together element-wise
- the merged sketch represents the combined stream — used to parallelize stream processing across shards
- Bloom filters support union (OR), but not addition — Count-Min sketch supports both

### Top-K detection is the killer application
- the "heavy hitters" problem: find all items whose frequency exceeds `φ × N`
- with Count-Min sketch: insert everything, then at query time find items whose estimate > `φ × N - ε × N`
- this is how Twitter finds trending topics, how ad systems detect click fraud, and how routers detect DDoS sources — all in real time with fixed memory

### Count-Min sketch always overcounts, never undercounts
- this asymmetry is intentional and exploited in practice
- for spam/fraud detection: overcounting means you occasionally block a legitimate request — undercounting means you miss fraud. Overcounting is the safer failure mode.
- for heavy-hitter detection: since you only care about items far above the noise floor, the overcount does not affect the answer

---

## Tradeoffs vs previous stages

| | Bloom Filter (stage 10) | Count-Min Sketch (stage 11) |
|---|---|---|
| Answers | "Is this in the set?" | "How many times have I seen this?" |
| Error type | False positives (wrong yes) | Overcounting (inflated count) |
| Error direction | Both ways possible | **One-way only — never undercounts** |
| Memory | O(m) bits | O(w × d) integers |
| Deletion | Not supported | Not supported (standard) |
| Use case | Set membership, deduplication | Frequency estimation, heavy hitters |
| Stores counts | No | Yes (approximate) |

Both use the same Kirsch-Mitzenmacher double hashing trick. Both use fixed memory regardless of stream size. The key difference: Bloom filter is a **probabilistic set**, Count-Min sketch is a **probabilistic counter**.
