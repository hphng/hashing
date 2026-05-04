#pragma once
#include<vector>
#include<functional>
#include<algorithm>
#include "../utility/primes/primes.h"
#include<iostream>

template<typename K, typename V>

class HashMap {
    enum class State { EMPTY, HAS_VALUE, TOMBSTONE };
    struct Node {
        K key;
        V value;
        State state;

        Node(const K& key, const V& value): key(key), value(value), state(State::HAS_VALUE) {}
        Node(): state(State::EMPTY) {}
    };
    std::vector<Node> bucket;
    size_t count;
    size_t tombstone_count;
    size_t prime;
    float max_load;

    std::pair<size_t, size_t> bucket_index(const K& key) const {
        size_t hash1 = std::hash<K>{}(key);
        size_t hash2 = prime - (hash1 % prime);

        return {hash1, hash2};
    }

public:
    HashMap(size_t cap = 11, float max_load = 0.7f): max_load(max_load), count(0), tombstone_count(0), prime(7) {
        bucket.resize(cap, Node());
    }

    ~HashMap() {}

    size_t size() const {
        return count;
    }

    /*
        double hashing guaranteed that it go through every distinct n element
        1. check already inserted key -> overwrite
        2. track earliest tombstone -> add to empty or tombstone whichever come first
    */
    void insert(const K& key, const V& value) {
        if(float(count + tombstone_count)/bucket.size() > max_load) {
            resize();
        }

        auto [hash1, hash2] = bucket_index(key);

        size_t index = hash1%bucket.size();
        size_t original_index = index;
        size_t offset = 1;
        size_t earliest_tombstone_index = bucket.size();

        while(bucket[index].state != State::EMPTY) {
            //check already inserted key
            if(bucket[index].key == key && bucket[index].state == State::HAS_VALUE) {
                bucket[index].value = value;
                return;
            }

            if(earliest_tombstone_index == bucket.size() && bucket[index].state == State::TOMBSTONE){
                earliest_tombstone_index = index;
            }

            index = (original_index + offset * hash2) % bucket.size();
            offset++;
        }

        if(earliest_tombstone_index != bucket.size()) {
            bucket[earliest_tombstone_index] = Node(key, value);
            tombstone_count--;
        } else {
            bucket[index] = Node(key, value);
        }
        count++;
    }

    V* get(const K& key) {
        auto [hash1, hash2] = bucket_index(key);

        size_t index = hash1%bucket.size();
        size_t original_index = index;
        size_t offset = 1;

        while(bucket[index].state != State::EMPTY) {
            if(bucket[index].key == key && bucket[index].state == State::HAS_VALUE) {
                return &bucket[index].value;
            }

            index = (original_index + offset * hash2) % bucket.size();
            offset++;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        auto [hash1, hash2] = bucket_index(key);

        size_t index = hash1%bucket.size();
        size_t original_index = index;
        size_t offset = 1;

        while(bucket[index].state != State::EMPTY) {
            if(bucket[index].key == key && bucket[index].state == State::HAS_VALUE) {
                bucket[index].state = State::TOMBSTONE;
                count--;
                tombstone_count++;
                return true;
            }

            index = (original_index + offset * hash2) % bucket.size();
            offset++;
        }

        return false;
    }

private:
    void resize() {
        size_t current_size = bucket.size();
        if(current_size >= PRIMES[PRIMES_COUNT-1]) {
            std::cerr << "array size too big!";
            return;
        }

        size_t next_index = std::upper_bound(PRIMES, PRIMES + PRIMES_COUNT, current_size) - PRIMES;
        prime = current_size;
        size_t next_size = PRIMES[next_index];

        std::vector<Node> new_bucket (next_size, Node());

        for(size_t index = 0; index < bucket.size(); index++) {
            if(bucket[index].state == State::HAS_VALUE) {
                K current_key = bucket[index].key;

                auto [hash1, hash2] = bucket_index(current_key);
                size_t new_index = hash1%next_size;
                size_t original_index = new_index;
                size_t offset = 1;

                while(new_bucket[new_index].state != State::EMPTY) {
                    new_index = (original_index + offset * hash2) % next_size;
                    offset++;
                }

                new_bucket[new_index] = bucket[index];
            }
        }

        bucket = std::move(new_bucket);
        tombstone_count = 0;
    }
};