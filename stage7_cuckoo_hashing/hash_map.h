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
        size_t h = std::hash<K>{}(key) ^ seed1;
        h ^= h >> 17;
        h *= 2654435761ULL;
        h ^= h >> 13;
        return h % bucket1.size();
    }

    size_t hash2(const K& key) const {
        size_t h = std::hash<K>{}(key) ^ seed2;
        h ^= h >> 13;
        h *= 2246822519ULL;
        h ^= h >> 17;
        return h % bucket2.size();
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
        // overwrite check before load check — avoids resize for updates
        size_t index1 = hash1(key);
        if(bucket1[index1].state == State::HAS_VALUE && bucket1[index1].key == key) {
            bucket1[index1].value = value;
            return;
        }

        size_t index2 = hash2(key);
        if(bucket2[index2].state == State::HAS_VALUE && bucket2[index2].key == key) {
            bucket2[index2].value = value;
            return;
        }

        if((float) count / (bucket1.size() + bucket2.size()) > max_load) {
            resize();
        }

        K current_key = key;
        V current_value = value;
        while(true) {
            for(size_t ev = 0; ev < MAX_EVICTIONS; ev++) {
                auto& bucket = (ev % 2 == 0) ? bucket1 : bucket2;
                size_t index = (ev % 2 == 0) ? hash1(current_key) : hash2(current_key);

                if(bucket[index].state == State::EMPTY) {
                    bucket[index] = Node(current_key, current_value);
                    count++;
                    return;
                }

                // swap instead of creating a tmp Node — avoids copies for complex K/V
                std::swap(current_key, bucket[index].key);
                std::swap(current_value, bucket[index].value);
            }
            // eviction cycle detected — current_key is floating (not in any bucket)
            // rehash rebuilds the table with new seeds; outer loop retries the float
            rehash();
        }
    }

    V* get(const K& key) {
        size_t index1 = hash1(key);
        size_t index2 = hash2(key);

        if(bucket1[index1].state == State::HAS_VALUE && bucket1[index1].key == key) {
            return &bucket1[index1].value;
        }

        if(bucket2[index2].state == State::HAS_VALUE && bucket2[index2].key == key) {
            return &bucket2[index2].value;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t index1 = hash1(key);
        size_t index2 = hash2(key);

        if(bucket1[index1].state == State::HAS_VALUE && bucket1[index1].key == key) {
            bucket1[index1] = Node();
            count--;
            return true;
        }

        if(bucket2[index2].state == State::HAS_VALUE && bucket2[index2].key == key) {
            bucket2[index2] = Node();
            count--;
            return true;
        }

        return false;
    }

private:
    void resize() {
        // move instead of copy — avoids allocating a duplicate of the old table
        std::vector<Node> old_bucket1 = std::move(bucket1);
        std::vector<Node> old_bucket2 = std::move(bucket2);

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
            size_t h = std::hash<K>{}(current_key);
            size_t index;
            if(i % 2 == 0) {
                h ^= s1; h ^= h >> 17; h *= 2654435761ULL; h ^= h >> 13;
                index = h % cap;
            } else {
                h ^= s2; h ^= h >> 13; h *= 2246822519ULL; h ^= h >> 17;
                index = h % cap;
            }

            if(b[index].state == State::EMPTY) {
                b[index] = Node(current_key, current_value);
                return true;
            }

            std::swap(current_key, b[index].key);
            std::swap(current_value, b[index].value);
        }
        return false;
    }

    void rehash() {
        size_t cap = bucket1.size();

        std::vector<Node> all_nodes;
        for(auto& n : bucket1) if(n.state == State::HAS_VALUE) all_nodes.push_back(n);
        for(auto& n : bucket2) if(n.state == State::HAS_VALUE) all_nodes.push_back(n);

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
