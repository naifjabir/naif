// TS.cpp

#include "TS.h"

template <typename T>
TS<T, MatrixSize, Threads>::TS(const std::vector<std::vector<T>>& matrixA, const std::vector<std::vector<T>>& matrixB, 
int N, int numT)
    : matrixA(matrixA), matrixB(matrixB), result(MatrixSize, std::vector<T>(MatrixSize)), SIZE(N), numThreads(numT) {}

template <typename T, int MatrixSize, int Threads>
void TS<T, MatrixSize, Threads>::multiply() {
    std::vector<std::thread> threads;
    int chunk_size = MatrixSize / Threads;

    for (int i = 0; i < Threads; ++i) {
        int startRow = i * chunk_size;
        int endRow = (i == Threads - 1) ? MatrixSize : (i + 1) * chunk_size;

        threads.emplace_back(&TS::multiplyBlock, this, startRow, endRow);
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
}

template <typename T, int MatrixSize, int Threads>
const std::vector<std::vector<T>>& TS<T, MatrixSize, Threads>::getResult() const {
    return result;
}

template <typename T, int MatrixSize, int Threads>
void TS<T, MatrixSize, Threads>::multiplyBlock(int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < MatrixSize; j += 8) {
            __m256 sum = _mm256_setzero_ps();
            for (int k = 0; k < MatrixSize; ++k) {
                __m256 a = _mm256_loadu_ps(&matrixA[i][k]);
                __m256 b = _mm256_loadu_ps(&matrixB[k][j]);
                sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
            }
            _mm256_storeu_ps(&result[i][j], sum);
        }
    }
}
