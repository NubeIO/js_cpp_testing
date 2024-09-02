//
// Created by craig on 19/08/2024.
//
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

double complexComputation() {
    std::vector<double> arr;
    arr.reserve(1e6);  // Reserve space for 1 million elements to avoid repeated reallocations

    for (int i = 0; i < 1e6; i++) {
        arr.push_back(std::sqrt(i) * std::sin(i));
    }

    double sum = 0;
    for (double value : arr) {
        sum += value;
    }

    return sum;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    double result = complexComputation();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Complex Computation took " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Result: " << result << std::endl;

    return 0;
}
