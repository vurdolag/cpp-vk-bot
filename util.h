//
// Created by pechenkin on 26.10.2021.
//

#ifndef AWAITEBLE_UTIL_H
#define AWAITEBLE_UTIL_H

#include "Loop.h"
#include "Coro.h"



Coro * CORO(void * val);
Loop * LOOP(void * val);
size_t get_time_now();
void sleep(size_t t);

#endif //AWAITEBLE_UTIL_H