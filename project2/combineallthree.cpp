#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <immintrin.h>
#include <chrono>

const int SIZE = 10000;  // Size of the matrices
const int BLOCK_SIZE = 250;  // Adjust the block size as needed
const int VECTOR_SIZE = 10; // Number of elements in AVX vector

// Function to generate a random integer between 1 and 10
int randomInt() {
    return rand() % 10 + 1;
}

// Perform vectorized multiplication using AVX
__m256i vectorizedMultiply(__m256i a, __m256i b) {
    __m256i result = _mm256_mullo_epi32(a, b);
    return result;
}

void multiply(const std::vector<std::vector<int>>& matrix1, const std::vector<std::vector<int>>& matrix2, 
    std::vector<std::vector<int>>& result, int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            result[i][j] = 0;
        }
    }

    for (int k1 = 0; k1 < SIZE; k1 += BLOCK_SIZE) {
        for (int k2 = 0; k2 < SIZE; k2 += BLOCK_SIZE) {
            for (int i = startRow; i < endRow; ++i) {
                for (int k = k1; k < k1 + BLOCK_SIZE; k += VECTOR_SIZE) {
                    for (int j = k2; j < k2 + BLOCK_SIZE; j += VECTOR_SIZE) {
                        // Load elements into AVX registers
                        __m256i vecResult = _mm256_setzero_si256();
                        for (int l = 0; l < VECTOR_SIZE; ++l) {
                            __m256i vec1 = _mm256_set1_epi32(matrix1[i][k + l]);
                            __m256i vec2 = _mm256_loadu_si256((__m256i*)&matrix2[k + l][j]);
                            vecResult = _mm256_add_epi32(vecResult, vectorizedMultiply(vec1, vec2));
                        }

                        // Store the result back to memory
                        _mm256_storeu_si256((__m256i*)&result[i][j], vecResult);
                    }
                }
            }
        }
    }
}

int main() {
    // Seed for random number generation
    std::srand(std::time(0));

    // Initialize matrices with random values
    std::vector<std::vector<int>> matrix1(SIZE, std::vector<int>(SIZE));
    std::vector<std::vector<int>> matrix2(SIZE, std::vector<int>(SIZE));
    std::vector<std::vector<int>> result(SIZE, std::vector<int>(SIZE, 0));

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            matrix1[i][j] = randomInt();
            matrix2[i][j] = randomInt();
        }
    }


    // Set the number of threads (adjust as needed)
    int numThreads = 4;

    // Calculate the number of rows each thread will handle
    int rowsPerThread = SIZE / numThreads;

    auto start = std::chrono::high_resolution_clock::now();
    // Create threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? SIZE : (i + 1) * rowsPerThread;
        threads.emplace_back(multiply, std::ref(matrix1), std::ref(matrix2), std::ref(result), startRow, endRow);
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "all 3 combo multiplication took " << duration.count() << " milliseconds.\n";

    return 0;
}
