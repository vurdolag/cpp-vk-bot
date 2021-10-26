//
// Created by pechenkin on 26.10.2021.
//

#include <cstdlib>
#include <chrono>
#include <thread>
#include "Loop.h"
#include "Coro.h"


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
