//
// Created by pechenkin on 26.10.2021.
//

#include "Coro.h"
#include "util.h"
#include <iostream>
#include <cstdlib>


void * Coro::args() {
    return arg;
}

void * Coro::wait() {
    loop->add(func,this, time_start);

    while (true) {
        if (ready) {
            break;
        }
        sleep(50);
    }
    return out;
}

void Coro::write(void * from, size_t count) {
    if (mem_size > 0) {
        memcpy(out, from, count);
    } else {
        out = from;
    }
    ready = true;
}

void Coro::set(void * val) {
    out = val;
    ready = true;
}

Coro::Coro(Loop * loop_, void (*func_)(void *), void * arg_, size_t t_start_, size_t mem_size_)
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


