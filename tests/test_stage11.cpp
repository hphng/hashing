#include "../stage11_count_min_sketch/count_min_sketch.h"
#include "../stage11_count_min_sketch/count_min_sketch.cpp"
#include <iostream>
#include <string>

void expect(bool condition, const std::string& test_name) {
    std::cout << (condition ? "PASS: " : "FAIL: ") << test_name << std::endl;
}

// inserted key must return at least its true count (no undercount)
void inserted_key_no_undercount() {
    CountMinSketch sketch;
    sketch.insert("alice");
    expect(sketch.estimate("alice") >= 1, "inserted_key_no_undercount");
}

// key never inserted should return 0
void missing_key_returns_zero() {
    CountMinSketch sketch;
    expect(sketch.estimate("ghost") == 0, "missing_key_returns_zero");
}

// inserting the same key N times should return at least N
void repeated_inserts_accumulate() {
    CountMinSketch sketch;
    for (int i = 0; i < 10; i++) sketch.insert("alice");
    expect(sketch.estimate("alice") >= 10, "repeated_inserts_accumulate");
}

// inserting many distinct keys should not undercount any of them
void many_keys_no_undercount() {
    CountMinSketch sketch;
    int n = 500;
    for (int i = 0; i < n; i++) sketch.insert("key:" + std::to_string(i));

    bool all_ok = true;
    for (int i = 0; i < n; i++) {
        if (sketch.estimate("key:" + std::to_string(i)) < 1) {
            all_ok = false;
            break;
        }
    }
    expect(all_ok, "many_keys_no_undercount");
}

// heavy hitter: key inserted far more than others should have the highest estimate
void heavy_hitter_dominates() {
    CountMinSketch sketch;
    for (int i = 0; i < 100; i++) sketch.insert("rare:" + std::to_string(i));
    for (int i = 0; i < 1000; i++) sketch.insert("popular");

    expect(sketch.estimate("popular") >= sketch.estimate("rare:0"), "heavy_hitter_dominates");
}

// estimates must never go below true count (fundamental CMS guarantee)
void estimate_never_undercounts() {
    CountMinSketch sketch;
    int n = 200;
    for (int i = 0; i < n; i++) sketch.insert("frequent");
    for (int i = 0; i < 50; i++) sketch.insert("noise:" + std::to_string(i));

    expect(sketch.estimate("frequent") >= (size_t)n, "estimate_never_undercounts");
}

// error stays within epsilon * N bound for the configured parameters
void error_within_epsilon_bound() {
    double epsilon = 0.01;
    double delta = 0.01;
    CountMinSketch sketch(epsilon, delta);

    int total = 10000;
    int true_count = 3000;
    for (int i = 0; i < true_count; i++) sketch.insert("target");
    for (int i = 0; i < total - true_count; i++) sketch.insert("noise:" + std::to_string(i));

    size_t est = sketch.estimate("target");
    size_t max_allowed = true_count + (size_t)(epsilon * total);
    expect(est >= (size_t)true_count && est <= max_allowed,
        "error_within_epsilon_bound (estimate: " + std::to_string(est) +
        ", allowed: [" + std::to_string(true_count) + ", " + std::to_string(max_allowed) + "])");
}

int main() {
    std::cout << "__________________stage 11 testcase________________________" << std::endl;
    inserted_key_no_undercount();
    missing_key_returns_zero();
    repeated_inserts_accumulate();
    many_keys_no_undercount();
    heavy_hitter_dominates();
    estimate_never_undercounts();
    error_within_epsilon_bound();
    return 0;
}
