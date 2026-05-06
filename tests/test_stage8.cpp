#include "../stage8_hopscotch_hashing/hash_map.h"
#include "tests_common.h"
#include <iostream>

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

void remove_then_reinsert() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);
    mp.remove("alice");
    mp.insert("alice", 999);
    int* value = mp.get("alice");
    expect(value && *value == 999, "remove_then_reinsert");
}

void remove_nonexistent_key() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);
    bool removed = mp.remove("bob");
    expect(!removed, "remove_nonexistent_key");
    expect(mp.size() == 1, "remove_nonexistent_key_size_unchanged");
}

// hopscotch specific: insert enough integer keys to force displacement chains
// and verify every key is still retrievable after displacement
void displacement_all_keys_retrievable() {
    HashMap<int, int> mp(16);

    for(int i = 0; i < 12; i++) {
        mp.insert(i, i * 10);
    }

    bool all_correct = true;
    for(int i = 0; i < 12; i++) {
        int* v = mp.get(i);
        if(!v || *v != i * 10) {
            all_correct = false;
            break;
        }
    }

    expect(all_correct, "displacement_all_keys_retrievable");
}

// hopscotch specific: after removing a displaced key, the slot is freed
// and a new key can occupy it
void remove_displaced_key_then_insert_new() {
    HashMap<int, int> mp(16);

    for(int i = 0; i < 12; i++) {
        mp.insert(i, i * 10);
    }

    mp.remove(5);
    expect(mp.get(5) == nullptr, "removed_displaced_key_not_found");

    mp.insert(5, 999);
    int* v = mp.get(5);
    expect(v && *v == 999, "reinsert_after_remove_displaced_key");
}

// hopscotch specific: stress displacement and resize by inserting many keys
// into a small initial table — forces both displacement chains and resize+retry
void stress_displacement_and_resize() {
    HashMap<int, int> mp(8);

    for(int i = 0; i < 200; i++) {
        mp.insert(i, i * 7);
    }

    bool all_correct = true;
    for(int i = 0; i < 200; i++) {
        int* v = mp.get(i);
        if(!v || *v != i * 7) {
            all_correct = false;
            break;
        }
    }

    expect(all_correct, "stress_displacement_and_resize");
    expect(mp.size() == 200, "stress_displacement_and_resize_size");
}

// hopscotch specific: interleaved inserts and removes keep the map consistent
void interleaved_insert_remove() {
    HashMap<int, int> mp;
    for(int i = 0; i < 50; i++) mp.insert(i, i);
    for(int i = 0; i < 25; i++) mp.remove(i);
    for(int i = 50; i < 75; i++) mp.insert(i, i);

    bool removed_gone = true;
    for(int i = 0; i < 25; i++) {
        if(mp.get(i) != nullptr) { removed_gone = false; break; }
    }

    bool rest_present = true;
    for(int i = 25; i < 75; i++) {
        int* v = mp.get(i);
        if(!v || *v != i) { rest_present = false; break; }
    }

    expect(removed_gone, "interleaved_insert_remove_removed_gone");
    expect(rest_present, "interleaved_insert_remove_rest_present");
    expect(mp.size() == 50, "interleaved_insert_remove_size");
}

int main() {
    std::cout << "__________________common testcase________________________" << std::endl;
    run_common_tests();
    std::cout << "__________________stage 8 only________________________" << std::endl;
    get_after_remove();
    overwrite_existing_key();
    remove_then_reinsert();
    remove_nonexistent_key();
    displacement_all_keys_retrievable();
    remove_displaced_key_then_insert_new();
    stress_displacement_and_resize();
    interleaved_insert_remove();
    return 0;
}
