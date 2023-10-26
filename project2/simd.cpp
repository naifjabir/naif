// SIMD.CPP
#include "simd.h"

template <typename T>
SIMD<T>::SIMD(int size) : N(size), matrixA(N, std::vector<T>(N)), matrixB(N, std::vector<T>(N)), result(N, std::vector<T>(N)) {}

template <typename T>
void SIMD<T>::initializeMatrices() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix1[i][j] = static_cast<T>(std::rand()) / RAND_MAX;
            matrix2[i][j] = static_cast<T>(std::rand()) / RAND_MAX;
        }
    }
}

template <typename T>
void SIMD::multiplyMatrices() {
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; j += 8) {
            __m256 sum = _mm256_setzero_ps();
            for (int k = 0; k < N; ++k) {
                __m256 a = _mm256_loadu_ps(&matrix1[i][k]);
                __m256 b = _mm256_loadu_ps(&matrix2[k][j]);
                sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
            }
            _mm256_storeu_ps(&result[i][j], sum);
        }
    }
}

