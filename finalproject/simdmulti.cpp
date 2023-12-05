#include <iostream>
#include <unordered_map>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <immintrin.h> // Include for AVX2

std::unordered_map<std::string, int> concurrentMap;
std::mutex mapMutex;

//function to initialize and guard the concurrentmap
void insertConcurrentMap(int start, int end) {
    for (int i = start; i < end; ++i) {
        std::string key = "Key" + std::to_string(i);
        int value = i;

        std::lock_guard<std::mutex> lock(mapMutex);
        concurrentMap[key] = value;
    }
}

// Function to perform multiple search operations and measure latency
void searchConcurrentMap(int start, int end, int numSearches, std::vector<double>& latencies) {
    for (int search = 0; search < numSearches; ++search) {
        auto searchStartTime = std::chrono::high_resolution_clock::now();

        // Use SIMD with AVX2 for search operation
        for (int i = start; i < end; i += 8) {
            std::string keys[8];
            __m256i results[8];

            for (int j = 0; j < 8; ++j) {
                keys[j] = "Key" + std::to_string(i + j);

                std::lock_guard<std::mutex> lock(mapMutex);
                auto it = concurrentMap.find(keys[j]);
                results[j] = _mm256_set1_epi32(it != concurrentMap.end());
            }

        }

        auto searchEndTime = std::chrono::high_resolution_clock::now();
        auto searchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEndTime - searchStartTime).count();

        // Record latency in microseconds
        latencies.push_back(static_cast<double>(searchDuration));
    }
}

int main() {
    int numItems = 10000;
    int numSearches = 1000;  // Number of searches per thread

    // Insert key-value pairs into the concurrent map
    insertConcurrentMap(0, numItems);

    // Measure latency and throughput for different numbers of threads
    for (int numThreads : {1, 2, 4, 8}) {
        std::vector<std::thread> threads;
        std::vector<double> latencies;

        auto startTime = std::chrono::high_resolution_clock::now();

        // Create threads for search operations
        for (int i = 0; i < numThreads; ++i) {
            int start = i * (numItems / numThreads);
            int end = (i + 1) * (numItems / numThreads);
            threads.emplace_back(searchConcurrentMap, start, end, numSearches, std::ref(latencies));
        }

        // Join the threads
        for (auto &thread : threads) {
            thread.join();
        }

        auto endTime = std::chrono::high_resolution_clock::now();

        // Calculate and print latency statistics
        double totalLatency = 0.0;
        for (auto latency : latencies) {
            totalLatency += latency;
        }

        //print outputs
        double averageLatency = totalLatency / (numThreads * numSearches);
        std::cout << "Average Latency for " << numThreads << " threads with SIMD: " << averageLatency << " microseconds" << std::endl;

        // Calculate and print throughput
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        double throughput = static_cast<double>(numThreads * numSearches) / duration * 1e6;  // Operations per second
        std::cout << "Throughput for " << numThreads << " threads with SIMD: " << throughput << " operations/second" << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
