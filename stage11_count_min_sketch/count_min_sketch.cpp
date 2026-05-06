#include "count_min_sketch.h"
#include <cmath>

CountMinSketch::CountMinSketch(double epsilon, double delta) {
    width = std::ceil(std::exp(1.0) / epsilon);
    depth = std::ceil(std::log(1.0 / delta));

    bucket.resize(depth, std::vector<size_t> (width, 0));
}

uint64_t CountMinSketch::hash1(const std::string& key) const {
    uint64_t hash = 14695981039346656037ULL; //offset basis
    for(char c: key) {
        hash ^= (uint8_t) c;
        hash *= 1099511628211ULL; //FNV prime
    }

    return hash;
}

uint64_t CountMinSketch::hash2(uint64_t h1) const {
    return (h1 >> 33) ^ h1;
}

void CountMinSketch::insert(const std::string& key) {
    uint64_t h1 = hash1(key);
    uint64_t h2 = hash2(h1);

    for(size_t d = 0; d < depth; d++) {
        size_t index = (h1 + d * h2) % width;

        bucket[d][index]++;
    }
}

size_t CountMinSketch::estimate(const std::string& key) {
    uint64_t h1 = hash1(key);
    uint64_t h2 = hash2(h1);

    size_t count = SIZE_MAX;

    for(size_t d = 0; d < depth; d++) {
        size_t index = (h1 + d * h2) % width;
        count = std::min(count, bucket[d][index]);
    }

    return count;
}