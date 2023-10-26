// TS.h

#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <immintrin.h>

template <typename T>
class TS {
public:
    TS(const std::vector<std::vector<T>>& matrixA, const std::vector<std::vector<T>>& matrixB,
    int N, int numT);
    void multiply();
    const std::vector<std::vector<T>>& getResult() const;

private:
    const std::vector<std::vector<T>>& matrixA;
    const std::vector<std::vector<T>>& matrixB;
    std::vector<std::vector<T>> result;
    int SIZE;
    int numThreads;

    void multiplyBlock(int startRow, int endRow);
};
