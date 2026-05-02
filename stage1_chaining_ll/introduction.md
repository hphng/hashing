# Chaining with Linked list

## Introduction
- Basically a normal hash map
- using an array -> by hashing the key and put the pair into the hashmap

## Collision handling
- Since the collision will be based **hashing algorithm** and **size of that array** -> using linked list to handle collision in one element of array

## Load factor
- `Load factor` = N items/K slots
- if `load factor` <= **0.75** -> lookup will ~ O(1) 

## Complexity
- **insert, lookup, delete**: average case O(1), worst case O(n)
- **re-hash**: O(n)
- **space**: O(n+k): n elements + k emty slots in array

## More information

### 1. Hash flooding (HashDoS attack)
- if attacker known the hash function
- created key that all land the same bucket -> turning O(1) into O(n)
- break the server

### 2. 0.75 of Load factor (items/slots = N/K)
- the number from mathematical tradeoff:
    - at 0.75 -> **expected value** of comparison look up is 1.5
    - at 1.0 -> **EV** is 2.5 comparisons

### 3. `std:unordered_map` in cpp
- not always the fastest option
- linkedlist is chained an random in memory -> bad at cache-sensitive code
- using `absl::flat_hash_map` from Google or `ankerl::unordered_dense` (more clear on *stage 3*)