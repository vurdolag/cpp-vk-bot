#include <iostream>
#include <fstream>
#include "Loop.h"
#include "util.h"
#include "RequestSession.h"


using namespace std;


int glob = 0;
std::mutex mux;


struct Request {
    string url;
    string data;

    RequestSession * session;

    Loop * loop;

    Request(string url_, string data_): url(std::move(url_)), data(std::move(data_)) {}
};



void request(void * arg) {
    auto req = (Request*)arg;

    string buff;

    auto v = req->session->GET(req->url.c_str(), nullptr, &buff);

    ofstream out(req->data);

    out << buff.c_str();

    out.close();

    cout << v << " " << buff.size() << endl;

    req->loop->add(request, arg, get_rand(15000, 60000));
}


void bar(void * c) {
    auto a = CORO(c);

    auto p = *(string *)a->args();

    auto ad = new string(p + " + Added " + to_string(glob++));

    sleep(get_rand(10, 1000));
    a->set(ad);
}

void foo(void * arg) {
    auto loop = LOOP(arg);


    auto i = new string("Hello -- ");

    auto a = Coro(loop, bar, i, get_rand(10, 1000));

    auto val = (string *)a.wait();

    cout << glob++ << " result: " << *val << endl;

    delete val;
    delete i;

    loop->add(foo, arg, get_rand(100, 1000));
    //loop->add(foo, arg, get_rand(90, 200));

}

void fun2(void * arg);

void fun1(void * arg) {
    auto loop = LOOP(arg);

    sleep(get_rand(10, 100));
    //glob++;
    cout << glob++ << " in === === === === === === === === === === !" << endl;
    sleep(get_rand(10, 100));

    loop->add(fun2, arg, get_rand(10, 150));
    //loop->add(fun1, arg, get_rand(100, 10000));
}

void fun2(void * arg) {
    auto loop = LOOP(arg);

    sleep(get_rand(10, 100));
    cout << glob++ << " in +++ +++ +++ +++ +++ +++ +++ +++ +++ +++ !" << endl;
    //glob++;
    sleep(get_rand(10, 100));

    loop->add(fun1, arg, get_rand(10, 100));
    //loop->add(fun2, arg, get_rand(10000, 100000));
}

int main() {
    auto loop = Loop();

    auto ua = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.54 Safari/537.36";
    auto proxy = Proxy{
            "http://45.86.1.109:4060",
            "Babayka",
            "T6jQSNo5Bo"
    };

    auto name = "first.txt";

    string data;

    auto r = RequestSession(name, ua, &proxy);
    auto r2 = RequestSession(name, ua, &proxy);

    auto req = new Request("https://pikabu.ru","pikabu.txt");
    auto req2 = new Request("https://vk.com/feed", "vk.txt");
    req->loop = &loop;
    req->session = &r;
    req2->loop = &loop;
    req2->session = &r2;

    for (int i = 1; i < 2; i++) {
        loop.add(request, req, i * 1000);
        sleep(1000);
        loop.add(request, req2, i * 1000 * 4);
        sleep(1000);
        loop.add(foo, &loop, i * 1000 * 8);
        sleep(1000);
        loop.add(fun1, &loop, i * 1000 * 16);
        sleep(1000);
    }

    system("pause");

    return 0;
}


