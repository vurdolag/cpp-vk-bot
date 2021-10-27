
#include "Loop.h"
#include "util.h"

#include <thread>
#include <iostream>

using thread = std::thread;


Task::Task(void (*func_)(void *), void * arg_, size_t time_): func(func_), arg(arg_), time_start(time_){}
void Task::call() {
    std::cout << "call start" << std::endl;
    func(arg);
    std::cout << "call end" << std::endl;
}
Task::Task(){}


[[noreturn]] void Worker::loop() {
    while (true){
        sleep(1000);
        std::cout << "task: " << task << std::endl;
        if (task != nullptr) {
            std::cout << "task start: " << task << std::endl;
            task->call();
            task = nullptr;
        }
    }
}

void Worker::set(Task * task_) {
    task = task_;
}

bool Worker::check() const {
    return task != nullptr;
}

Worker::Worker() {
    std::thread(&Worker::loop, this).detach();
}


Worker * ThreadPool::alloc_new_worker() {
        count = count + 1;
        auto buff = (Worker *) new char[sizeof(Worker) * count];
        for (size_t i = 0; i < count - 1; i++) {
            buff[i] = pool[i];
        }
        delete[] pool;

        pool = buff;

        return pool + (count - 1);
    }

Worker * ThreadPool::get_worker() {
        Worker * w ;

        for (size_t c = 0; c < 5; c++) {
            mux->lock();
            for (size_t i = 0; i < count; i++) {
                w = pool + i;
                if (!w->check()) {
                    mux->unlock();
                    return w;
                }
            }
            mux->unlock();
            sleep(50);
        }
        mux->lock();
        w = alloc_new_worker();
        mux->unlock();
        return w;
    }

void ThreadPool::start(Task * task_) {
        Worker * w = get_worker();

        std::cout << "worker task : " <<  task_ << std::endl;
        std::cout << "worker start: " <<  w << std::endl;

        w->set(task_);
    }

ThreadPool::ThreadPool() {
        count = 5;
        pool = (Worker *) new char[sizeof(Worker) * count];

        for (size_t i = 0; i < count; i++) {
            pool[i] = Worker();
        }
        mux = new std::mutex;
    }


void Loop::event_loop() {
    Task * task;
    while (true) {
        std::cout << "loop" << std::endl;

        auto t = get_time_now();

        mux->lock();
        task = get_last_task();
        mux->unlock();

        while (task_index > 0 && task->time_start != 0 && t > task->time_start) {
            pool->start(task);

            mux->lock();
            del_last_task();
            task = get_last_task();
            mux->unlock();
        }

        sleep(1000);
    }
}

void Loop::del_last_task() {
    if (task_index > 0) {
        tasks[task_index].time_start = 0;
        task_index--;
    } else {
        tasks->time_start = 0;
    }
}

Task * Loop::get_last_task() {
    if (task_index == 0) {
        return tasks;
    }
    return tasks + (task_index - 1);
}

void Loop::tasks_re_alloc() {
    task_count *= 2;
    auto new_tasks = new Task[task_count];

    for (size_t i = 0; i < task_index; i++) {
        new_tasks[i] = tasks[i];
    }

    delete[] tasks;
    tasks = new_tasks;
}

void Loop::add(void (*func)(void *), void * arg, size_t time_start) {
    std::cout << "add" << std::endl;

    auto t = Task(func, arg, get_time_now() + time_start * 10000000);

    mux->lock();

    if (task_index >= task_count) {
        tasks_re_alloc();
    }

    if (task_index == 0) {
        tasks[task_index++] = t;
        mux->unlock();
        return;
    }

    for (size_t i = 0; i < task_index; i++) {
        if (t.time_start > tasks[i].time_start) {
            for (size_t x = task_index; x > i; x--) {
                tasks[x] = tasks[x-1];
            }
            tasks[i] = t;
            task_index++;
            mux->unlock();
            return;
        }
    }
    tasks[task_index++] = t;
    mux->unlock();
}

Loop::Loop(size_t task_count_): task_count(task_count_) {
    mux = new std::mutex();
    tasks = new Task[task_count];
    pool = new ThreadPool();
    thread(&Loop::event_loop, this).detach();
}

Loop::~Loop() {
    delete[] tasks;
    delete pool;
    delete mux;
}
