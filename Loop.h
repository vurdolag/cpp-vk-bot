//
// Created by pechenkin on 26.10.2021.
//

#ifndef AWAITEBLE_LOOP_H
#define AWAITEBLE_LOOP_H

#include <mutex>


struct Task {
    size_t time_start = 0;
    void (*func)(void *) = nullptr;
    void * arg = nullptr;

    Task(void (*func_)(void *), void * arg_, size_t time_);
    Task();
};


class Loop {
private:
    Task * tasks;
    size_t task_count = 0;
    size_t task_index = 0;
    std::mutex * mux;

    [[noreturn]] void event_loop();

    void del_last_task();

    Task * get_last_task();

    void tasks_re_alloc();

public:
    void add(void (*func)(void *), void * arg, size_t time_start = 0);
    explicit Loop(size_t task_count_);
    ~Loop();

};

#endif //AWAITEBLE_LOOP_H
