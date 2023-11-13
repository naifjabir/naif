#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <immintrin.h>

std::pair<std::unordered_map<std::string, int>, std::vector<int>> read_encoded_file(const std::string& encoded_file) {
    std::ifstream file(encoded_file);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << encoded_file << std::endl;
        exit(EXIT_FAILURE);
    }

    std::unordered_map<std::string, int> hash_table;
    std::vector<int> indices;

    // Read the dictionary part of the file
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            break; // Reached the end of the dictionary part
        }

        std::istringstream iss(line);
        std::string key;
        int value;

        if (iss >> key >> value) {
            // Remove the ":" from the key
            key.pop_back();
            hash_table[key] = value;
        } else {
            std::cerr << "Error reading dictionary part from file: " << encoded_file << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Read the indices part of the file
    while (file >> line) {
        int index = std::stoi(line);
        indices.push_back(index);
    }

    return {hash_table, indices};
}

std::vector<int> search_key_indices_simd(const std::unordered_map<std::string, int>& hash_table, const std::vector<int>& indices, const std::string& search_key) {
    std::vector<int> result_indices;

    // Search for the key in the hash table using SIMD instructions
    auto it = hash_table.find(search_key);
    if (it != hash_table.end()) {
        int search_value = it->second;

        // Use SIMD to compare multiple elements simultaneously
        __m128i searchValueVector = _mm_set1_epi32(search_value);

        // Loop through the indices vector in chunks of 4 (assuming 32-bit integers)
        for (size_t i = 0; i < indices.size(); i += 4) {
            // Load 4 indices into a SIMD register
            __m128i indicesVector = _mm_loadu_si128((__m128i*)&indices[i]);

            // Compare the indices with the search value
            __m128i cmpResult = _mm_cmpeq_epi32(indicesVector, searchValueVector);

            // Extract the comparison result as a bitmask
            int mask = _mm_movemask_ps(_mm_castsi128_ps(cmpResult));

            // Check if there is at least one match in the chunk
            if (mask != 0) {
                // Iterate within the chunk to find exact matches
                for (size_t j = 0; j < 4; ++j) {
                    if ((mask & (1 << j)) != 0) {
                        // Found an exact match
                        result_indices.push_back(i + j);
                    }
                }
            }
        }
    }

    return result_indices;
}

int main() {
    const std::string encoded_file = "output_string.txt";
    const std::string search_key = "banana";

    auto result = read_encoded_file(encoded_file);
    const auto& hash_table = result.first;
    const auto& indices = result.second;

    // Measure time for search_key_indices_simd
    auto startIndicesSIMD = std::chrono::high_resolution_clock::now();
    std::vector<int> result_indices_simd = search_key_indices_simd(hash_table, indices, search_key);
    auto endIndicesSIMD = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> indicesSIMDDuration = endIndicesSIMD - startIndicesSIMD;

    std::cout << "\n";
    std::cout << "Indices SIMD (Key): ";
    for (int index : result_indices_simd) {
        std::cout << index << " ";
    }
    std::cout << std::endl;

    // Print the timings
    std::cout << "Search_key_indices_simd Time: " << indicesSIMDDuration.count() << " seconds\n";

    return 0;
}
