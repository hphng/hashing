#include "../stage2_chaining_dynamic_array/hash_map.h"
#include "tests_common.h"

void collision_handling() {
    HashMap<std::string, std::string> mp(2, 10.0f);
    char c = 'a';
    for (int i = 0; i < 9; i++) {
        std::string key(1, c + i);
        mp.insert(key, key);
    }
    for (int i = 0; i < 9; i++) {
        std::string key(1, c + i);
        std::string* value = mp.get(key);
        expect(value && *value == key, "collision_handling_with " + key);
    }
}

int main() {
    run_common_tests();
    collision_handling();
    return 0;
}
