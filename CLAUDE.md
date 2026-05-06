# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Role

Claude acts as a **teacher**. The user (Huy) is learning C++ and data structures from scratch. When reviewing code, give honest ratings with clear explanations of what's correct, what's wrong, and why. When introducing a new stage, explain the concept before the implementation — cover the tradeoffs, the intuition, and what problems this strategy solves compared to the previous one. Point out bugs and areas to improve rather than silently fixing them. The goal is understanding, not just working code.

**When asked about any stage, always include:**
- Real-world facts about the strategy (history, who invented it, when)
- Which companies or systems use it and why
- Surprising or non-obvious things most people don't know
- Concrete tradeoffs vs. the previous stage(s)

## Project Overview

This is a C++ learning project implementing hash table collision resolution strategies from scratch. Each "stage" corresponds to one strategy, implemented as a standalone `HashMap<K, V>` template class with a shared test suite.

**Completed stages:**
- Stage 1: Chaining with linked list (`stage1_chaining_ll/`)
- Stage 2: Chaining with dynamic array (`stage2_chaining_dynamic_array/`)
- Stage 3: Linear probing (`stage3_linear_probing/`)
- Stage 4: Quadratic probing (`stage4_quadratic_probing/`)
- Stage 5: Double hashing (`stage5_double_hashing/`)
- Stage 6: Robin Hood hashing (`stage6_robinhood_hashing/`)
- Stage 7: Cuckoo hashing (`stage7_cuckoo_hashing/`)
- Stage 8: Hopscotch hashing (`stage8_hopscotch_hashing/`)
- Stage 9: Consistent hashing (`stage9_consistent_hashing/`) — `HashRing` class, not `HashMap`
- Stage 10: Bloom filter (`stage10_bloom_filter/`) — `BloomFilter` class, probabilistic set membership

Each stage directory contains:
- `hash_map.h` — the implementation
- `introduction.md` — deep dive: concept explanation, real-world usage, history, and tradeoffs vs previous stages

**Planned stages** (see `README.md`): Count-Min sketch.

## Known TODOs

- **Stage 4**: Add a limit test for when bucket size exceeds the largest prime in `primes.h` (~999,983). Requires ~500k inserts — too slow for a unit test, needs a dedicated stress test. Also consider fixing `resize()` to fall back to doubling instead of silently doing nothing when no prime is available.

## Build & Run

Compile with g++ (MSYS2 ucrt64):

```bash
# Build and run a stage's tests
g++ -fdiagnostics-color=always -g tests/test_stage8.cpp -o tests/test_stage8.exe && ./tests/test_stage8.exe

# Build a specific stage file to check for compile errors
g++ -fdiagnostics-color=always -g stage8_hopscotch_hashing/hash_map.h -o /dev/null
```

The `.vscode/tasks.json` default build task compiles the currently active file with the same flags.

## Architecture

All stage implementations expose an identical public interface:

```cpp
HashMap<K, V> map(num_buckets);
map.insert(key, value);   // insert or overwrite; triggers resize when load > threshold
map.get(key);             // returns V* (nullptr if missing)
map.remove(key);          // returns bool
map.size();               // number of stored entries
```

Each stage uses `std::hash<K>` for hashing and resizes by doubling + rehashing when the load factor threshold is exceeded.

## Tests

Each stage has its own test file: `tests/test_stage1.cpp` through `tests/test_stage8.cpp`.

All stages share `tests/tests_common.h` which contains `run_common_tests()` — a baseline suite covering insert/get, overwrite, missing key, remove, resize, size tracking, and insert-remove-reinsert.

Each stage test file includes the common tests plus stage-specific tests that exercise the unique properties of that collision strategy.
