
#include "Loop.h"
#include "util.h"

#include <thread>
#include <iostream>

using thread = std::thread;


Task::Task(void (*func_)(void *), void * arg_, size_t time_): func(func_), arg(arg_), time_start(time_){}
Task::Task(){}


void Loop::event_loop()  {
    Task * m;
    while (true) {
        auto t = get_time_now();

        mux->lock();
        m = get_last_task();

        while (task_index > 0 && m->time_start != 0 && t > m->time_start) {
            thread(m->func, m->arg).detach();
            del_last_task();
            m = get_last_task();
        }
        mux->unlock();

        sleep(50);
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
    thread(&Loop::event_loop, this).detach();
}

Loop::~Loop() {
    delete[] tasks;
    delete mux;
}