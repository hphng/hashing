#pragma once
#include <vector>
#include <functional>

template <typename K, typename V>

class HashMap {
    struct Node {
        K key;
        V value;
        Node* next;

        Node(K key, V value): key(key), value(value), next(nullptr) {}
    };

    std::vector<Node*> bucket;
    size_t count;
    float max_load = 0.75f; //count/slot

    size_t bucketIndex(const K& key){
        size_t size = bucket.size();
        int hash_value = std::hash<K>{}(key) % size;

        return hash_value;
    }

public:
    HashMap(size_t cap = 16, float max_load = 0.75f): count(0), max_load(max_load) {
        bucket.resize(cap, nullptr);
    }

    ~HashMap() {
        for (Node* head: bucket){
            Node* cur = head;
            while(cur){
                Node* next = cur -> next;
                delete cur;
                cur = next;
            }
        }
    }

    void insert(const K& key, const V& value) {
        if((float)(count + 1) / bucket.size() > max_load){
            resize();
        }
        size_t index = bucketIndex(key);
        Node* cur = bucket[index];
        
        //slot have no value
        if(!cur){
            Node* newNode = new Node(key, value);
            bucket[index] = newNode;
            count++;
            return;
        }
        
        while(cur) {
            //overwrite the same key
            if(cur -> key == key){
                cur -> value = value;
                return;
            }
            cur = cur -> next;
        }

        Node* newNode = new Node(key, value);
        newNode -> next = bucket[index];
        bucket[index] = newNode;
        count++;
    }

    V* get(const K& key) {
        size_t index = bucketIndex(key);
        Node* cur = bucket[index];

        if(!cur) {
            return nullptr;
        }

        while(cur) {
            if(cur -> key == key){
                return &cur -> value;
            }
            cur = cur -> next;
        }

        return nullptr;
    }

    bool remove(const K& key) {
        size_t index = bucketIndex(key);
        Node* cur = bucket[index];

        if(!cur){
            return false;
        }

        if(cur -> key == key){
            bucket[index] = cur -> next;
            delete cur;
            count--;
            return true;
        }

        Node* prev = cur;
        cur = cur -> next;
        while(cur){
            if(cur -> key == key){
                prev -> next = cur -> next;
                delete cur;
                count--;
                return true;
            }

            prev = cur;
            cur = cur -> next;
        }

        return false;
    }

    size_t size(){
        return count;
    }

private:
    void resize() {
        std::vector<Node*> newBucket(bucket.size() * 2, nullptr);

        for(Node* head: bucket){
            Node* cur = head;
            while(cur){
                Node* next = cur -> next;

                size_t newIndex = std::hash<K>{}(cur -> key) % newBucket.size();
                cur -> next = newBucket[newIndex];
                newBucket[newIndex] = cur;

                cur = next;
            }
        }

        bucket = std::move(newBucket);
    }
};