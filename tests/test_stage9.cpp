#include "../stage9_consistent_hashing/hash_ring.h"
#include <iostream>
#include <string>
#include <unordered_map>

void expect(bool condition, const std::string& test_name) {
    std::cout << (condition ? "PASS: " : "FAIL: ") << test_name << std::endl;
}

void add_and_get_node() {
    HashRing ring;
    ring.add_node("server-A");
    std::string node = ring.get_node("user:1234");
    expect(node == "server-A", "add_and_get_node");
}

void get_returns_one_of_the_servers() {
    HashRing ring;
    ring.add_node("server-A");
    ring.add_node("server-B");
    ring.add_node("server-C");

    std::string node = ring.get_node("user:1234");
    bool valid = node == "server-A" || node == "server-B" || node == "server-C";
    expect(valid, "get_returns_one_of_the_servers");
}

void same_key_always_routes_to_same_server() {
    HashRing ring;
    ring.add_node("server-A");
    ring.add_node("server-B");
    ring.add_node("server-C");

    std::string first = ring.get_node("user:9999");
    std::string second = ring.get_node("user:9999");
    expect(first == second, "same_key_always_routes_to_same_server");
}

void size_tracks_physical_servers() {
    HashRing ring(100);
    ring.add_node("server-A");
    ring.add_node("server-B");
    expect(ring.size() == 2, "size_tracks_physical_servers");
}

void remove_node_routes_keys_elsewhere() {
    HashRing ring;
    ring.add_node("server-A");
    ring.add_node("server-B");

    std::string before = ring.get_node("user:42");
    if(before == "server-A") {
        ring.remove_node("server-A");
        std::string after = ring.get_node("user:42");
        expect(after == "server-B", "remove_node_routes_keys_elsewhere");
    } else {
        ring.remove_node("server-B");
        std::string after = ring.get_node("user:42");
        expect(after == "server-A", "remove_node_routes_keys_elsewhere");
    }
}

void remove_node_updates_size() {
    HashRing ring;
    ring.add_node("server-A");
    ring.add_node("server-B");
    ring.remove_node("server-A");
    expect(ring.size() == 1, "remove_node_updates_size");
}

void add_duplicate_node_throws() {
    HashRing ring;
    ring.add_node("server-A");
    bool threw = false;
    try {
        ring.add_node("server-A");
    } catch(const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "add_duplicate_node_throws");
}

void remove_nonexistent_node_throws() {
    HashRing ring;
    bool threw = false;
    try {
        ring.remove_node("server-A");
    } catch(const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "remove_nonexistent_node_throws");
}

void get_on_empty_ring_throws() {
    HashRing ring;
    bool threw = false;
    try {
        ring.get_node("user:1234");
    } catch(const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "get_on_empty_ring_throws");
}

// keys distribute across all servers (no server gets 0 keys with 1000 samples)
void keys_distribute_across_servers() {
    HashRing ring(100);
    ring.add_node("server-A");
    ring.add_node("server-B");
    ring.add_node("server-C");

    std::unordered_map<std::string, int> counts;
    for(int i = 0; i < 1000; i++) {
        std::string key = "key:" + std::to_string(i);
        counts[ring.get_node(key)]++;
    }

    expect(counts["server-A"] > 0, "distribution_server_A_gets_keys");
    expect(counts["server-B"] > 0, "distribution_server_B_gets_keys");
    expect(counts["server-C"] > 0, "distribution_server_C_gets_keys");
}

// when a server is removed, keys that were on other servers must not move
void remove_node_does_not_reroute_unaffected_keys() {
    HashRing ring(100);
    ring.add_node("server-A");
    ring.add_node("server-B");
    ring.add_node("server-C");

    // record which server each key goes to
    std::unordered_map<std::string, std::string> before;
    for(int i = 0; i < 300; i++) {
        std::string key = "key:" + std::to_string(i);
        before[key] = ring.get_node(key);
    }

    ring.remove_node("server-C");

    int moved_incorrectly = 0;
    for(int i = 0; i < 300; i++) {
        std::string key = "key:" + std::to_string(i);
        std::string after = ring.get_node(key);
        // keys that were NOT on server-C must stay on the same server
        if(before[key] != "server-C" && after != before[key]) {
            moved_incorrectly++;
        }
    }

    expect(moved_incorrectly == 0, "remove_node_does_not_reroute_unaffected_keys");
}

int main() {
    std::cout << "__________________stage 9 testcase________________________" << std::endl;
    add_and_get_node();
    get_returns_one_of_the_servers();
    same_key_always_routes_to_same_server();
    size_tracks_physical_servers();
    remove_node_routes_keys_elsewhere();
    remove_node_updates_size();
    add_duplicate_node_throws();
    remove_nonexistent_node_throws();
    get_on_empty_ring_throws();
    keys_distribute_across_servers();
    remove_node_does_not_reroute_unaffected_keys();
    return 0;
}
