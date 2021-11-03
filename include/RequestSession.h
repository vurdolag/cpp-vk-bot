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

    inline void set_default_opt();

    inline void set_opt(std::string * buff, const char * url, bool is_post, const char * params, const char * data);

    inline void set_headers(bool is_post);

    inline void set_proxy();

    inline int request();


public:
    inline int POST(const char * url, const char * params, const char * data, std::string * buff);

    inline int GET(const char * url, const char * params, std::string * buff);

    RequestSession(const char * name, const char * user_agent_, Proxy * proxy_);

    ~RequestSession();
};


#endif //AWAITEBLE_REQUESTSESSION_H
