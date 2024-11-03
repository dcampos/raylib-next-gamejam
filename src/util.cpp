#include <iostream>
#include <random>
#include "util.hpp"

bool inRange(float a, float b, float width) {
    return a >= b && a < b + width;
}

float randomNumber(int from, int to) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(from, to);

    int r = dist(gen);
    return r;
}
