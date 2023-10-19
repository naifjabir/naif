#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

const int N = 2000;
int BLOCK_SIZE = 250;//from testing, we determine block size 250 to be the best overall

using Matrix = std::vector<std::vector<int>>;

void initializeMatrix(Matrix& matrix) {
    for (int i = 0; i < N; i++) {
        matrix.emplace_back();
        for (int j = 0; j < N; j++) {
            matrix[i].push_back(rand() % 100); // Adjust the range of random values as needed
        }
    }
}

template <typename T>
std::vector<std::vector<T>> matrixMultiply(const std::vector<std::vector<T>>& A, const std::vector<std::vector<T>>& B) {
    std::vector<std::vector<T>> result(N, std::vector<T>(N, 0));

    for (int jj = 0; jj < N; jj=jj+BLOCK_SIZE)
        for (int kk = 0; kk < N; kk=kk+BLOCK_SIZE)
            for (int i = 0; i < N; i++)
                for (int j=jj; j < std::min((jj+BLOCK_SIZE),N); j++){
                    int r = 0;
                    for (int k = kk; k < std::min((kk+BLOCK_SIZE),N); k++)
                        r = r + (A[i][k] * B[k][j]);
                    result[i][j] = result[i][j] + r;
                }

    return result;
}

Matrix basicMultiply(const Matrix& A, const Matrix& B) {
    Matrix result(N, std::vector<int>(N, 0));

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++){
            int r = 0;
            for (int k = 0; k < N; k++)
                r = r + A[i][k] + B[k][j];
            result[i][j] = r;
        }

    return result;
}

int main() {
    std::srand(std::time(nullptr)); // Seed for random number generation

    Matrix A, B, C, D;

    // Initialize matrices A, B, and C with random values
    initializeMatrix(A);
    initializeMatrix(B);

    // Measure the time for basic matrix multiplication
    auto start = std::chrono::high_resolution_clock::now();
    D = basicMultiply(A, B);
    auto end = std::chrono::high_resolution_clock::now();

    // Print the elapsed time
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "--Basic multiplication took " << duration.count() << " milliseconds.\n";


    // Measure the time for matrix multiplication
    start = std::chrono::high_resolution_clock::now();
    C = matrixMultiply(A, B);
    end = std::chrono::high_resolution_clock::now();

    // Print the elapsed time
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "++Matrix multiplication took " << duration.count() << " milliseconds.\n";


    return 0;
}
