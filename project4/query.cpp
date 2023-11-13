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

std::vector<int> search_key_indices(const std::unordered_map<std::string, int>& hash_table, const std::vector<int>& indices, const std::string& search_key) {
    std::vector<int> result_indices;

    // Search for the key in the hash table
    auto it = hash_table.find(search_key);
    if (it != hash_table.end()) {
        int search_value = it->second;

        // Loop through the indices vector to find matching indices
        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] == search_value) {
                result_indices.push_back(i);
            }
        }
    }

    return result_indices;
}

std::vector<int> search_key_indices_and_prefixes(const std::unordered_map<std::string, int>& hash_table, const std::vector<int>& indices, const std::string& search_key) {
    std::vector<int> result_indices;

    // Get all values from the dictionary that have the search key as the complete key or as a prefix
    for (const auto& entry : hash_table) {
        const std::string& key = entry.first;

        if (key == search_key || key.find(search_key) == 0) {
            int search_value = entry.second;

            // Loop through the indices vector to find matching indices
            for (size_t i = 0; i < indices.size(); ++i) {
                if (indices[i] == search_value) {
                    result_indices.push_back(i);
                }
            }
        }
    }

    return result_indices;
}

int main() {
    const std::string encoded_file = "output_string.txt";
    const std::string search_key = "banana";

    std::vector<std::string> words;
    const std::string column = "column.txt";
    std::ifstream file(column);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << column << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read the the file
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            break; // Reached the end of the file
        }

        std::istringstream iss(line);
        std::string word;

        iss >> word;
        words.push_back(word);
    }

    auto result = read_encoded_file(encoded_file);
    const auto& hash_table = result.first;
    const auto& indices = result.second;

    // Measure time for vanilla search through original
    auto startvanilla = std::chrono::high_resolution_clock::now();
    std::vector<int> vanilla_indices;
    for (unsigned int i = 0; i<words.size(); i++){
        if ((words[i]).compare(search_key) == 0)
            vanilla_indices.push_back(i);
    }
    auto endvanilla = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> vanillaDuration = endvanilla - startvanilla;
    
    // Measure time for vanilla prefix search through original
    auto startprefixvanilla = std::chrono::high_resolution_clock::now();
    std::vector<int> vanilla_prefix_indices;
    for (unsigned int i = 0; i<words.size(); i++){
        if (words[i].compare(0, search_key.length(), search_key) == 0)
            vanilla_prefix_indices.push_back(i);
    }
    auto endprefixvanilla = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> vanillaprefixDuration = endprefixvanilla - startprefixvanilla;

    // Measure time for search_key_indices
    auto startIndices = std::chrono::high_resolution_clock::now();
    std::vector<int> result_indices = search_key_indices(hash_table, indices, search_key);
    auto endIndices = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> indicesDuration = endIndices - startIndices;

    // Measure time for search_key_indices_and_prefixes
    auto startPrefixes = std::chrono::high_resolution_clock::now();
    std::vector<int> result_prefixes = search_key_indices_and_prefixes(hash_table, indices, search_key);
    auto endPrefixes = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> prefixesDuration = endPrefixes - startPrefixes;

    // Print the results
    std::cout << "Search Key: " << search_key << "\n";
    std::cout << "Vanilla Indices (Key): ";
    for (int index : vanilla_indices) {
        std::cout << index << " ";
    }
    std::cout << "\n";
    std::cout << "Vanilla Indices (Key and Prefixes): ";
    for (int index : vanilla_prefix_indices) {
        std::cout << index << " ";
    }
    std::cout << "\n";
    std::cout << "Dic Indices (Key): ";
    for (int index : result_indices) {
        std::cout << index << " ";
    }
    std::cout << "\n";
    std::cout << "Dic Indices (Key and Prefixes): ";
    for (int index : result_prefixes) {
        std::cout << index << " ";
    }
    std::cout << "\n";

    // Print the timings
    std::cout << "Vanilla Search_key_indices Time: " << vanillaDuration.count() << " seconds\n";
    std::cout << "Vanilla Search_key_indices Time: " << vanillaprefixDuration.count() << " seconds\n";
    std::cout << "Dic Search_key_indices Time: " << indicesDuration.count() << " seconds\n";
    std::cout << "Dic Search_key_indices_and_prefixes Time: " << prefixesDuration.count() << " seconds\n";
    
    return 0;
}
