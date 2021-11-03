//
// Created by vurdolag on 03.11.2021.
//

#include "Session.h"



void Session::get_id(){

}

void Session::get_name(){

}

bool Session::auth(){

}

bool Session::check(){

}

Session::Session(std::string login_, std::string password_, std::string ua, Proxy * proxy_)
            : login(std::move(login_)),
              password(std::move(password_)),
              user_agent(std::move(ua)),
              proxy(proxy_)
{
        methods = new Methods();
        methods->set_session(this);

        session = new RequestSession(login.c_str(), ua.c_str(), proxy_);
}

Session::~Session() {
        delete session;
}
