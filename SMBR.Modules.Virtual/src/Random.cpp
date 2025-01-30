#include "Random.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <random>

double Random::nextDouble(double min, double max) {
    return min + (max - min) * rand() / RAND_MAX;
}

float Random::nextFloat(float min, float max) {
    return min + (max - min) * rand() / RAND_MAX;
}

int Random::nextInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

void Random::randomDelay() {
    //make random number between 100 and 1000
    int rndSleep = rand() % 1000 + 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(rndSleep));
}