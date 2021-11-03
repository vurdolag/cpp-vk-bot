
#include "Loop.h"
#include "util.h"
#include "Coro.h"

#include <thread>
#include <iostream>
#include <algorithm>

using thread = std::thread;


inline void Task::call() {
    is_called = true;
    func(arg);
    is_ready = true;

    //delete this;
}

Task::Task(void (*func_)(void *), void * arg_, size_t time_): func(func_), arg(arg_), time_start(time_){}
Task::Task(){
    func = nullptr;
    arg = nullptr;
    time_start = 0;
}


[[noreturn]] void Worker::loop() {
    while (active){
        sleep(10);
        if (task) {
            in_process = true;
            if (task && !task->is_called) {
                task->call();
                task = nullptr;
                last_start = get_time_now();
            }
            in_process = false;
        }
    }
    loop_is_exit = true;
}

inline bool Worker::is_processed() const {
    return in_process;
}

inline bool Worker::is_active() const {
    return (active && !loop_is_exit);
}

inline bool Worker::has_task() const {
    return task != nullptr;
}

inline size_t Worker::get_last_start() {
    return last_start;
}

inline void Worker::add(Task * t){
    task = t;
}

inline void Worker::stop() {
    active = false;
}

Worker::Worker() {
    last_start = get_time_now();
    loop_thread = new std::thread(&Worker::loop, this);
    loop_thread->detach();
}

Worker::~Worker() {
    stop();
    while(!loop_is_exit) {
        sleep(20);
    }
    delete loop_thread;
}


inline Worker * ThreadPool::get_worker() {
    for (auto w: pool) {
        std::cout << w->is_processed() << " ";
    }
    std::cout << "\n";

    for (auto w: pool) {
        if (!w->is_processed() && !w->has_task() && w->is_active()) {
            return w;
        }
    }

    auto w = new Worker();
    pool.push_back(w);

    return w;
}

size_t MAX_SLEEP_THREAD_TIME = 1000000000 * (size_t)60;

inline void ThreadPool::management(size_t t) {
    auto w = pool.back();

    if (!w->is_processed() && !w->has_task() && t - w->get_last_start() > MAX_SLEEP_THREAD_TIME || !w->is_active()) {
        pool.pop_back();
        delete w;
    }
}

inline void ThreadPool::start(Task * task_) {
    auto w = get_worker();
    w->add(task_);
}

ThreadPool::ThreadPool() {
    mux = new std::mutex;
    count = 10;
    for (size_t i = 0; i < count; i++) {
        auto w = new Worker();
        pool.push_back(w);
    }
}

ThreadPool::~ThreadPool() {
    for (auto w : pool) {
        delete w;
    }
    delete mux;
}


[[noreturn]] void Loop::event_loop() {
    size_t t;
    TASKS tasks_for;

    while (true) {
        sleep(25);

        //for (auto i : tasks) {
        //    std::cout << i->time_start << std::endl;
        //}

        t = get_time_now();

        //std::cout << "tasks.size() " << tasks.size() << " " << t << std::endl;

        if (!tasks.empty()) {
            mux->lock();
            Task * tt = tasks.back();
            while (tt && tt->time_start <= t) {
                tasks_for.push_back(tt);
                tasks.pop_back();
                if (tasks.empty()) {
                    break;
                }
                tt = tasks.back();
            }
            mux->unlock();
        }

        if (!tasks_for.empty()) {
            for (auto task : tasks_for) {
                pool->start(task);
                tasks_in_process.push_back(task);
            }
            tasks_for.clear();
        }

        pool->management(t);

        dealloc_called_tasks();
    }
}

inline Task * Loop::alloc_task(void (*func)(void *), void * arg, size_t time_start) const {
    auto t = get_time_now() + time_start * 1000000;
    return new Task(func, arg, t);
}

inline void Loop::dealloc_called_tasks() {
    for (size_t i = 0; i < tasks_in_process.size(); i++) {
        auto t = tasks_in_process[i];
        if (t && t->is_called && t->is_ready) {
            delete t;
            tasks_in_process[i] = nullptr;
        }
    }

    if (!tasks_in_process.empty()) {
        std::sort(tasks_in_process.begin(), tasks_in_process.end(),
                  [](const Task * lhs, const Task * rhs) {
                      return (lhs == nullptr) < (rhs == nullptr);
                  });


        Task * t = tasks_in_process.back();
        while(!tasks_in_process.empty() && !t) {
            tasks_in_process.pop_back();
            if (tasks_in_process.empty()) {
                break;
            }
            t = tasks_in_process.back();
        }
    }
}


void Loop::add(void (*func)(void *), void * arg, size_t time_start) {
    auto task = alloc_task(func, arg, time_start);

    mux->lock();
    tasks.push_back(task);
    std::sort(tasks.begin(), tasks.end(), [](const Task * lhs, const Task * rhs) {
        return lhs->time_start > rhs->time_start;
    });
    mux->unlock();
}


void * Loop::wait(void (*func)(void *), void * arg, size_t time_start){
    return Coro(this, func, arg, time_start).wait();
}

Loop::Loop() {
    mux = new std::mutex();
    pool = new ThreadPool();
    loop_thread = new thread(&Loop::event_loop, this);
    loop_thread->detach();
}

Loop::~Loop() {
    delete pool;
    delete mux;
    delete loop_thread;
}
