#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

const int MATRIX_SIZE = 5000; 
const int VECTOR_SIZE = MATRIX_SIZE;

void matrixMatrixMultiply(const std::vector<std::vector<float>>& matrixA, const std::vector<std::vector<float>>& matrixB,
                           std::vector<std::vector<float>>& result, int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            result[i][j] = 0.0;
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                result[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}

void initializeMatrix(std::vector<std::vector<float>>& matrix) {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            matrix[i][j] = static_cast<float>(rand()) / RAND_MAX; // Scale to [0, 1)
        }
    }
}

int main() {
    // Initialize matrices with random values
    std::vector<std::vector<float>> matrixA(MATRIX_SIZE, std::vector<float>(MATRIX_SIZE));
    std::vector<std::vector<float>> matrixB(MATRIX_SIZE, std::vector<float>(MATRIX_SIZE));
    std::vector<std::vector<float>> result(MATRIX_SIZE, std::vector<float>(MATRIX_SIZE, 0.0));

    initializeMatrix(matrixA);
    initializeMatrix(matrixB);

    // Measure execution time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Parallel matrix-matrix multiplication
    const int NUM_THREADS = 4;
    int chunk_size = MATRIX_SIZE / NUM_THREADS;
    std::vector<std::thread> threads;

    for (int t = 0; t < NUM_THREADS; ++t) {
        int startRow = t * chunk_size;
        int endRow = (t == NUM_THREADS - 1) ? MATRIX_SIZE : (t + 1) * chunk_size;
        threads.emplace_back(matrixMatrixMultiply, std::ref(matrixA), std::ref(matrixB), std::ref(result), startRow, endRow);
    }

    // Join threads
    for (auto &thread : threads) {
        thread.join();
    }

    // Measure execution time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // Display execution time
    std::cout << MATRIX_SIZE <<" Matrix multiplication took " << duration << " milliseconds" << std::endl;

    return 0;
}
