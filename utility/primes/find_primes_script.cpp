/* 
    run this script with the command below so that it can create primes.h inside the local folder:

    g++ -g utility/primes//find_primes_script.cpp -o find_primes_script.exe
    ./find_primes_script.exe utility/primes/primes.h
*/

#include<iostream>
#include<vector>
#include<fstream>
#include<math.h>

//using Sieve of Eratosthenes for optimize finding prime number from 1 to 100k
std::vector<int> sieve_of_eratosthenes(size_t max_size = std::round(1e6)) {
    std::vector<int> ans;
    std::vector<bool> nums(max_size+1, true);
    nums[0] = false;
    nums[1] = false;

    for(int i = 2; i <= max_size; i++){
        if(nums[i]){
            for(int j = i*i; j <= max_size; j += i){
                nums[j] = false;
            }
        }
    }

    for(int i = 2; i <= max_size; i++){
        if(nums[i]){
            ans.push_back(i);
        }
    }

    return ans;
} 

int main(int argc, char* argv[]) {
    std::vector<int> primes = sieve_of_eratosthenes();

    std::string path = argc > 1 ? argv[1] : "utility/primes";
    std::ofstream file(path);
    file << "#pragma once\n";
    file << "#include <cstddef>\n"; 
    file << "constexpr size_t PRIMES[] = { ";
    for(int p: primes){
        file << p << ", ";
    }
    file << "};\n";
    file << "constexpr size_t PRIMES_COUNT = sizeof(PRIMES) / sizeof(PRIMES[0]);\n";

    std::cout <<"finished write!";
    return 0;
}


