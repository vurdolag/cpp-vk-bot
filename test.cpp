#include "curl/curl.h"
#include <iostream>

using str = std::string;


size_t write_callback(void *cont, size_t size, size_t n, void *u) {
    size_t real_size = size * n;
    ((str*)u)->append((char*)cont, real_size);
    return real_size;
}


void POST(str & url, str & data, str & buff) {
    std::cout << "post" << std::endl;

    CURL *curl = curl_easy_init();

    curl_slist * headers = nullptr;

    headers = curl_slist_append(headers, "Accept: */*");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::cout << "post end" << std::endl;
}



int main() {

    str buff;
    str data;
    str url = "http://postbin.org/anything";

    std::cout << "start" << std::endl;

    POST(url, data, buff);

    std::cout << buff << std::endl;
    std::cout << "end" << std::endl;

    return 0;
}