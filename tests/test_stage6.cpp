#include "../stage6_robinhood_hashing/hash_map.h"
#include "tests_common.h"
#include<iostream>

void get_after_remove() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 100);

    mp.remove("alice");
    int* value = mp.get("alice");

    expect(value == nullptr, "get_after_remove");
}

// verify keys displaced past a removed slot are still findable (backward shift correctness)
void find_key_after_remove() {
    HashMap<std::string, int> mp(16, 0.6f);
    mp.insert("alice", 1);
    mp.insert("bob", 2);
    mp.insert("candice", 3);
    mp.remove("alice");

    int* bob = mp.get("bob");
    int* candice = mp.get("candice");
    expect(bob && candice && *bob == 2 && *candice == 3, "find_key_after_remove");
}

void overwrite_existing_key() {
    HashMap<std::string, int> mp(16, 0.6f);
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

int main() {
    std::cout << "__________________common testcase________________________" << std::endl;
    run_common_tests();
    std::cout << "__________________stage 6 only________________________" << std::endl;
    get_after_remove();
    find_key_after_remove();
    overwrite_existing_key();
    size_after_remove_and_reinsert();
    remove_nonexistent_key();
    return 0;
}
