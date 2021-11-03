//
// Created by pechenkin on 26.10.2021.
//

#ifndef AWAITEBLE_LOOP_H
#define AWAITEBLE_LOOP_H

#include <mutex>
#include <vector>
#include <thread>

using FUNC = void (*)(void *);

struct Task {
    size_t time_start = 0;
    FUNC func = nullptr;
    void * arg = nullptr;
    bool is_called = false;
    bool is_ready = false;

    inline void call();

    Task(FUNC func_, void * arg_, size_t time_);
    Task();
};


using TASKS = std::vector<Task *>;
using TASKS_IN_PROCESS = std::vector<Task *>;


class Worker {
private:
    Task * task = nullptr;
    std::thread * loop_thread = nullptr;
    size_t last_start = 0;
    bool active = true;
    bool in_process = false;
    bool loop_is_exit = false;

    void loop();

public:
    inline size_t get_last_start();
    inline bool is_processed() const;
    inline bool is_active() const;
    inline bool has_task() const;
    inline void add(Task * t);
    inline void stop();

    Worker();
    ~Worker();
};


using POOL = std::vector<Worker *>;


class ThreadPool {
private:
    POOL pool;
    size_t count = 0;
    std::mutex * mux;

    inline Worker * get_worker();

public:
    inline void management(size_t t);
    inline void start(Task * task_);

    ThreadPool();
    ~ThreadPool();
};


class Loop {
private:
    TASKS tasks;
    TASKS_IN_PROCESS tasks_in_process;
    ThreadPool * pool = nullptr;
    std::thread * loop_thread = nullptr;
    std::mutex * mux;

    [[noreturn]] void event_loop();

    inline void dealloc_called_tasks();
    inline Task * alloc_task(FUNC func, void * arg, size_t t) const;

public:
    inline void add(FUNC func, void * arg, size_t time_start = 0);

    inline void * wait(FUNC func, void * arg, size_t time_start = 0);

    explicit Loop();
    ~Loop();

};

#endif //AWAITEBLE_LOOP_H
