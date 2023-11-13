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

std::vector<int> search_key_indices_and_prefixes_simd(const std::unordered_map<std::string, int>& hash_table, const std::vector<int>& indices, const std::string& search_key) {
    std::vector<int> result_indices;

    // Get all values from the dictionary that have the search key as the complete key or as a prefix using SIMD instructions
    __m128i searchValueVector = _mm_set1_epi32(search_key.length());

    // Iterate through the hash table
    for (auto it = hash_table.begin(); it != hash_table.end(); ++it) {
        const std::string& key = it->first;

        // Skip if the key is shorter than the search key
        if (key.length() < search_key.length()) {
            continue;
        }

        std::cout << "Checking key: " << key << ", length: " << key.length() << std::endl;

        // Load the key length into a SIMD register
        __m128i keyLengthVector = _mm_set1_epi32(key.length());

        // Compare the length of the key with the search key length
        __m128i cmpResult = _mm_cmpeq_epi32(keyLengthVector, searchValueVector);

        // Extract the comparison result as a bitmask
        int mask = _mm_movemask_ps(_mm_castsi128_ps(cmpResult));

        std::cout << "Mask: " << mask << std::endl;

        // Check if there is at least one match
        if (mask != 0) {
            // Loop through the indices vector in chunks of 4 (assuming 32-bit integers)
            for (size_t k = 0; k < indices.size(); k += 4) {
                // Load 4 indices into a SIMD register
                __m128i indicesVector = _mm_loadu_si128((__m128i*)&indices[k]);

                std::cout << "Checking indices: " << indicesVector[0] << ", " << indicesVector[1] << ", " << indicesVector[2] << ", " << indicesVector[3] << std::endl;

                // Compare the indices with the search value
                __m128i cmpResultIndices = _mm_cmpeq_epi32(indicesVector, _mm_set1_epi32(it->second));

                // Extract the comparison result as a bitmask
                int maskIndices = _mm_movemask_ps(_mm_castsi128_ps(cmpResultIndices));

                std::cout << "MaskIndices: " << maskIndices << std::endl;

                // Check if there is at least one match in the chunk
                if (maskIndices != 0) {
                    // Iterate within the chunk to find exact matches
                    for (size_t l = 0; l < 4; ++l) {
                        if ((maskIndices & (1 << l)) != 0) {
                            // Found an exact match
                            result_indices.push_back(k + l);
                        }
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

    // Measure time for search_key_indices_and_prefixes_simd
    auto startPrefixesSIMD = std::chrono::high_resolution_clock::now();
    std::vector<int> result_prefixes_simd = search_key_indices_and_prefixes_simd(hash_table, indices, search_key);
    auto endPrefixesSIMD = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> prefixesSIMDDuration = endPrefixesSIMD - startPrefixesSIMD;

    std::cout << "\n";
    std::cout << "Indices SIMD (Key and Prefixes): ";
    for (int index : result_prefixes_simd) {
        std::cout << index << " ";
    }
    std::cout << std::endl;

    // Print the timings
    std::cout << "Search_key_indices_and_prefixes_simd Time: " << prefixesSIMDDuration.count() << " seconds\n";

    return 0;
}
