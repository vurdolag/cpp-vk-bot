#include <iostream>
#include <bits/stdc++.h>

#include "Loop.h"
#include "util.h"


using namespace std;


void bar(void * c) {
    auto a = CORO(c);

    auto p = *(string *)a->args();

    auto ad = new string(p + " + Added");

    sleep(100);

    a->set(ad);
}

int glob = 0;


void foo(void * arg) {
    auto loop = LOOP(arg);


    auto i = new string("Hello");

    auto a = Coro(loop, bar, i, 5);

    auto val = (string *)a.wait();

    cout << glob++ << " result: " << *val << endl;

    delete val;
    delete i;

    loop->add(foo, arg, 2);

}


void fun(void *) {
    sleep(1000);
    cout << "in fun!" << endl;
    sleep(1000);
}

int main() {
    auto loop = Loop(1024);

    for (int i = 0; i < 100; i++) {
        loop.add(foo, &loop, i);
        //loop.add(fun, nullptr, i * 5);
    }

    system("pause");

    return 0;
}


