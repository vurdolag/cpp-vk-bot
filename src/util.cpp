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



size_t write_callback_(char * cont_buff, size_t size, size_t n, std::string * buff) {
    size_t s = size * n;
    if (buff) {
        buff->append(cont_buff, s);
    }
    return s;
}

CURLcode POST(str & url, str & data, str & buff) {
    std::cout << "Request" << std::endl;

    char curlErrorBuffer[CURL_ERROR_SIZE];

    CURL * curl = curl_easy_init();

    curl_slist * headers = nullptr;

    headers = curl_slist_append(headers, "Accept: */*");
    headers = curl_slist_append(headers, "accept-language: en-US,en;q=0.9,ru;q=0.8,ja;q=0.7");

    curl_easy_setopt(curl, CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.54 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate,br");
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);


    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS , 10);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 30);
    //curl_easy_setopt(curl, CURLOPT_POST, 1);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);

    //curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);


    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

    //curl_easy_setopt(curl, CURLOPT_COOKIE, "name=daniel; present=yes;");



    CURLcode code = curl_easy_perform(curl);

    if (code != CURLE_OK) {
        std::cout << "Error: " << code << " " << curlErrorBuffer << std::endl;
    }

    curl_slist * cookies = nullptr;
    curl_slist * each = nullptr;

    auto res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
    if(!res && cookies) {
        /* a linked list of cookies in cookie file format */
        each = cookies;
        while(each) {
            std::cout << "cookie: " << each->data << std::endl;
            each = each->next;
        }
        //curl_slist_free_all(cookies);
    }

    int status = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    std::cout << "status code: " << status << std::endl;


    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    std::cout << "Request END" << std::endl;

    return code;
}


size_t get_rand(size_t a, size_t b) {
    std::mt19937 rng(get_time_now());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(a, b);

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
