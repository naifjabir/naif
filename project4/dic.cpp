#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <chrono>

std::vector<std::string> read_raw_column(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::string> raw_data;
    std::string value;

    while (file >> value) {
        if (file.fail()) {
            // Extraction failed, possibly due to mismatched type
            file.clear(); // Clear the error flag
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line
            continue;
        }
        raw_data.push_back(value);
    }

    if (file.bad()) {
        std::cerr << "Error reading data from the file: " << file_path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Read " << raw_data.size() << " values from the file.\n";

    return raw_data;
}

template <typename T>
std::pair<std::vector<int>, std::unordered_map<T, int>> dictionary_encode(const std::vector<T>& raw_data) {
    std::unordered_map<T, int> dictionary;
    std::vector<int> encoded_data;

    int index = 0;  // Start index from 0

    for (const auto& value : raw_data) {
        auto it = dictionary.find(value);
        if (it == dictionary.end()) {
            dictionary[value] = index++;
        }
        encoded_data.push_back(dictionary[value]);
    }

    return {encoded_data, dictionary};
}

std::pair<std::vector<int>, std::unordered_map<std::string, int>> parallel_dictionary_encode(
    const std::vector<std::string>& raw_data, int num_threads) {

    std::vector<std::future<std::pair<std::vector<int>, std::unordered_map<std::string, int>>>> futures;

    size_t chunk_size = raw_data.size() / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        auto start = raw_data.begin() + i * chunk_size;
        auto end = (i == num_threads - 1) ? raw_data.end() : start + chunk_size;

        futures.emplace_back(std::async(std::launch::async, dictionary_encode<std::string>, std::vector<std::string>(start, end)));
    }

    std::vector<int> encoded_data;
    std::unordered_map<std::string, int> merged_dict;
    int count = 0;

    for (auto& future : futures) {
        auto result = future.get();
        encoded_data.insert(encoded_data.end(), result.first.begin(), result.first.end());

        for (const auto& entry : result.second) {
            auto it = merged_dict.find(entry.first);
            if (it == merged_dict.end()) {
                merged_dict[entry.first] = count++;
            }
        }
    }

    return {encoded_data, merged_dict};
}

void write_output(const std::unordered_map<std::string, int>& dictionary, const std::vector<int>& encoded_data) {
    std::ofstream file("output_string.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening output file: output_string.txt" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Write the dictionary
    for (const auto& entry : dictionary) {
        file << entry.first << ": " << entry.second << "\n";
    }

    // Putting a break to help separate the definitions and the list of indices
    file << "\n";

    // Write the compressed encoded data
    for (int value : encoded_data) {
        file << value << "\n";
    }

    std::cout << "Data written to output_string.txt" << std::endl;
}

int main() {
    const std::string input_file = "column.txt";

    // Using std::string as the data type
    std::vector<std::string> raw_data_string = read_raw_column(input_file);

    int num_threads = 16;
    auto result_string = parallel_dictionary_encode(raw_data_string, num_threads);
    
    std::vector<int> threads = {1, 4, 8, 16, 32, 64, 128, 512, 1024};
    for (unsigned int i = 0; i<threads.size(); i++){
        auto start = std::chrono::high_resolution_clock::now();
        num_threads = threads[i];
        result_string = parallel_dictionary_encode(raw_data_string, num_threads);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> Duration = end - start;

        std::cout << "Dictionary Creation Time (numbers of threads = ";
        std::cout << num_threads << "): " << Duration.count() << " seconds\n";
    }

    std::unordered_map<std::string, int> dic = result_string.second;
    std::vector<int> indices = result_string.first;

    // Apply integer compression to the encoded data
    std::vector<int> compressed_data(indices.size());
    std::adjacent_difference(indices.begin(), indices.end(), compressed_data.begin());

    write_output(dic, compressed_data);

    return 0;
}
