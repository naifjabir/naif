#include <iostream>
#include <vector>
#include <immintrin.h>
#include <chrono>

const int N = 5000;  
const int BLOCK_SIZE = 250;

void matrixMultiply(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& result) {
    #pragma omp parallel for
    for (int jj = 0; jj < N; jj=jj+BLOCK_SIZE)
        for (int kk = 0; kk < N; kk=kk+BLOCK_SIZE)
            for (int i = 0; i < N; i++)
                for (int j=jj; j < std::min((jj+BLOCK_SIZE),N); j++){
                    __m256 sum = _mm256_setzero_ps();
                    for (int k = kk; k < std::min((kk+BLOCK_SIZE),N); k++){
                        __m256 a = _mm256_loadu_ps(&A[i * N + k]);
                        __m256 b = _mm256_loadu_ps(&B[k * N + j]);
                        sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
                    }
                    _mm256_storeu_ps(&result[i * N + j], sum);
                }
}

int main() {
    std::srand(std::time(nullptr));
    std::vector<float> matrixA(N * N, 1.0f);
    std::vector<float> matrixB(N * N, 2.0f);
    std::vector<float> result(N * N, 0.0f);

    auto start = std::chrono::high_resolution_clock::now();
    matrixMultiply(matrixA, matrixB, result);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "simd+cache multiplication took " << duration.count() << " milliseconds.\n";

    return 0;
}
