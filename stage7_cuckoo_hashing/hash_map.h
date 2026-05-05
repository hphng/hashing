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

    std::vector<Node> bucket1;
    std::vector<Node> bucket2;

    size_t seed1;
    size_t seed2;

    static constexpr size_t MAX_EVICTIONS = 32;

    size_t count;
    float max_load;

    size_t hash1(const K& key) const {
        return ((std::hash<K>{}(key) + seed1) * 2654435761ULL) % bucket1.size();
    }

    size_t hash2(const K& key) const {
        return ((std::hash<K>{}(key) + seed2) * 2246822519ULL) % bucket2.size();
    }

public:
    HashMap(size_t cap = 16, float max_load = 0.5f): max_load(max_load), count(0), seed1(1), seed2(2) {
        bucket1.resize(cap, Node());
        bucket2.resize(cap, Node());
    }

    size_t size() const {
        return count;
    }

    void insert(const K& key, const V& value) {
        if((float) count/(bucket1.size() + bucket2.size()) > max_load) {
            resize();
        }

        size_t eviction_count = 0;
        size_t index1 = hash1(key);
        size_t index2 = hash2(key);

        //overwrite existing key
        if(bucket1[index1].state == State::HAS_VALUE && bucket1[index1].key == key) {
            bucket1[index1].value = value;
            return;
        }

        if(bucket2[index2].state == State::HAS_VALUE && bucket2[index2].key == key) {
            bucket2[index2].value = value;
            return;
        }

        //check key in 2 buckets, evict
        K current_key = key;
        V current_value = value;
        while(true) {
            eviction_count = 0;
            while(eviction_count < MAX_EVICTIONS) {
                auto& bucket = (eviction_count % 2 == 0) ? bucket1 : bucket2;
                size_t index = (eviction_count % 2 == 0) ? hash1(current_key) : hash2(current_key);

                if(bucket[index].state == State::EMPTY) {
                    bucket[index] = Node(current_key, current_value);
                    count++;
                    return;
                }

                Node tmp = Node(current_key, current_value);
                current_key = bucket[index].key;
                current_value = bucket[index].value;
                bucket[index] = tmp;

                eviction_count++;
            }
            rehash();
        }
    }

    V* get(const K& key) {
        size_t index1 = hash1(key);
        size_t index2 = hash2(key);

        if(bucket1[index1].key == key && bucket1[index1].state == State::HAS_VALUE) {
            return &bucket1[index1].value;
        }

        if(bucket2[index2].key == key && bucket2[index2].state == State::HAS_VALUE) {
            return &bucket2[index2].value;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t index1 = hash1(key);
        size_t index2 = hash2(key);

        if(bucket1[index1].key == key && bucket1[index1].state == State::HAS_VALUE) {
            bucket1[index1] = Node();
            count--;
            return true;
        }

        if(bucket2[index2].key == key && bucket2[index2].state == State::HAS_VALUE) {
            bucket2[index2] = Node();
            count--;
            return true;
        }

        return false;
    }

private:
    void resize() {
        std::vector<Node> old_bucket1 = bucket1;
        std::vector<Node> old_bucket2 = bucket2;

        bucket1.assign(old_bucket1.size() * 2, Node());
        bucket2.assign(old_bucket2.size() * 2, Node());
        count = 0;

        for(auto& node: old_bucket1) {
            if(node.state == State::HAS_VALUE) {
                insert(node.key, node.value);
            }
        }

        for(auto& node: old_bucket2) {
            if(node.state == State::HAS_VALUE) {
                insert(node.key, node.value);
            }
        }
    }

    // raw eviction loop into provided buckets — no rehash fallback, returns false on cycle
    bool raw_insert(std::vector<Node>& b1, std::vector<Node>& b2,
                    K key, V value, size_t s1, size_t s2) {
        size_t cap = b1.size();
        K current_key = key;
        V current_value = value;

        for(size_t i = 0; i < MAX_EVICTIONS; i++) {
            auto& b = (i % 2 == 0) ? b1 : b2;
            size_t index = (i % 2 == 0)
                ? ((std::hash<K>{}(current_key) + s1) * 2654435761ULL) % cap
                : ((std::hash<K>{}(current_key) + s2) * 2246822519ULL) % cap;

            if(b[index].state == State::EMPTY) {
                b[index] = Node(current_key, current_value);
                return true;
            }

            Node tmp = Node(current_key, current_value);
            current_key = b[index].key;
            current_value = b[index].value;
            b[index] = tmp;
        }
        return false;
    }

    void rehash() {
        size_t cap = bucket1.size();

        // collect all current keys
        std::vector<Node> all_nodes;
        for(auto& n : bucket1) if(n.state == State::HAS_VALUE) all_nodes.push_back(n);
        for(auto& n : bucket2) if(n.state == State::HAS_VALUE) all_nodes.push_back(n);

        // try new seeds until all keys fit without cycles
        while(true) {
            seed1 += 2;
            seed2 += 2;

            std::vector<Node> new_b1(cap, Node());
            std::vector<Node> new_b2(cap, Node());
            bool success = true;

            for(auto& node : all_nodes) {
                if(!raw_insert(new_b1, new_b2, node.key, node.value, seed1, seed2)) {
                    success = false;
                    break;
                }
            }

            if(success) {
                bucket1 = std::move(new_b1);
                bucket2 = std::move(new_b2);
                return;
            }
        }
    }
};