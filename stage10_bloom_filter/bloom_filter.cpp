#include "bloom_filter.h"
#include <cmath>

BloomFilter::BloomFilter(size_t nums_element, float false_positive_rate) {
    size_t size = ceil(-(nums_element * log(false_positive_rate)) / (log(2) * log(2)));
    num_hash_function = (size / nums_element) * log(2);

    bits.resize(size, false);
}

uint64_t BloomFilter::hash1(const std::string& key) const {
    uint64_t hash = 14695981039346656037ULL; //offset basis
    for(char c: key) {
        hash ^= (uint8_t) c;
        hash *= 1099511628211ULL; //FNV prime
    }

    return hash;
}

uint64_t BloomFilter::hash2(const std::string& key) const {
    uint64_t h1 = hash1(key);
    return h1 >> 33 ^ h1;
}

/*  
    Kirsch-Mitzenmacher trick for h2 hashing
*/
void BloomFilter::insert(const std::string& key) {
    uint64_t h1 = hash1(key);
    uint64_t h2 = hash2(key);

    for(size_t i = 0; i < num_hash_function; i++){
        size_t index = (h1 + i * h2) % bits.size();
        bits[index] = true;
    }
}

bool BloomFilter::check(const std::string& key) const {
    uint64_t h1 = hash1(key);
    uint64_t h2 = hash2(key);

    for(size_t i = 0; i < num_hash_function; i++) {
        size_t index = (h1 + i * h2) % bits.size();
        if(!bits[index]) {
            return false;
        }
    }

    return true;
}