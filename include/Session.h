//
// Created by pechenkin on 18.10.2021.
//

#ifndef AWAITEBLE_SESSION_H
#define AWAITEBLE_SESSION_H


#include <utility>

#include "RequestSession.h"
#include "Methods.h"


class Methods;


class Session {
private:
    std::string login = "";
    std::string password = "";

    std::string user_agent = "";

    Proxy * proxy;
    RequestSession * session;
    Methods * methods;


    size_t id = 0;
    std::string name = "";
    bool is_auth = false;

    void get_id();
    void get_name();



public:
    bool auth();
    bool check();

    Session(std::string login_, std::string password_, std::string user_agent_ = "", Proxy * proxy_ = nullptr);
    ~Session();

};

#endif //AWAITEBLE_SESSION_H
