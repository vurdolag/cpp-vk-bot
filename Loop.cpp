
#include "Loop.h"
#include "util.h"

#include <thread>
#include <iostream>
#include <algorithm>

using thread = std::thread;


void Task::call() {
    is_called = true;
    func(arg);

    delete this;
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
        if (!task.empty()) {
            in_process = true;
            Task * t = task.back();
            if (task.empty() || t->is_called) {
                in_process = false;
                continue;
            }
            task.pop_back();

            t->call();
            in_process = false;
        }
    }
}

int Worker::check() const {
    return in_process;
}

void Worker::add(Task * t){
    task.push_back(t);
}

void Worker::stop() {
    active = false;
}

Worker::Worker() {
    std::thread(&Worker::loop, this).detach();
}


Worker * ThreadPool::get_worker() {
    //for (auto w: pool) {
    //    std::cout << w->check() << " ";
    //}
    //std::cout << "\n";


    //std::cout << "xxx " << pool.size() << std::endl;

    for (auto w: pool) {
        if (!w->check()) {
            return w;
        }
    }

    auto w = new Worker();
    pool.push_back(w);

    return w;
}


void ThreadPool::start(Task * task_) {
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
    while (true) {
        sleep(10);

        while (!tasks.empty()) {
            auto t = get_time_now();
            mux->lock();
            Task * task = tasks.back();
            if (task->time_start >= t) {
                mux->unlock();
                break;
            }
            tasks.pop_back();
            mux->unlock();

            pool->start(task);
        }
    }
}


void Loop::add(void (*func)(void *), void * arg, size_t time_start) {
    auto t = get_time_now() + time_start * 1000000;
    auto tk = new Task(func, arg, t);

    mux->lock();
    tasks.push_back(tk);
    std::sort(tasks.begin(), tasks.end(), [](const Task * lhs, const Task * rhs) {
        return lhs->time_start > rhs->time_start;
    });
    mux->unlock();
}

Loop::Loop() {
    mux = new std::mutex();
    pool = new ThreadPool();
    thread(&Loop::event_loop, this).detach();
}

Loop::~Loop() {
    delete pool;
    delete mux;
}
