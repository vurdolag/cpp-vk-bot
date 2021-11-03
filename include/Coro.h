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
    Loop * loop = nullptr;
    FUNC func = nullptr;
    size_t mem_size = 0;
    size_t time_start = 0;
    bool ready = false;

public:
    inline void * args();

    inline void * wait();

    inline void write(void * from, size_t count);

    inline void send(void * val);

    Coro(Loop * loop_, FUNC func_, void * arg_, size_t t_start_ = 0, size_t mem_size_ = 0);
    ~Coro();
};


#endif //AWAITEBLE_CORO_H
