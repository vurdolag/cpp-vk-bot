//
// Created by pechenkin on 26.10.2021.
//

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>
#include "Loop.h"
#include "Coro.h"
#include "util.h"


using str = std::string;


size_t write_callback(char * cont_buff, size_t size, size_t n, std::string * buff) {
    size_t s = size * n;
    if (buff) {
        buff->append(cont_buff, s);
    }
    return s;
}

CURLcode POST(str & url, str & data, str & buff) {
    char curlErrorBuffer[CURL_ERROR_SIZE];

    CURL *curl = curl_easy_init();

    curl_slist * headers = nullptr;

    headers = curl_slist_append(headers, "Accept: */*");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS , 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_POST, 1);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);

    CURLcode code = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (code != CURLE_OK) {
        std::cout << "Error: " << code << " " << curlErrorBuffer << std::endl;
    }

    curl_slist_free_all(headers);

    return code;
}


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
