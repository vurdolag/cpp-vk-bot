//
// Created by pechenkin on 26.10.2021.
//

#include "Coro.h"
#include "util.h"
#include <iostream>
#include <cstdlib>


inline void * Coro::args() {
    return arg;
}

inline void * Coro::wait() {
    loop->add(func,this, time_start);

    while (true) {
        if (ready) {
            break;
        }
        sleep(10);
    }
    return out;
}

inline void Coro::write(void * from, size_t count) {
    if (mem_size > 0) {
        memcpy(out, from, count);
    } else {
        out = from;
    }
    ready = true;
}

inline void Coro::send(void * val) {
    out = val;
    ready = true;
}

Coro::Coro(Loop * loop_, FUNC func_, void * arg_, size_t t_start_, size_t mem_size_)
    : loop(loop_), func(func_), arg(arg_), time_start(t_start_) {
        if (mem_size_ > 0) {
            mem_size = mem_size_;
            out = (void *) new char[mem_size_];
        }
}

Coro::~Coro(){
    if (mem_size > 0 && out != nullptr) {
        std::cout << "Coro del" << std::endl;

        delete (char *)out;
    }
}


