#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

const int SIZE = 2000;  // Size of the matrices
const int BLOCK_SIZE = 250;  // Adjust the block size as needed

// Function to generate a random integer between 1 and 10
int randomInt() {
    return rand() % 10 + 1;
}

void multiply(const std::vector<std::vector<int>>& matrix1, const std::vector<std::vector<int>>& matrix2, std::vector<std::vector<int>>& result, int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            result[i][j] = 0;
        }
    }

    for (int k1 = 0; k1 < SIZE; k1 += BLOCK_SIZE) {
        for (int k2 = 0; k2 < SIZE; k2 += BLOCK_SIZE) {
            for (int i = startRow; i < endRow; ++i) {
                for (int k = k1; k < k1 + BLOCK_SIZE; ++k) {
                    for (int j = k2; j < k2 + BLOCK_SIZE; ++j) {
                        result[i][j] += matrix1[i][k] * matrix2[k][j];
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

    auto start = std::chrono::high_resolution_clock::now();

    // Set the number of threads (adjust as needed)
    int numThreads = 4;

    // Calculate the number of rows each thread will handle
    int rowsPerThread = SIZE / numThreads;

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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Display execution time
    std::cout << "threads+cache multiplication took " << duration << " milliseconds" << std::endl;

    return 0;
}
