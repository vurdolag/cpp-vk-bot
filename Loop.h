//
// Created by pechenkin on 26.10.2021.
//

#ifndef AWAITEBLE_LOOP_H
#define AWAITEBLE_LOOP_H

#include <mutex>
#include <vector>


struct Task {
    size_t time_start = 0;
    void (*func)(void *) = nullptr;
    void * arg = nullptr;
    bool is_called = false;

    void call();

    Task(void (*func_)(void *), void * arg_, size_t time_);
    Task();
};


using TASKS = std::vector<Task *>;


class Worker {
private:
    TASKS task;
    bool active = true;
    bool in_process = false;

    [[noreturn]] void loop();

public:
    int check() const ;
    void add(Task * t);
    void stop();

    Worker();
};


using POOL = std::vector<Worker *>;


class ThreadPool {
private:
    POOL pool;
    size_t count = 0;
    std::mutex * mux;

    Worker * get_worker();


public:
    void start(Task * task_);

    ThreadPool();
    ~ThreadPool();
};



class Loop {
private:
    TASKS tasks;
    ThreadPool * pool = nullptr;
    std::mutex * mux;

    [[noreturn]] void event_loop();

public:
    void add(void (*func)(void *), void * arg, size_t time_start = 0);

    explicit Loop();
    ~Loop();

};

#endif //AWAITEBLE_LOOP_H
