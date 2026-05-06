#pragma once
#include <vector>
#include <functional>
#include <string>
#include <cstdint>

class CountMinSketch {
    std::vector<std::vector<size_t>> bucket;
    size_t width;
    size_t depth;

public:
    CountMinSketch(double epsilon = 0.01, double delta = 0.01);
    void insert(const std::string& key);
    size_t estimate(const std::string& key);

private:
    uint64_t hash1(const std::string& key) const;
    uint64_t hash2(uint64_t h1) const;
};