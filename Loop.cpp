
#include "Loop.h"
#include "util.h"

#include <thread>
#include <iostream>
#include <algorithm>

using thread = std::thread;


void Task::call() const {
    func(arg);
}
void Task::clear() {
    arg = nullptr;
    func = nullptr;
    time_start = 0;
}
bool Task::is_empty() const {
    return func == nullptr;
}

Task::Task(void (*func_)(void *), void * arg_, size_t time_): func(func_), arg(arg_), time_start(time_){}
Task::Task(){
    func = nullptr;
    arg = nullptr;
    time_start = 0;
}



[[noreturn]] void Worker::loop() {
    while (true){
        if (!task.is_empty()) {
            task.call();
            task.clear();
        }
        sleep(100);
    }
}

void Worker::set(Task & task_) {
    task.func = task_.func;
    task.arg = task_.arg;
    task.time_start = task_.time_start;
}

bool Worker::check() const {
    return task.is_empty();
}

Worker::Worker() {
    std::thread(&Worker::loop, this).detach();
}


Worker * ThreadPool::alloc_new_worker() {
    auto w = new Worker();
    mux->lock();
    pool.push_back(w);
    mux->unlock();
    return w;
}

Worker * ThreadPool::get_worker() {
    for (size_t c = 0; c < 5; c++) {
        mux->lock();
        for (auto w : pool) {
            if (w->check()) {
                mux->unlock();
                return w;
            }
        }
        mux->unlock();
        sleep(100);
    }
    return alloc_new_worker();
}

void ThreadPool::start(Task * task_) {
    get_worker()->set(*task_);
}

ThreadPool::ThreadPool() {
    count = 10;
    for (size_t i = 0; i < count; i++) {
        auto w = new Worker();
        pool.push_back(w);
    }
    mux = new std::mutex;
}

ThreadPool::~ThreadPool() {
    for (auto w : pool) {
        delete w;
    }
    delete mux;
}


void Loop::event_loop() {
    Task * task;
    while (true) {
        auto t = get_time_now();

        //for (auto i : tasks) {
        //    std::cout << i->time_start << std::endl;
        //}

        mux->lock();
        task = tasks.back();
        mux->unlock();

        while (!tasks.empty() && task->time_start != 0 && t > task->time_start) {
            pool->start(task);

            mux->lock();
            tasks.pop_back();
            task = tasks.back();
            mux->unlock();
        }
        sleep(100);
    }
}


void Loop::add(void (*func)(void *), void * arg, size_t time_start) {


    auto t = get_time_now() + time_start * 10000000;
    auto tk = new Task(func, arg, t);

    //std::cout << "add " << t << std::endl;

    mux->lock();

    tasks.push_back(tk);
    std::sort(tasks.begin(), tasks.end(), [](const Task * lhs, const Task * rhs) {
        return lhs->time_start < rhs->time_start;
    });
    mux->unlock();
}

Loop::Loop(size_t task_count_): task_count(task_count_) {
    mux = new std::mutex();
    pool = new ThreadPool();
    thread(&Loop::event_loop, this).detach();
}

Loop::~Loop() {
    delete pool;
    delete mux;
}
