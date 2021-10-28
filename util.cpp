//
// Created by pechenkin on 26.10.2021.
//

#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>
#include "Loop.h"
#include "Coro.h"
#include "util.h"


size_t get_rand(size_t a, size_t b) {
    std::mt19937 rng(get_time_now());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(a, b); // distribution in range [1, 6]

    auto t = dist6(rng);
    //printf(" | %i | \n", t);
    return t;
}


Coro * CORO(void * val) {
    return (Coro *)val;
}

Loop * LOOP(void * val) {
    return (Loop *)val;
}

size_t get_time_now() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}

void sleep(size_t t) {
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}
