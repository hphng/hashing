#include "stage7_cuckoo_hashing/hash_map.h"
#include <iostream>
#include <string>

int main() {
    HashMap<std::string, int> mp;
    for(int i = 0; i < 15; i++) {
        std::string key = "key " + std::to_string(i);
        std::cout << "inserting " << key << std::endl << std::flush;
        mp.insert(key, i);
        std::cout << "done " << key << std::endl << std::flush;
    }
    return 0;
}
