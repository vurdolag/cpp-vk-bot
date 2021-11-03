//
// Created by pechenkin on 26.10.2021.
//

#ifndef AWAITEBLE_UTIL_H
#define AWAITEBLE_UTIL_H

#include "Loop.h"
#include "Coro.h"
#include "curl/curl.h"


CURLcode POST(std::string & url, std::string & data, std::string & buff);
inline size_t get_rand(size_t a, size_t b);
inline Coro * CORO(void * val);
inline Loop * LOOP(void * val);
inline size_t get_time_now();
inline void sleep(size_t t);

#endif //AWAITEBLE_UTIL_H
