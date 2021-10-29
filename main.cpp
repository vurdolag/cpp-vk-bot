#include <iostream>
#include "Loop.h"
#include "util.h"
#include "curl/curl.h"


using namespace std;


int glob = 0;
std::mutex mux;


struct Request {
    string method;
    string url;
    string data;

    Loop * loop;

    Request(string method_, string url_, string data_):
        method(std::move(method_)), url(std::move(url_)), data(std::move(data_)) {}
};



void request(void * arg) {
    auto req = (Request*)arg;

    string buff;

    POST(req->url, req->data, buff);

    cout << buff << endl;

    req->loop->add(request, arg, get_rand(1000, 30000));
}


void bar(void * c) {
    auto a = CORO(c);

    auto p = *(string *)a->args();

    auto ad = new string(p + " + Added " + to_string(glob));

    sleep(10);

    a->set(ad);
}

void foo(void * arg) {
    auto loop = LOOP(arg);


    auto i = new string("Hello -- ");

    auto a = Coro(loop, bar, i, 100);

    auto val = (string *)a.wait();

    cout << glob++ << " result: " << *val << endl;

    delete val;
    delete i;

    loop->add(foo, arg, get_rand(10000, 100000));
    //loop->add(foo, arg, get_rand(90, 200));

}

void fun2(void * arg);

void fun1(void * arg) {
    auto loop = LOOP(arg);

    sleep(get_rand(300, 10000));
    cout << glob++ << " in === === === === === === === === === === !" << endl;
    sleep(get_rand(300, 10000));

    loop->add(fun2, arg, get_rand(10000, 100000));
    //loop->add(fun1, arg, get_rand(100, 10000));
}

void fun2(void * arg) {
    auto loop = LOOP(arg);


    sleep(get_rand(100, 1000));
    cout << glob++ << " in +++ +++ +++ +++ +++ +++ +++ +++ +++ +++ !" << endl;
    sleep(get_rand(100, 1000));

    loop->add(fun1, arg, get_rand(10000, 100000));
    //loop->add(fun2, arg, get_rand(10000, 100000));

}

int main() {
    auto loop = Loop();

    auto req = new Request("POST","https://httpbin.org/anything","key=val");
    req->loop = &loop;


    for (int i = 1; i < 3; i++) {
        loop.add(request, req, i * 1000 * 3);
        sleep(100);
        loop.add(foo, &loop, i * 1500);
        sleep(100);
    }

    system("pause");

    return 0;
}


