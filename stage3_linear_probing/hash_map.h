#pragma once
#include <vector>
#include <optional>
#include <functional>

template< typename K, typename V>

class HashMap {
    struct Node {
        K key;
        V value;
        bool is_tombstone;

        Node(const K& key, const V& value): key(key), value(value), is_tombstone(false) {}
    };

    std::vector<std::optional<Node>> bucket;
    size_t count;
    size_t tombstone_count;
    float max_load;

    size_t bucket_index(const K& key) const {
        size_t size = bucket.size();
        size_t hash_value = std::hash<K>{}(key) % size;

        return hash_value;
    }

public:
    HashMap(size_t cap = 16, float max_load = 0.75f): count(0), tombstone_count(0), max_load(max_load) {
        bucket.resize(cap, std::nullopt);
    }

    ~HashMap() {}

    size_t size() const {
        return count;
    }

    /*
        1. probe through everything
        2. track the first tombstone (nearest) compare to the hash index
        3. if there is a key -> overwrite 
        4. if hit a nullopt first -> insert at the nearest tombstone or the nullopt
    */
    void insert(const K& key, const V& value) {
        if((float) (count + tombstone_count)/bucket.size() > max_load){
            resize();
        }
        size_t size = bucket.size();

        std::optional<size_t> nearest_tombstone_index = std::nullopt;
        size_t index = bucket_index(key);
        while(bucket[index].has_value()) {
            if(!nearest_tombstone_index.has_value() && bucket[index] -> is_tombstone == true) {
                nearest_tombstone_index = index;
            }

            if(bucket[index] -> key == key && bucket[index] -> is_tombstone == false){
                bucket[index] -> value = value;
                return;
            }
            index = (index + 1) % size;
        }

        if(nearest_tombstone_index.has_value()){
            bucket[nearest_tombstone_index.value()] = Node(key, value);
            tombstone_count--;
        } else {
            bucket[index] = Node(key, value);
        }
        count++;
    }

    V* get(const K& key){
        size_t index = bucket_index(key);
        size_t size = bucket.size();

        while(bucket[index].has_value()) {
            if(bucket[index] -> key == key && bucket[index] -> is_tombstone == false){
                return &bucket[index] -> value;
            }
            index = (index+1) % size;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t index = bucket_index(key);
        size_t size = bucket.size();

        while(bucket[index].has_value()) {
            if(bucket[index] -> key == key && bucket[index] -> is_tombstone == false){
                bucket[index] -> is_tombstone = true;
                tombstone_count++;
                count--;
                return true;
            }
            index = (index+1) % size;
        }

        return false;
    }

private:
    void resize() {
        std::vector<std::optional<Node>> new_bucket(bucket.size()*2, std::nullopt);
        for(size_t index = 0; index < bucket.size(); index++){
            if(bucket[index].has_value() && bucket[index] -> is_tombstone == false) {
                const K& key = bucket[index] -> key;
                size_t new_index = std::hash<K>{}(key) % new_bucket.size();
                //no tombstone
                while(new_bucket[new_index].has_value()) {
                    new_index = (new_index + 1) % new_bucket.size();
                }
                new_bucket[new_index] = bucket[index];
            }
        }

        bucket = std::move(new_bucket);
        tombstone_count = 0;
    }
};