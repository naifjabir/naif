#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>

std::unordered_map<std::string, int> concurrentMap;

void insertConcurrentMap(int start, int end) {
    for (int i = start; i < end; ++i) {
        std::string key = "Key" + std::to_string(i);
        int value = i;

        concurrentMap[key] = value;
    }
}

void searchConcurrentMap(int start, int end, int numSearches, std::vector<double>& latencies) {
    for (int search = 0; search < numSearches; ++search) {
        auto searchStartTime = std::chrono::high_resolution_clock::now();

        for (int i = start; i < end; ++i) {
            std::string key = "Key" + std::to_string(i);

            auto it = concurrentMap.find(key);
            if (it != concurrentMap.end()) {
                // Key found
            }
        }

        auto searchEndTime = std::chrono::high_resolution_clock::now();
        auto searchDuration = std::chrono::duration_cast<std::chrono::microseconds>(searchEndTime - searchStartTime).count();

        latencies.push_back(static_cast<double>(searchDuration));
    }
}

int main() {
    int numItems = 10000;
    int numSearches = 1000;  // Number of searches per "thread"

    insertConcurrentMap(0, numItems);

    for (int numThreads : {1, 2, 4, 8}) {
        std::vector<double> latencies;
        auto startTime = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numThreads; ++i) {
            int start = i * (numItems / numThreads);
            int end = (i + 1) * (numItems / numThreads);
            searchConcurrentMap(start, end, numSearches, latencies);
        }

        auto endTime = std::chrono::high_resolution_clock::now();

        // Calculate and print latency statistics
        double totalLatency = 0.0;
        for (auto latency : latencies) {
            totalLatency += latency;
        }

        //print outputs
        double averageLatency = totalLatency / (numThreads * numSearches);
        std::cout << "Average Latency for " << numThreads << " pseudo-threads: " << averageLatency << " microseconds" << std::endl;

        // Calculate and print throughput
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        double throughput = static_cast<double>(numThreads * numSearches) / duration * 1e6;  // Operations per second
        std::cout << "Throughput for " << numThreads << " pseudo-threads: " << throughput << " operations/second" << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
