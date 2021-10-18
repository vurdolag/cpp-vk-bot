//
// Created by pechenkin on 18.10.2021.
//

#ifndef AWAITEBLE_SESSION_H
#define AWAITEBLE_SESSION_H


struct Proxy {
    string login;
    string password;
    string host;
    int port;

    Proxy(string host_, int port_, string login_ = "", string password_ = "")
            : host(host_), port(port_), login(login_), password(password_) {}
};

struct Cookie {
    string val;
    string path;

    void set(string & val);
    void get();
    void save();

    Cookie(string path_): path(path_) {}

};


class Session {
private:
    string login = "";
    string password = "";

    string user_agent = "";

    Proxy * proxy;
    Cookie * cookie;

    int id = 0;
    string name = "";
    bool is_auth = false;

    void get_id();
    void get_name();



public:
    bool auth();
    bool check();


    Session(string login_, string password_, string user_agent_ = "", Proxy * proxy_ = nullptr)
    : login(login_), password(password_), user_agent(user_agent_), proxy(proxy_) {}

    ~Session() {
        delete proxy;
        delete cookie;
    }

};

#endif //AWAITEBLE_SESSION_H
