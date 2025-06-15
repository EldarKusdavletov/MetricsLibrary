#include <iostream>
#include <random>
#include <cassert>

#include "utils.h"

float simulateCPU(int cpu_cores) {
    assert(cpu_cores > 0);
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> dist(0.0f, static_cast<float>(cpu_cores));
    return dist(rng);
}

int simulateRPS(int min, int max) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}