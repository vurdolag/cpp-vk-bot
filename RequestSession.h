//
// Created by vurdolag on 31.10.2021.
//

#ifndef AWAITEBLE_REQUESTSESSION_H
#define AWAITEBLE_REQUESTSESSION_H

#include "curl/curl.h"
#include <iostream>
#include <utility>


struct Proxy {
    std::string url;
    std::string user;
    std::string password;
};


class RequestSession {
private:
    CURL * curl = nullptr;
    const char * cookie_file_name = "cookie.txt";
    const char * user_agent = nullptr;
    Proxy * proxy = nullptr;

    void set_default_opt();

    void set_opt(std::string * buff, const char * url, bool is_post, const char * params, const char * data);

    void set_headers(bool is_post);

    void set_proxy();

    int request();


public:
    int POST(const char * url, const char * params, const char * data, std::string * buff);

    int GET(const char * url, const char * params, std::string * buff);

    RequestSession(const char * name, const char * user_agent_, Proxy * proxy_);

    ~RequestSession();
};







#endif //AWAITEBLE_REQUESTSESSION_H
