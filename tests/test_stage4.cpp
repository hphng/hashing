#include "../stage4_quadratic_probing/hash_map.h"
#include "tests_common.h"
#include<iostream>

void get_after_remove() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 100);

    mp.remove("alice");
    int* value = mp.get("alice");

    expect(value == nullptr, "get_after_remove");
}

void find_key_past_tombstone() {
    HashMap<std::string, int> mp(11, 0.4f);
    mp.insert("alice", 1);
    mp.insert("bob", 2);
    mp.insert("candice", 3);
    mp.remove("candice");
    mp.insert("huy", 4);

    int* alice = mp.get("alice");
    int* bob = mp.get("bob");
    int* huy = mp.get("huy");
    expect(alice && bob && huy && *alice == 1 && *bob == 2 && *huy == 4, "find_key_past_tombstone");
}

void overwrite_key_past_tombstone(){
    HashMap<std::string, int> mp(11, 0.4f);
    mp.insert("alice", 1);
    mp.insert("bob", 2);
    mp.insert("candice", 3);
    mp.remove("candice");
    mp.insert("huy", 4);

    mp.insert("alice", 101);
    mp.insert("bob", 102);
    mp.insert("huy", 104);

    int* alice = mp.get("alice");
    int* bob = mp.get("bob");
    int* huy = mp.get("huy");

    expect(alice && bob && huy && *alice == 101 && *bob == 102 && *huy == 104, "overwrite_key_past_tombstone");
}

void size_after_remove_and_reinsert() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);
    mp.insert("bob", 2);
    mp.insert("candice", 3);
    mp.remove("candice");
    mp.insert("candice", 2);

    size_t size = mp.size();
    expect(size == 3, "size_after_remove_and_reinsert");
}

int main() {
    std::cout << "__________________common testcase________________________" << std::endl;
    run_common_tests();
    std::cout << "__________________stage 4 only________________________" << std::endl;
    get_after_remove();
    find_key_past_tombstone();
    overwrite_key_past_tombstone();
    size_after_remove_and_reinsert();
    return 0;
}
