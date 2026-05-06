#include "../stage10_bloom_filter/bloom_filter.h"
#include "../stage10_bloom_filter/bloom_filter.cpp"
#include <iostream>
#include <string>

void expect(bool condition, const std::string& test_name) {
    std::cout << (condition ? "PASS: " : "FAIL: ") << test_name << std::endl;
}

void inserted_key_always_found() {
    BloomFilter filter(1000, 0.01);
    filter.insert("alice");
    expect(filter.check("alice") == true, "inserted_key_always_found");
}

void missing_key_returns_false() {
    BloomFilter filter(1000, 0.01);
    filter.insert("alice");
    // "bob" was never inserted — should return false (could be a false positive but extremely unlikely with 1% rate and 1 key)
    expect(filter.check("bob") == false, "missing_key_returns_false");
}

// bloom filter guarantee: no false negatives — inserted keys must ALWAYS be found
void no_false_negatives() {
    BloomFilter filter(1000, 0.01);

    for(int i = 0; i < 500; i++) {
        filter.insert("key:" + std::to_string(i));
    }

    bool all_found = true;
    for(int i = 0; i < 500; i++) {
        if(!filter.check("key:" + std::to_string(i))) {
            all_found = false;
            break;
        }
    }

    expect(all_found, "no_false_negatives");
}

// false positive rate should be close to configured rate (within 2x)
void false_positive_rate_within_bounds() {
    int n = 10000;
    float target_fpr = 0.01;
    BloomFilter filter(n, target_fpr);

    for(int i = 0; i < n; i++) {
        filter.insert("inserted:" + std::to_string(i));
    }

    int false_positives = 0;
    int checks = 10000;
    for(int i = 0; i < checks; i++) {
        // these keys were never inserted
        if(filter.check("notinserted:" + std::to_string(i))) {
            false_positives++;
        }
    }

    float actual_fpr = (float)false_positives / checks;
    // allow up to 2x the target rate — hash functions aren't perfect
    expect(actual_fpr < target_fpr * 2, "false_positive_rate_within_bounds (actual: " + std::to_string(actual_fpr) + ")");
}

// inserting the same key twice should not cause issues
void insert_duplicate_is_safe() {
    BloomFilter filter(1000, 0.01);
    filter.insert("alice");
    filter.insert("alice");
    expect(filter.check("alice") == true, "insert_duplicate_is_safe");
}

// tighter false positive rate needs more memory but should still work
void tight_false_positive_rate() {
    int n = 10000;
    float target_fpr = 0.001;  // 0.1%
    BloomFilter filter(n, target_fpr);

    for(int i = 0; i < n; i++) {
        filter.insert("inserted:" + std::to_string(i));
    }

    // no false negatives
    bool all_found = true;
    for(int i = 0; i < n; i++) {
        if(!filter.check("inserted:" + std::to_string(i))) {
            all_found = false;
            break;
        }
    }

    expect(all_found, "tight_fpr_no_false_negatives");

    int false_positives = 0;
    for(int i = 0; i < 10000; i++) {
        if(filter.check("notinserted:" + std::to_string(i))) false_positives++;
    }

    float actual_fpr = (float)false_positives / 10000;
    expect(actual_fpr < target_fpr * 2, "tight_false_positive_rate (actual: " + std::to_string(actual_fpr) + ")");
}

int main() {
    std::cout << "__________________stage 10 testcase________________________" << std::endl;
    inserted_key_always_found();
    missing_key_returns_false();
    no_false_negatives();
    false_positive_rate_within_bounds();
    insert_duplicate_is_safe();
    tight_false_positive_rate();
    return 0;
}
