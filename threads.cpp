// Threads.cpp

#include "Threads.h"

// Explicit instantiation for int (or other types if needed)
template class Threads<int>;
template class Threads<float>;  // Example for float, add more as needed
// ...

template <typename T>
Threads<T>::Threads(const std::vector<std::vector<T>>& m1,
                                      const std::vector<std::vector<T>>& m2,
                                      std::vector<std::vector<T>>& res,
                                      int start, int end)
    : matrix1(m1), matrix2(m2), result(res), startRow(start), endRow(end), BLOCK_SIZE(250) {}

template <typename T>
void Threads<T>::multiply() {
    // Implementation of multiply function
    for (int i = 0; i < matrix1.size(); i ++) {
        for (int j = 0; j < matrix1.size(); j ++) {
            for (int k = startRow; k < endRow; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

template <typename T>
void Threads<T>::multiply_with_cache() {
    // Implementation of multiply function with cache optimization
    for (int k1 = 0; k1 < matrix1.size(); k1 += BLOCK_SIZE) {
        for (int k2 = 0; k2 < matrix1.size(); k2 += BLOCK_SIZE) {
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

template <typename T>
void Threads<T>::multiply_with_SIMD() {
    // Implementation of multiply function with cache optimization
    for (int k1 = 0; k1 < matrix1.size(); k1 += BLOCK_SIZE) {
        for (int k2 = 0; k2 < matrix1.size(); k2 += BLOCK_SIZE) {
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