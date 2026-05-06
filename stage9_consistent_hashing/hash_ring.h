#pragma once
#include <string>
#include <map>
#include <unordered_set>
#include <functional>
#include <stdexcept>
#include <cstdint>

class HashRing {
    std::map<uint32_t, std::string> ring;
    std::unordered_set<std::string> nodes;
    int virtual_nodes;

    uint32_t hash(const std::string& key) const;

public:
    HashRing(int virtual_nodes = 100);

    void add_node(const std::string& name);
    void remove_node(const std::string& name);
    std::string get_node(const std::string& key) const;
    int size() const;
};
