#pragma once
#include <vector>
#include <functional>

template <typename K, typename V>

class HashMap {
    std::vector<std::vector<std::pair<K, V>>> bucket;
    size_t count;
    float max_load = 0.75f;

    size_t bucketIndex(const K& key){
        size_t size = bucket.size();
        size_t hash_value =  std::hash<K>{}(key) % size;
        
        return hash_value;
    }

public:
    HashMap(size_t cap = 16, float max_load = 0.75f): count(0), max_load(max_load) {
        bucket.resize(cap);
    }

    ~HashMap() {}

    size_t size(){
        return count;
    }

    void insert(const K& key, const V& value) {
        if((float) (count + 1)/bucket.size() > max_load) {
            resize();
        }

        size_t index = bucketIndex(key);

        //check exist
        for(auto& [cur_key, cur_value]: bucket[index]) {
            if(key == cur_key){
                cur_value = value;
                return;
            }
        }

        bucket[index].push_back({key, value});
        count++;
    }

    V* get(const K& key) {
        size_t index = bucketIndex(key);
        for(auto& [cur_key, cur_value]: bucket[index]){
            if(key == cur_key){
                return &cur_value;
            }
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t index = bucketIndex(key);

        for(size_t i = 0; i < bucket[index].size(); i++) {
            if(key == bucket[index][i].first){
                bucket[index][i] = bucket[index].back();
                bucket[index].pop_back();
                count--;
                return true;
            }
        }

        return false;
    }

private:
    void resize() {
        std::vector<std::vector<std::pair<K, V>>> new_bucket(bucket.size() * 2);
        size_t size = new_bucket.size();

        for(auto& vec: bucket){
            for(auto& data_point: vec){
                size_t new_index = std::hash<K>{}(data_point.first) % size;
                new_bucket[new_index].push_back(data_point);
            }
        }

        bucket = std::move(new_bucket);
    }
};