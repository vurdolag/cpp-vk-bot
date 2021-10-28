#include <iostream>
#include <bits/stdc++.h>

#include "Loop.h"
#include "util.h"


using namespace std;


void bar(void * c) {
    auto a = CORO(c);

    auto p = *(string *)a->args();

    auto ad = new string(p + " + Added");

    sleep(get_rand(100, 200));

    a->set(ad);
}

int glob = 0;
std::mutex mux;


void foo(void * arg) {
    auto loop = LOOP(arg);


    auto i = new string("Hello -- ");

    auto a = Coro(loop, bar, i, get_rand(10, 50));

    auto val = (string *)a.wait();

    cout << glob++ << " result: " << *val << endl;

    delete val;
    delete i;

    loop->add(foo, arg, get_rand(100, 500));
    loop->add(foo, arg, get_rand(90, 200));

}

void fun2(void * arg);

void fun1(void * arg) {
    auto loop = LOOP(arg);

    sleep(get_rand(300, 10000));
    mux.lock();
    glob++;
    cout << glob << " in === ===!" << endl;
    mux.unlock();
    sleep(get_rand(300, 10000));

    loop->add(fun2, arg, get_rand(10000, 100000));
    loop->add(fun1, arg, get_rand(100, 10000));
}

void fun2(void * arg) {
    auto loop = LOOP(arg);


    sleep(get_rand(100, 1000));
    mux.lock();
    glob++;
    cout << glob << " in +++!" << endl;
    mux.unlock();
    sleep(get_rand(100, 1000));

    loop->add(fun1, arg, get_rand(10000, 100000));
    loop->add(fun2, arg, get_rand(10000, 100000));

}

int main() {
    auto loop = Loop();

    for (int i = 1; i < 11; i++) {
        //loop.add(foo, &loop, i);
        loop.add(foo, &loop, i * 1000);
        sleep(100);
    }

    system("pause");

    return 0;
}


