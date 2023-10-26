// simd.h
#ifndef SIMD_H
#define SIMD_H

#include <iostream>
#include <vector>
#include <immintrin.h>

template <typename T>
class SIMD {
public:
    using Matrix = std::vector<std::vector<T>>;

    SIMD();

    void initializeMatrices();

    void multiplyMatrices();

private:
    int N;
    Matrix matrix1;
    Matrix matrix2;
    Matrix result;
};

#endif  // SIMD_H