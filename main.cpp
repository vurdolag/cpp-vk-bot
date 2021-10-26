#include <iostream>
#include <bits/stdc++.h>

#include "Loop.h"
#include "util.h"


using namespace std;


void bar(void * c) {
    auto a = CORO(c);

    auto p = (char *)a->args();

    auto ad = " + Added";
    strcat(p, ad);

    sleep(3000);

    a->write(p, strlen(p));
}


void foo(void * arg) {
    auto loop = LOOP(arg);

    char i[128] = "Hello";

    auto a = Coro(loop, bar, &i, 5);

    auto val = a.wait();

    cout << "result: " << (char *)val << endl;

    loop->add(foo, arg, 10);
}


int main() {
    auto loop = Loop(1024);

    for (int i = 0; i < 100; i++) {
        loop.add(foo, &loop, i);
    }

    system("pause");

    return 0;
}


