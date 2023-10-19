// Threads.h

#ifndef THREADS_H
#define THREADS_H

#include <vector>

template <typename T>
class Threads {
public:
    Threads(const std::vector<std::vector<T>>& m1,
                     const std::vector<std::vector<T>>& m2,
                     std::vector<std::vector<T>>& res,
                     int start, int end);

    void multiply();
    void multiply_with_cache();
    void multiply_with_SIMD();

private:
    const std::vector<std::vector<T>>& matrix1;
    const std::vector<std::vector<T>>& matrix2;
    std::vector<std::vector<T>>& result;
    int startRow;
    int endRow;
    int BLOCK_SIZE;
};

#include "Threads.cpp"  // Include the implementation file for explicit instantiation

#endif  // Threads
