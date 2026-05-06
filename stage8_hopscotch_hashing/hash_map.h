/*
    assume that the hop size (neighborhood size) is 8 -> using uint8_t for 8 bit representation
    same with linear probing but just H neighborhood
*/

#pragma once
#include<functional>
#include<vector>
#include<cstdint>
#include<stdexcept>

template<typename K, typename V>

class HashMap {
    struct Node {
        K key;
        V value;
        bool is_occupied;

        Node(const K& key, const V& value): key(key), value(value), is_occupied(true) {}
        Node(): is_occupied(false) {}
    };

    static const int H = 8;
    std::vector<Node> bucket;
    std::vector<uint8_t> hop_info;
    size_t count;
    float max_load;

    size_t bucket_index(const K& key) const {
        return std::hash<K>{}(key) % bucket.size();
    }

public:
    HashMap(size_t cap = 16, float max_load = 0.75): max_load(max_load), count(0) {
        bucket.resize(cap, Node());
        hop_info.resize(cap, 0);
    }

    ~HashMap() {}

    size_t size() const {
        return count;
    }
    /*
        1. If retry limit exceeded, throw.
        2. If load factor exceeds max_load, resize.
        3. Scan the H-slot neighborhood: if key exists, overwrite. Track first empty slot found.
        4. If an empty slot was found in the neighborhood, place the key there and update hop_info.
        5. Otherwise, linear-probe beyond the neighborhood to find any empty slot.
        6. Displace entries backward: find an entry near empty_index that can legally move to empty_index
           (its home is within H of empty_index), swap it, update hop_info. Repeat until empty_index
           is inside the home's neighborhood.
        7. If no entry can be displaced (stuck), resize and retry.
        8. Place the key at empty_index, update hop_info, increment count.
    */
    void insert(const K& key, const V& value, int retry = 0) {
        if(retry > 3) {
            throw std::runtime_error("hopscotch: too many resizes");
        }

        if((float)count/bucket.size() > max_load){
            resize();
        }

        size_t home_index = bucket_index(key);
        size_t empty_index = bucket.size();
        for(size_t step = 0; step < H; step++) {
            size_t current_index = (home_index + step) % bucket.size();
            //overwrite the inserted value
            if(bucket[current_index].is_occupied && bucket[current_index].key == key){
                bucket[current_index].value = value;
                return;
            }

            if(!bucket[current_index].is_occupied && empty_index == bucket.size()) {
                empty_index = current_index;
            }
        }

        //there is an empty index in neighborhood
        if(empty_index != bucket.size()) {
            bucket[empty_index] = Node(key, value);
            size_t distance = (empty_index - home_index + bucket.size()) % bucket.size();
            hop_info[home_index] |= (1 << distance);
            count++;
            return;
        }

        // no empty index in neighborhood, find the earliest empty index beyond
        empty_index = bucket.size();
        for(size_t step = 0; step < bucket.size(); step++){
            size_t current_index = (home_index + step) % bucket.size();
            if(!bucket[current_index].is_occupied) {
                empty_index = current_index;
                break;
            }
        }

        while((empty_index - home_index + bucket.size()) % bucket.size() >= H) {
            bool found = false;
            for(size_t rstep = 1; rstep < H; rstep++){
                size_t current_index = (empty_index - rstep + bucket.size()) % bucket.size();
                if(!bucket[current_index].is_occupied) continue;

                size_t current_home_index = bucket_index(bucket[current_index].key);
                size_t distance = (empty_index - current_home_index + bucket.size()) % bucket.size();

                if(distance < H) {
                    size_t dist_from_home = (current_index - current_home_index + bucket.size()) % bucket.size();
                    bucket[empty_index] = bucket[current_index];
                    bucket[current_index].is_occupied = false;

                    //move from current index to empty index
                    hop_info[current_home_index] &= ~(1 << dist_from_home);
                    hop_info[current_home_index] |= (1 << distance);

                    empty_index = current_index;
                    found = true;
                    break;
                }
            }

            if(!found) {
                resize();
                insert(key, value, retry+1);
                return;
            }
        }

        // empty_index is now within the neighborhood — place the key
        bucket[empty_index] = Node(key, value);
        size_t dist = (empty_index - home_index + bucket.size()) % bucket.size();
        hop_info[home_index] |= (1 << dist);
        count++;
    }

    V* get(const K& key) {
        size_t home_index = bucket_index(key);
        uint8_t info = hop_info[home_index];

        for(size_t step = 0; step < H; step++) {
            if(info & (1 << step)) {
                size_t current_index = (home_index + step) % bucket.size();
                if(bucket[current_index].key == key) {
                    return &bucket[current_index].value;
                }
            }
        }
        return nullptr;
    }

    bool remove(const K& key) {
        size_t home_index = bucket_index(key);
        uint8_t info = hop_info[home_index];

        for(size_t step = 0; step < H; step++) {
            if(info & (1 << step)) {
                size_t current_index = (home_index + step) % bucket.size();
                if(bucket[current_index].key == key) {
                    bucket[current_index].is_occupied = false;
                    hop_info[home_index] &= ~(1 << step);
                    count--;
                    return true;
                }
            }
        }
        return false;
    }
private:
    void resize() {
        std::vector<Node> old_bucket = std::move(bucket);
        bucket.assign(old_bucket.size() * 2, Node());
        hop_info.assign(old_bucket.size() * 2, 0);
        count = 0;

        for(auto& node : old_bucket) {
            if(node.is_occupied) {
                insert(node.key, node.value);
            }
        }
    }
};
