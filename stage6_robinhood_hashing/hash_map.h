#pragma once
#include<vector>
#include<functional>

template<typename K, typename V>

class HashMap {
    enum class State {EMPTY, HAS_VALUE};
    struct Node {
        K key;
        V value;
        State state;

        Node(const K& key, const V& value): key(key), value(value), state(State::HAS_VALUE) {}
        Node(): state(State::EMPTY) {}
    };

    std::vector<Node> bucket;
    size_t count;
    float max_load;

    size_t bucket_index(const K& key) const {
        size_t size = bucket.size();
        size_t hash_value = std::hash<K>{}(key) % size;

        return hash_value;
    }

public:
    HashMap(size_t cap = 16, float max_load = 0.75): max_load(max_load), count(0) {
        bucket.resize(cap, Node());
    }

    ~HashMap() {}

    size_t size() const {
        return count;
    }

    /*
        same with linear probing, but need to find DIB (Distance from inital bucket). 
        swap the current and the insert element
    */
    void insert(const K& key, const V& value) {
        if((float) count/bucket.size() > max_load){
            resize();
        }

        size_t size = bucket.size();
        size_t index = bucket_index(key);
        size_t original_index = index;

        K current_key = key;
        V current_value = value;

        while(bucket[index].state != State::EMPTY) {
            //overwrite existed key
            if(bucket[index].key == current_key && bucket[index].state == State::HAS_VALUE) {
                bucket[index].value = current_value;
                return;
            }

            size_t insert_dib = (index - original_index + size) % size;

            size_t current_original_index = bucket_index(bucket[index].key);
            size_t current_dib = (index - current_original_index + size) % size;

            if(insert_dib > current_dib) {
                K tmp_key = current_key;
                V tmp_value = current_value;

                current_key = bucket[index].key;
                current_value = bucket[index].value;

                bucket[index] = Node(tmp_key, tmp_value);
                //start from original new index
                original_index = current_original_index;
            }

            index = (index + 1) % size;
        }

        bucket[index] = Node(current_key, current_value);
        count++;
    }


    V* get(const K& key) {
        size_t size = bucket.size();
        size_t index = bucket_index(key);

        while(bucket[index].state != State::EMPTY) {
            if(bucket[index].key == key && bucket[index].state == State::HAS_VALUE){
                return &bucket[index].value;
            }
            index = (index + 1) % size;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t size = bucket.size();
        size_t index = bucket_index(key);

        while(bucket[index].state != State::EMPTY) {
            if(bucket[index].key == key) {
                // backward shift: pull subsequent keys back until EMPTY or DIB=0
                while(true) {
                    size_t next = (index + 1) % size;
                    if(bucket[next].state == State::EMPTY || (next - bucket_index(bucket[next].key) + size) % size == 0) {
                        bucket[index].state = State::EMPTY;
                        break;
                    }
                    bucket[index] = bucket[next];
                    index = next;
                }
                count--;
                return true;
            }
            index = (index + 1) % size;
        }

        return false;
    }
private:
    /*
        double the size and rehash with robin hood ordering
    */
    void resize() {
        std::vector<Node> new_bucket(bucket.size() * 2, Node());
        size_t new_size = new_bucket.size();

        for(size_t i = 0; i < bucket.size(); i++) {
            if(bucket[i].state == State::HAS_VALUE) {
                size_t new_index = std::hash<K>{}(bucket[i].key) % new_size;
                size_t new_original_index = new_index;

                K current_key = bucket[i].key;
                V current_value = bucket[i].value;

                while(new_bucket[new_index].state != State::EMPTY) {
                    size_t insert_dib = (new_index - new_original_index + new_size) % new_size;

                    size_t current_original_index = std::hash<K>{}(new_bucket[new_index].key) % new_size;
                    size_t current_dib = (new_index - current_original_index + new_size) % new_size;

                    if(insert_dib > current_dib) {
                        K tmp_key = current_key;
                        V tmp_value = current_value;

                        current_key = new_bucket[new_index].key;
                        current_value = new_bucket[new_index].value;

                        new_bucket[new_index] = Node(tmp_key, tmp_value);
                        new_original_index = current_original_index;
                    }

                    new_index = (new_index + 1) % new_size;
                }

                new_bucket[new_index] = Node(current_key, current_value);
            }
        }

        bucket = std::move(new_bucket);
    }
};