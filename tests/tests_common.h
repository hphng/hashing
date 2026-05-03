#pragma once
#include <iostream>
#include <string>

void expect(bool condition, const std::string &test_name) {
    std::cout << (condition ? "PASS: " : "FAIL: ") << test_name << std::endl;
}

void test_insert_get() {
    HashMap<std::string, int> mp;
    mp.insert("alice", 1);
    int *value = mp.get("alice");
    expect(value && *value == 1, "insert_get");
}

void update_existing_key() {
    HashMap<std::string, std::string> mp;
    mp.insert("alice", "A");
    mp.insert("alice", "B");
    std::string* value = mp.get("alice");
    expect(value && *value == "B", "update_existing_key");
}

void get_missing_key() {
    HashMap<std::string, std::string> mp;
    mp.insert("alice", "A");
    std::string* value = mp.get("bob");
    expect(value == nullptr, "get_missing_key");
}

void remove_existing_key() {
    HashMap<std::string, std::string> mp;
    mp.insert("alice", "A");
    expect(mp.remove("alice") == true, "remove_existing_key");
}

void remove_missing_key() {
    HashMap<std::string, std::string> mp;
    mp.insert("alice", "A");
    expect(mp.remove("bob") == false, "remove_missing_key");
}

void resize() {
    HashMap<std::string, int> mp;
    for (int i = 0; i < 15; i++) {
        std::string key = "key " + std::to_string(i);
        mp.insert(key, i);
    }
    for (int i = 0; i < 15; i++) {
        std::string key = "key " + std::to_string(i);
        int* value = mp.get(key);
        expect(value && *value == i, "resize_with " + key);
    }
}

void size_tracking() {
    HashMap<std::string, int> mp;
    mp.insert("Alice", 100);
    mp.insert("Bob", 300);
    mp.remove("Alice");
    mp.insert("Candance", 20);
    expect(mp.size() == 2, "size_tracking");
}

void insert_remove_then_get() {
    HashMap<std::string, int> mp;
    mp.insert("Alice", 100);
    mp.remove("Alice");
    int* value = mp.get("Alice");
    expect(value == nullptr, "insert_remove_then_get");
}

void insert_remove_then_insert_same_key() {
    HashMap<std::string, int> mp;
    mp.insert("Alice", 100);
    mp.remove("Alice");
    mp.insert("Alice", 200);
    int* value = mp.get("Alice");
    expect(value && *value == 200, "insert_remove_then_insert_same_key");
}

void run_common_tests() {
    test_insert_get();
    update_existing_key();
    get_missing_key();
    remove_existing_key();
    remove_missing_key();
    resize();
    size_tracking();
    insert_remove_then_get();
    insert_remove_then_insert_same_key();
}
