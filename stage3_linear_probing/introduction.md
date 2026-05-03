# Linear Probing

## Introduction
- the seperate chaining (both linked list and vector), for each node, its will be in a seperation heap allocation
- linked list is random we all know that, but vector also need another allocation to store the collision

## collision handling
- if there is a collision, the insert element will go to the next index, insert into **tombstone** or **empty slot**
-> colliosn we "borrow" the next index 
-> no extra memory solution

## Tombstone
- we need tombstone for **deleted element** because if we actually deleted it, it will break the whole **cluster** of colliding
- Tombstone make lookup (get) and insert slow down -> resize can delete all the tombstone (**fully rehash**)

## Complexity
- **insert, get, remove**: O(1) in average, O(n) in worst case
    - worst case happen when all the key collide and go to the same slot
- **space**: O(n) -> no extra memory

# More information

## Usecase:

### 1. CPython dict (3.6+)
- switch from chaining to open addressing -> memory + cache efficiency
- python dict auto ordered by insertion by the nature of open addressing

### 2. PHP array
- the most used ordered hash map 

### 3. V8 (Javascript engine)
- object lookup in Google's JS engine -> critical path of every js execution

### 4. Ruby hash
- linear probing in Ruby 2.4


## Fact
- for the **load factor** calculator, its coming from **knuth's 1963 formula**
    - load factor = 0.7 -> 2.17 CPU cycles
    - load factor = 0.9 -> 5.5 CPU cycles
- **python probe sequence** is not pure linear
    - Cpython use `i = (5 * i + 1 + perturb)%size`, `perturb` derived from the hash