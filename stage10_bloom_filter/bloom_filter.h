#pragma once
#include<vector>
#include<functional>
#include<string>
#include<cstdint>

class BloomFilter {
    std::vector<bool> bits;
    size_t num_hash_function;

public:
    BloomFilter(size_t nums_element = 1e6, float false_positive_rate = 0.01);
    void insert(const std::string& key);
    bool check(const std::string& key) const;

private:
    uint64_t hash1(const std::string& key) const;
    uint64_t hash2(const std::string& key) const;
};