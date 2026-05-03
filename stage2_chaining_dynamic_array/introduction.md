# Chaining with dynamic array


## Problem with stage 1
- linked list -> every node is a seperate new allocation in the heap
- CPU fetch a **cache line** (64 bytes) at a time
- if next node is a random allocation -> CPU need to fectch another cache line -> **cache miss**
- each **cache miss** cost 100 CPU cycles
- each **L1 cache hit** cost only 5 CPU cycles -> 25x difference

## Introduction
- same algorithm with stage 1, but using **dynamic array** instead of **linked list**
- now nodes will next to each other in term of allocation in heap -> faster query 
- the hash logic, load factor (N/K), and resize strategy are the same

## Disadvantage
- `std::vector` on a 64-bit system is **24 bytes** (a pointer + size + capacity), even when empty
- 100k buckets -> 24MB for empty slots

-> waste more memory on the empty slot

# More information

## Popular usage

### 1. Google's dense hashmap
- countiguous storage + cache efficiency in hot path
- `unordered_map` in cpp using stage 1.
- google built `flat_hash_map` for stage 2 and most ultra low latency + performance sensitive project avoid `unordered_map`

### 2. Unreal engine/Unity (wtf??)
- at 60 fps -> cache missed is waste a lot of time, we trade of resouce with hash time




