//
// Created by pechenkin on 26.10.2021.
//

#ifndef AWAITEBLE_CORO_H
#define AWAITEBLE_CORO_H

#include "Loop.h"


class Coro {
private:
    void * out = nullptr;
    void * arg = nullptr;
    size_t mem_size = 0;
    size_t time_start = 0;
    Loop * loop;
    void (*func)(void *);
    bool ready = false;

public:
    void * args();

    void * wait();

    void write(void * from, size_t count);

    void set(void * val);

    Coro(Loop * loop_, void (*func_)(void *), void * arg_, size_t t_start_, size_t mem_size_ = 0);
    ~Coro();
};


#endif //AWAITEBLE_CORO_H
