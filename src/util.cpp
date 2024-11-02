#include <iostream>
#include <random>
#include "util.hpp"

float randomNumber(int from, int to) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(from, to);

    int r = dist(gen);
    return r;
}
