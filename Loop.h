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

    void call() const;
    void clear();
    bool is_empty() const;

    Task(void (*func_)(void *), void * arg_, size_t time_);
    Task();
};


class Worker {
private:
    Task task;

    [[noreturn]] void loop();

public:
    void set(Task & task_);

    bool check() const ;

    Worker();
};


using POOL = std::vector<Worker *>;


class ThreadPool {
private:
    POOL pool;
    size_t count = 0;
    std::mutex * mux;

    Worker * alloc_new_worker();

    Worker * get_worker();


public:
    void start(Task * task_);

    ThreadPool();
    ~ThreadPool();
};



class Loop {
private:
    std::vector<Task *> tasks;
    ThreadPool * pool = nullptr;
    size_t task_count = 0;
    size_t task_index = 0;
    std::mutex * mux;

    [[noreturn]] void event_loop();

    void del_last_task();

    Task * get_last_task();

public:
    void add(void (*func)(void *), void * arg, size_t time_start = 0);
    explicit Loop(size_t task_count_);
    ~Loop();

};

#endif //AWAITEBLE_LOOP_H
