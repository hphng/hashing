#include "hash_ring.h"

uint32_t HashRing::hash(const std::string& key) const {
    return static_cast<uint32_t>(std::hash<std::string>{}(key));
}

HashRing::HashRing(int virtual_nodes) : virtual_nodes(virtual_nodes) {}

void HashRing::add_node(const std::string& name) {
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

void HashRing::remove_node(const std::string& name) {
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

std::string HashRing::get_node(const std::string& key) const {
    if(ring.empty()) throw std::runtime_error("ring is empty");

    uint32_t position = hash(key);
    auto it = ring.lower_bound(position);

    if(it == ring.end()) {
        it = ring.begin();
    }

    return it->second;
}

int HashRing::size() const {
    return nodes.size();
}
