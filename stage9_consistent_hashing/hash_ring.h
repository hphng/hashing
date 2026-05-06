#pragma once
#include <string>
#include <map>
#include <unordered_set>
#include <functional>
#include <stdexcept>
#include <cstdint>

class HashRing {
    std::map<uint32_t, std::string> ring;       // position → server name (one entry per virtual node)
    std::unordered_set<std::string> nodes;      // physical server names
    int virtual_nodes;                          // number of virtual nodes per server

    uint32_t hash(const std::string& key) const {
        return static_cast<uint32_t>(std::hash<std::string>{}(key));
    }

public:
    HashRing(int virtual_nodes = 100): virtual_nodes(virtual_nodes) {}

    void add_node(const std::string& name) {
        if(nodes.contains(name)) {
            throw std::runtime_error("node already exists: " + name);
        }

        nodes.insert(name);
        for(size_t index = 0; index < virtual_nodes; index++) {
            std::string label = name + "#" + std::to_string(index);
            uint32_t position = hash(label);
            ring[position] = name;
        }
    }

    void remove_node(const std::string& name) {
        if(!nodes.contains(name)) {
            throw std::runtime_error("node did not exists: " + name);
        }

        nodes.erase(name);
        for(size_t index = 0; index < virtual_nodes; index++) {
            std::string label = name + "#" + std::to_string(index);
            uint32_t position = hash(label);
            ring.erase(position);
        }
    }

    std::string get_node(const std::string& key) const {
        if(ring.empty()) throw std::runtime_error("ring is empty");

        uint32_t position = hash(key);
        auto it = ring.lower_bound(position);

        if(it == ring.end()) {
            it = ring.begin();
        }

        return it->second;
    }

    int size() const {
        return nodes.size();
    }
};
