# Double hashing

## Introduction
- it is using open addressing 
- one of the oldest hash table techniques still use today
- using 2 hash functions to generate the path of collde
## Collision handling
- `next_index = (original_index + i * hash_2)`
- There are 2 ways of handle this:
### 1. bucket size (n) is a prime number
- with n is a prime -> gcd(hash2(key), n) guaranteed to be 1
- hash2 must not be = 0 -> if 0 then infinite loop

### 2. bucket size (n) is 2^x
- we can make hash2(key) as an odd number -> always guaranteed gcd = 1

## Complexity
- **insert, get, remove**: O(1) average, O(n) worst case
- **space**: O(n) — no extra memory per element

# More information

## Use case

### 1. Python set (before 3.6)
- pertubation based variant of double hashing
- after 3.6, python use compact array with insertion-order preservation

### 2. Cooku hashing
- build on the idea of two different hashing algorithm

### 3. Cryptographic hash tables
- security software use double hashing becasue 2 independent hash functions -> harder for attacker to create keys

### 4. Google dense hashmap (sparase hash)
- use open addressing with prob sequence inspired by double hashing -> fastest hashmap ever benchmark in Cpp

## Fact
- h2 dont need to be different than h1, we can use complementary of hash 1 t create hash 2
- slower than linear probing, becasue the step is hash -> cpu performance gonna miss cache every single step -> much slower

- at low load factor, linear probing often wins benchmark despite the clustering problem