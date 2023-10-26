//8 different programs (regular multtiplication, threads, SIMD AVX, cache optimization, 
//a unique 1+1 of the 3 optimizations mentioned before, and finally a combination of all 3)

//we will do 4 different data type (2 byte fixed, 4 byte fixed, 4 byte float, 8 byte float)
//we will also do [1,000x1,000], [2,000x2,000], [5,000x5,000] and [10,000x10,000]
//we will test all data types and matrix sizes (16 tests)

//users can choose which of the three optimization combinations they want (7 choices)

#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <immintrin.h>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <memory>
#include <typeinfo>
#include "threads.h"
#include "simd.h"

int N = 1000; // Matrix size of [1,000x1,000], we will change this variable as needed
              //2000, 5000, 10000

const int BLOCK_SIZE = 250;
const int VECTOR_SIZE = 10; // Number of elements in AVX vector

int numThreads = 4; //configurable by user

//matrix sizes to use
int matrix_sizes[4] = {1000,2000,5000,10000};

template <typename T>
void initializeMatrix(std::vector<std::vector<T>>& matrix){
    for (unsigned int i = 0; i<matrix.size(); i++)
        for (unsigned int j = 0; j<matrix.size(); j++)
            matrix[i][j] = (static_cast<T>(rand()) + static_cast<T>(rand()) / RAND_MAX);
}

template <typename T>
void function1_inside_loop(T val){
    std::vector<std::vector<T>> matrix1(N, std::vector<T>(N));
    std::vector<std::vector<T>> matrix2(N, std::vector<T>(N));
    std::vector<std::vector<T>> result(N, std::vector<T>(N, static_cast<T>(0)));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    int userthreadin = 0;
    std::cout << "Enter a number to decide the amount of threads: ";
    std::cin >> userthreadin;
    numThreads = userthreadin;

    // Calculate the number of rows each thread will handle
    int rowsPerThread = N / numThreads;

    auto start = std::chrono::high_resolution_clock::now();
    
    // Create threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
        threads.emplace_back(&Threads<T>::multiply, Threads<T>(matrix1, matrix2, result, startRow, endRow));
    }

    // Join threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        } else {
            std::cerr << "Thread not joinable!" << std::endl;
            // Handle the error as needed
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Threads multiplication for two " << N << "x" << N 
    << " "<< typeid(val).name() << " matrices took " << duration.count() << " milliseconds.\n";
}

void function1(){
    for (int p = 0; p<4; p++){
        N = matrix_sizes[p];
        for (int q = 0; q<4; q++){
            if (q==0){
                int16_t input = 1;
                function1_inside_loop(input);
            } else if (q==1){
                int32_t input = 1;
                function1_inside_loop(input);
            } else if (q==2){
                float input = 1.1;
                function1_inside_loop(input);
            } else if (q==3){
                double input = 1.1;
                function1_inside_loop(input);
            }
        }
    }
}

void multiplyMatricesInt16() {
    std::vector<std::vector<int16_t>> matrix1(N, std::vector<int16_t>(N));
    std::vector<std::vector<int16_t>> matrix2(N, std::vector<int16_t>(N));
    std::vector<std::vector<int16_t>> result(N, std::vector<int16_t>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; j += 8) {
            __m128i sum = _mm_setzero_si128();
            for (int k = 0; k < N; ++k) {
                __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&matrix1[i][k]));
                __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&matrix2[k][j]));
                sum = _mm_add_epi16(sum, _mm_mullo_epi16(a, b));
            }
           _mm_storeu_si128(reinterpret_cast<__m128i*>(&result[i][j]), _mm_add_epi16(_mm_loadu_si128(reinterpret_cast<__m128i*>(&result[i][j])), sum));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " int16_t matrices took " << duration.count() << " milliseconds.\n";
}

void multiplyMatricesInt32() {
    std::vector<std::vector<int32_t>> matrix1(N, std::vector<int32_t>(N));
    std::vector<std::vector<int32_t>> matrix2(N, std::vector<int32_t>(N));
    std::vector<std::vector<int32_t>> result(N, std::vector<int32_t>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; j += 8) {
            __m256i sum = _mm256_setzero_si256();
            for (int k = 0; k < N; ++k) {
                __m256i a = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrix1[i][k]));
                __m256i b = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrix2[k][j]));
                sum = _mm256_add_epi32(sum, _mm256_mullo_epi32(a, b));
            }
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result[i][j]), sum);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " int32_t matrices took " << duration.count() << " milliseconds.\n";
}

void multiplyMatricesFloat() {
    std::vector<std::vector<float>> matrix1(N, std::vector<float>(N));
    std::vector<std::vector<float>> matrix2(N, std::vector<float>(N));
    std::vector<std::vector<float>> result(N, std::vector<float>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();
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
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " float matrices took " << duration.count() << " milliseconds.\n";
}

void multiplyMatricesDouble() {
    std::vector<std::vector<double>> matrix1(N, std::vector<double>(N));
    std::vector<std::vector<double>> matrix2(N, std::vector<double>(N));
    std::vector<std::vector<double>> result(N, std::vector<double>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();
    
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; j += 8) {
            __m256d sum = _mm256_setzero_pd();
            for (int k = 0; k < N; ++k) {
                __m256d a = _mm256_loadu_pd(&matrix1[i][k]);
                __m256d b = _mm256_loadu_pd(&matrix2[k][j]);
                sum = _mm256_add_pd(sum, _mm256_mul_pd(a, b));
            }
            _mm256_storeu_pd(&result[i][j], sum);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " double matrices took " << duration.count() << " milliseconds.\n";
}

void function2(){
    for (int p = 0; p<4; p++){
        N = matrix_sizes[p];
        for (int q = 0; q<4; q++){
            if (q==0){
                multiplyMatricesInt16();
            } else if (q==1){
                multiplyMatricesInt32();
            } else if (q==2){
                multiplyMatricesFloat();
            } else if (q==3){
                multiplyMatricesDouble();
            }
        }
    }
}

template <typename T>
void cacheMatrixMultiply(T val) {
    std::vector<std::vector<T>> matrix1(N, std::vector<T>(N));
    std::vector<std::vector<T>> matrix2(N, std::vector<T>(N));
    std::vector<std::vector<T>> result(N, std::vector<T>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();
    for (int jj = 0; jj < N; jj=jj+BLOCK_SIZE)
        for (int kk = 0; kk < N; kk=kk+BLOCK_SIZE)
            for (int i = 0; i < N; i++)
                for (int j=jj; j < std::min((jj+BLOCK_SIZE),N); j++){
                    int r = 0;
                    for (int k = kk; k < std::min((kk+BLOCK_SIZE),N); k++)
                        r = r + (matrix1[i][k] * matrix2[k][j]);
                    result[i][j] = result[i][j] + r;
                }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Cache optimization multiplication for two " << N << "x" << N 
    << " " << typeid(val).name() << " matrices took " << duration.count() << " milliseconds.\n";
}

void function3(){
    for (int p = 0; p<4; p++){
        N = matrix_sizes[p];
        for (int q = 0; q<4; q++){
            if (q==0){
                int16_t input = 1;
                cacheMatrixMultiply(input);
            } else if (q==1){
                int32_t input = 1;
                cacheMatrixMultiply(input);
            } else if (q==2){
                float input = 1.1;
                cacheMatrixMultiply(input);
            } else if (q==3){
                double input = 1.1;
                cacheMatrixMultiply(input);
            }
        }
    }
}

template <typename T>
void function4_inside_loop(T val){
    std::vector<std::vector<T>> matrix1(N, std::vector<T>(N));
    std::vector<std::vector<T>> matrix2(N, std::vector<T>(N));
    std::vector<std::vector<T>> result(N, std::vector<T>(N, static_cast<T>(0)));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    int userthreadin = 0;
    std::cout << "Enter a number to decide the amount of threads: ";
    std::cin >> userthreadin;
    numThreads = userthreadin;

    // Calculate the number of rows each thread will handle
    int rowsPerThread = N / numThreads;

    auto start = std::chrono::high_resolution_clock::now();
    
    // Create threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
        threads.emplace_back(&Threads<T>::multiply_with_cache, Threads<T>(matrix1, matrix2, result, startRow, endRow));
    }

    // Join threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        } else {
            std::cerr << "Thread not joinable!" << std::endl;
            // Handle the error as needed
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Threads multiplication for two " << N << "x" << N 
    << " "<< typeid(val).name() << " matrices took " << duration.count() << " milliseconds.\n";
}

void function4(){
    for (int p = 0; p<4; p++){
        N = matrix_sizes[p];
        for (int q = 0; q<4; q++){
            if (q==0){
                int16_t input = 1;
                function4_inside_loop(input);
            } else if (q==1){
                int32_t input = 1;
                function4_inside_loop(input);
            } else if (q==2){
                float input = 1.1;
                function4_inside_loop(input);
            } else if (q==3){
                double input = 1.1;
                function4_inside_loop(input);
            }
        }
    }
}

void multiplyMatricesInt16_with_cache() {
    std::vector<std::vector<int16_t>> matrix1(N, std::vector<int16_t>(N));
    std::vector<std::vector<int16_t>> matrix2(N, std::vector<int16_t>(N));
    std::vector<std::vector<int16_t>> result(N, std::vector<int16_t>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE) {
            for (int k_block = 0; k_block < N; k_block += BLOCK_SIZE) {
                for (int j = j_block; j < std::min(j_block + BLOCK_SIZE, N); ++j) {
                    __m128i sum = _mm_setzero_si128();

                    for (int k = k_block; k < std::min(k_block + BLOCK_SIZE, N); ++k) {
                        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&matrix1[i][k]));
                        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&matrix2[k][j]));
                        sum = _mm_add_epi16(sum, _mm_mullo_epi16(a, b));
                    }

                    _mm_storeu_si128(reinterpret_cast<__m128i*>(&result[i][j]), _mm_add_epi16(_mm_loadu_si128(reinterpret_cast<__m128i*>(&result[i][j])), sum));
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " int16_t matrices took " << duration.count() << " milliseconds.\n";
}

void multiplyMatricesInt32_with_cache() {
    std::vector<std::vector<int32_t>> matrix1(N, std::vector<int32_t>(N));
    std::vector<std::vector<int32_t>> matrix2(N, std::vector<int32_t>(N));
    std::vector<std::vector<int32_t>> result(N, std::vector<int32_t>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE) {
            for (int k_block = 0; k_block < N; k_block += BLOCK_SIZE) {
                for (int j = j_block; j < std::min(j_block + BLOCK_SIZE, N); ++j) {
                    __m256i sum = _mm256_setzero_si256();

                    for (int k = k_block; k < std::min(k_block + BLOCK_SIZE, N); ++k) {
                        __m256i a = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrix1[i][k]));
                        __m256i b = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrix2[k][j]));
                        sum = _mm256_add_epi32(sum, _mm256_mullo_epi32(a, b));
                    }

                    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&result[i][j]), sum);
                }
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " int32_t matrices took " << duration.count() << " milliseconds.\n";
}

void multiplyMatricesFloat_with_cache() {
    std::vector<std::vector<float>> matrix1(N, std::vector<float>(N));
    std::vector<std::vector<float>> matrix2(N, std::vector<float>(N));
    std::vector<std::vector<float>> result(N, std::vector<float>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE) {
            for (int k_block = 0; k_block < N; k_block += BLOCK_SIZE) {
                for (int j = j_block; j < std::min(j_block + BLOCK_SIZE, N); ++j) {
                    __m256 sum = _mm256_setzero_ps();

                    for (int k = k_block; k < std::min(k_block + BLOCK_SIZE, N); ++k) {
                        __m256 a = _mm256_loadu_ps(&matrix1[i][k]);
                        __m256 b = _mm256_loadu_ps(&matrix2[k][j]);
                        sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
                    }

                    _mm256_storeu_ps(&result[i][j], sum);
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " float matrices took " << duration.count() << " milliseconds.\n";
}

void multiplyMatricesDouble_with_cache() {
    std::vector<std::vector<double>> matrix1(N, std::vector<double>(N));
    std::vector<std::vector<double>> matrix2(N, std::vector<double>(N));
    std::vector<std::vector<double>> result(N, std::vector<double>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();
    
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j_block = 0; j_block < N; j_block += BLOCK_SIZE) {
            for (int k_block = 0; k_block < N; k_block += BLOCK_SIZE) {
                for (int j = j_block; j < std::min(j_block + BLOCK_SIZE, N); ++j) {
                    __m256d sum = _mm256_setzero_pd();

                    for (int k = k_block; k < std::min(k_block + BLOCK_SIZE, N); ++k) {
                        __m256d a = _mm256_loadu_pd(&matrix1[i][k]);
                        __m256d b = _mm256_loadu_pd(&matrix2[k][j]);
                        sum = _mm256_add_pd(sum, _mm256_mul_pd(a, b));
                    }

                    _mm256_storeu_pd(&result[i][j], sum);
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "SIMD multiplication for two " << N << "x" << N 
    << " double matrices took " << duration.count() << " milliseconds.\n";
}

void function5(){
    for (int p = 0; p<4; p++){
        N = matrix_sizes[p];
        for (int q = 0; q<4; q++){
            if (q==0){
                multiplyMatricesInt16_with_cache();
            } else if (q==1){
                multiplyMatricesInt32_with_cache();
            } else if (q==2){
                multiplyMatricesFloat_with_cache();
            } else if (q==3){
                multiplyMatricesDouble_with_cache();
            }
        }
    }
}

/*
template <typename T>
void function8(){
    std::vector<std::vector<double>> matrix1(N, std::vector<double>(N));
    std::vector<std::vector<double>> matrix2(N, std::vector<double>(N));
    std::vector<std::vector<double>> result(N, std::vector<double>(N));

    initializeMatrix(matrix1);
    initializeMatrix(matrix2);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) 
        for (int j = 0; j < N; j += 8) 
            for (int k = 0; k < N; ++k) 
                result[i][j] = matrix1[i][k] + matrix2[k][j];
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Threads multiplication for two " << N << "x" << N 
    << " "<< typeid(matrix1[0][0]).name() << " matrices took " << duration.count() << " milliseconds.\n";
}
*/

int main() {
    // Seed for random number generation
    std::srand(std::time(0));

    std::cout << "Enter an integer: ";
    for (int userNumber = 0; userNumber < 9; std::cin >> userNumber){

        // Print the entered integer
        std::cout << "You entered: " << userNumber << std::endl;

        if (userNumber==1) function1(); //1) multi-threads only
        else if (userNumber==2) function2(); //2) SIMD only
        else if (userNumber==3) function3(); //3) cache optimization only
        else if (userNumber==4) function4(); //4) multi-threads and cache optimzation
        else if (userNumber==5) function5(); //5) SIMD and cache optimization
        //else if (userNumber==6) function6(); //6) multi-threads and SIMD
        //else if (userNumber==7) function7(); //7) all three optimzations
        //else if (userNumber==8) function8(); //8)basic matrix multiplication with no optimization
        
        //need another user input to configure thread number, prob put inside functions

        // Prompt the user for input
        std::cout << "Enter an integer: ";
    }

    return 0;
}
