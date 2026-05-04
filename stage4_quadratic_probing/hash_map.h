/*
Note: we need our bucket size to be prime
-> run the find_primes_script.cpp first, then run/read this
*/

#pragma once
#include<vector>
#include<functional>
#include<stddef.h>
#include "primes.h"
#include<iostream>
#include<algorithm>

template<typename K, typename V>

class HashMap {
    enum class State {EMPTY, HAS_VALUE, TOMBSTONE};
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
    float max_load;

    size_t bucket_index(const K& key) const {
        size_t size = bucket.size();
        size_t hash_value = std::hash<K>{}(key) % size;

        return hash_value;
    }

public:
    HashMap(size_t cap = 11, float max_load = 0.5f): count(0), tombstone_count(0), max_load(max_load) {
        bucket.resize(cap, Node());
    }

    ~HashMap() {}

    size_t size() const {
        return count;
    }

    /*
        1. check existed (loop through until meet an EMPTY). if yes, overwrite the old one
        2. write to an EMPTY or TOMBSTONE, whichever come first
    */
    void insert(const K& key, const V& value) {
        if(float(count + tombstone_count)/bucket.size() > max_load) {
            resize();
        }

        size_t index = bucket_index(key);
        size_t original_index = index;
        size_t quad = 1;
        size_t earliest_tombstone_index = bucket.size();

        while(bucket[index].state != State::EMPTY) {
            //check existed
            if(bucket[index].key == key && bucket[index].state == State::HAS_VALUE) {
                bucket[index] = Node(key, value);
                return;
            }

            if(earliest_tombstone_index == bucket.size() && bucket[index].state == State::TOMBSTONE) {
                earliest_tombstone_index = index;
            }

            index = (original_index + quad * quad) % bucket.size();
            quad++;
        }

        if(earliest_tombstone_index != bucket.size()){
            bucket[earliest_tombstone_index] = Node(key, value);
            tombstone_count--;            
        } else {
            bucket[index] = Node(key, value);
        }
        count++;
    }

    V* get(const K& key) {
        size_t index = bucket_index(key);
        size_t original_index = index;
        size_t quad = 1;
        while(bucket[index].state != State::EMPTY) {
            if(bucket[index].state == State::HAS_VALUE && bucket[index].key == key) {
                return &bucket[index].value;
            }

            index = (original_index + quad * quad) % bucket.size();
            quad++;
        }

        return nullptr;
    }
    bool remove(const K& key) {
        size_t index = bucket_index(key);
        size_t original_index = index;
        size_t quad = 1;

        while(bucket[index].state != State::EMPTY) {
            if(bucket[index].state == State::HAS_VALUE && bucket[index].key == key) {
                bucket[index].state = State::TOMBSTONE;
                count--;
                tombstone_count++;
                return true;
            }

            index = (original_index + quad * quad) % bucket.size();
            quad++;
        }

        return false;
    }
private:
    void resize() {
        size_t current_size = bucket.size();
        if(current_size > PRIMES[PRIMES_COUNT-1]) {
            std::cerr << "cant resize the array, too big";
            return;
        }

        const size_t* next_size_ptr = std::upper_bound(PRIMES, PRIMES + PRIMES_COUNT, current_size);
        const size_t next_size = *next_size_ptr;

        std::vector<Node> new_bucket(next_size, Node());

        for(size_t index = 0; index < current_size; index++) {
            if(bucket[index].state == State::HAS_VALUE) {
                K current_key = bucket[index].key;
                size_t next_index = std::hash<K>{}(current_key) % next_size;
                size_t original_next_index = next_index;

                //assign { key, value } into new bucket
                size_t quad = 1;
                while(new_bucket[next_index].state != State::EMPTY) {
                    next_index = (original_next_index + quad * quad) % next_size;
                    quad++;
                }

                new_bucket[next_index] = bucket[index];
            }
        }

        bucket = std::move(new_bucket);
        tombstone_count = 0;
    }
};