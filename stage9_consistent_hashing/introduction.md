# Consistent Hashing

## Introduction
- all previous stages are about collision resolution — how to store keys in a single table on one machine
- consistent hashing is fundamentally different: it decides **which machine** among many should own a key
- normal `key % N` breaks when N changes — adding or removing a server reshuffles almost every key
- consistent hashing solves this: when a server is added or removed, only `1/N` of keys move

## How it works
- imagine all possible hash values (0 to 2^32-1) arranged in a **circle** (the ring)
- each server is placed at multiple positions on the ring via **virtual nodes**
- a key hashes to a position on the ring, then walks clockwise to the first virtual node — that server owns the key
- when a server is removed, its virtual nodes disappear and its keys naturally fall to the next clockwise server

## Virtual nodes
- placing each physical server at only 1 position causes uneven load — one server may own 70% of the ring
- the fix: each server spawns V virtual nodes at different positions (e.g. `"server-A#0"`, `"server-A#1"`, ...)
- with V=100+, the positions average out and each server owns roughly `1/N` of the ring
- more virtual nodes = more even distribution, but more memory and slower add/remove

## Key routing
```
get_node("user:1234"):
  hash("user:1234") → position 650M
  find first virtual node at position >= 650M → "server-B#3" at 800M
  return "server-B"
```

## Complexity
- **get_node**: O(log V×N) — binary search in a sorted map of V×N virtual nodes
- **add_node**: O(V log V×N) — insert V virtual nodes into the sorted map
- **remove_node**: O(V log V×N) — erase V virtual nodes from the sorted map
- **space**: O(V×N) — one entry per virtual node in the map

---

# More Information

## Usecase

### 1. Amazon Dynamo (2007)
- consistent hashing was popularized by Amazon's Dynamo paper (SOSP 2007)
- Dynamo is the storage system behind Amazon's shopping cart — it must stay available even when servers fail
- consistent hashing means a server failure only affects the keys it owned, not the entire dataset
- Dynamo uses 100–200 virtual nodes per server for even distribution

### 2. Apache Cassandra
- Cassandra (Facebook, 2008, now Apache) uses consistent hashing for its distributed ring architecture
- each Cassandra node owns a token range on the ring — consistent hashing determines which node stores which rows
- used by Apple (75,000+ nodes), Netflix, Discord, Instagram

### 3. Memcached / Redis Cluster
- Memcached clients use consistent hashing to distribute cache keys across multiple cache servers
- without it, adding a cache server invalidates most of the cache (thundering herd problem)
- Redis Cluster uses a variant called hash slots (16384 slots) derived from the same idea

### 4. Content Delivery Networks (Akamai, Cloudflare)
- CDNs use consistent hashing to route requests to edge servers
- a URL hashes to a specific edge node — the same URL always hits the same cache
- when an edge node goes down, only its URLs are re-routed, not the entire CDN

### 5. Load balancers (Nginx, HAProxy, AWS ALB)
- consistent hashing in load balancers enables **session affinity** — requests from the same client always reach the same backend
- useful for stateful services where session data lives on a specific server

---

## Facts

### Invented in 1997 — predates most distributed systems you know
- invented by David Karger, Eric Lehman, Tom Leighton, Rina Panigrahy, Matthew Levine, and Daniel Lewin at MIT
- paper: *"Consistent Hashing and Random Trees"*, STOC 1997
- Daniel Lewin later co-founded Akamai Technologies and deployed consistent hashing at internet scale
- Lewin was tragically killed on American Airlines Flight 11 on September 11, 2001

### The `key % N` problem
- with normal modular hashing, adding 1 server to a pool of N servers remaps `N/(N+1)` of all keys — almost everything moves
- with consistent hashing, only `1/(N+1)` of keys move — only the keys that were owned by the new server's neighborhood
- this difference is catastrophic for cache systems: `key % N` causes a **cache stampede** when servers change

### Virtual nodes were added later
- the original 1997 paper used real nodes only — uneven load was a known problem
- virtual nodes (vnodes) were introduced in practice by systems like Dynamo and Cassandra to fix load skew
- the number of virtual nodes is a tunable tradeoff: more = even load, slower topology changes

### Rendezvous hashing — a simpler alternative
- invented the same year (1997) independently by Thaler and Ravishankar
- instead of a ring, each server scores `hash(key + server_name)` and the highest score wins
- simpler to implement, same consistency guarantee, but O(N) lookup vs O(log N) for consistent hashing
- used in some CDN and load balancer implementations

### Jump consistent hashing (Google, 2014)
- Google published a simpler, faster variant that uses no ring at all — just a loop and a hash
- only 5 lines of code, O(ln N) time, perfectly uniform distribution
- limitation: servers must be added/removed in order (no arbitrary removal)
- used inside Google's infrastructure for shard assignment

---

## Tradeoffs vs previous stages

| | HashMap (stages 1–8) | Consistent Hashing (stage 9) |
|---|---|---|
| Purpose | Store key→value on one machine | Route keys to the right machine |
| Returns | The value | The server name |
| Keys move on resize | Most keys (rehash) | Only 1/N keys |
| Node failure impact | Entire table lost | Only that server's keys affected |
| Lookup complexity | O(1) average | O(log V×N) |
| Use case | Single-machine storage | Distributed systems |
