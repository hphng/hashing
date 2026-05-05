#include "../stage7_cuckoo_hashing/hash_map.h"
#include "tests_common.h"
#include<iostream>

void get_after_remove() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 100);

    mp.remove("alice");
    int* value = mp.get("alice");

    expect(value == nullptr, "get_after_remove");
}

void overwrite_existing_key() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);
    mp.insert("bob", 2);
    mp.insert("candice", 3);

    mp.insert("alice", 101);
    mp.insert("bob", 102);
    mp.insert("candice", 103);

    int* alice = mp.get("alice");
    int* bob = mp.get("bob");
    int* candice = mp.get("candice");

    expect(alice && bob && candice && *alice == 101 && *bob == 102 && *candice == 103, "overwrite_existing_key");
}

void size_after_remove_and_reinsert() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);
    mp.insert("bob", 2);
    mp.insert("candice", 3);
    mp.remove("candice");
    mp.insert("candice", 2);

    expect(mp.size() == 3, "size_after_remove_and_reinsert");
}

void remove_nonexistent_key() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);

    bool removed = mp.remove("bob");
    expect(!removed, "remove_nonexistent_key");
    expect(mp.size() == 1, "remove_nonexistent_key_size_unchanged");
}

// cuckoo specific: verify keys survive eviction chains and are still findable
void stress_evictions() {
    HashMap<int, int> mp(16, 0.4f);

    for(int i = 0; i < 100; i++) {
        mp.insert(i, i * 3);
    }

    bool all_correct = true;
    for(int i = 0; i < 100; i++) {
        int* v = mp.get(i);
        if(!v || *v != i * 3) {
            all_correct = false;
            break;
        }
    }

    expect(all_correct, "stress_evictions");
    expect(mp.size() == 100, "stress_evictions_size");
}

int main() {
    std::cout << "__________________common testcase________________________" << std::endl;
    run_common_tests();
    std::cout << "__________________stage 7 only________________________" << std::endl;
    get_after_remove();
    overwrite_existing_key();
    size_after_remove_and_reinsert();
    remove_nonexistent_key();
    stress_evictions();
    return 0;
}
