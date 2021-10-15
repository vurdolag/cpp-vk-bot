#include <iostream>
#include <thread>
#include <chrono>


using namespace std;


static size_t global_time_start = time(nullptr);


struct Task {
    size_t time_start = 0;
    void (*func)(void *) = nullptr;
    void * arg = nullptr;

    Task(void (*func_)(void *), void * arg_, size_t time_): func(func_), arg(arg_), time_start(time_){}
    Task(){}


};



class Loop {
private:
    Task * tasks;
    size_t task_count = 0;
    size_t task_index = 0;

    [[noreturn]] void worker()  {
        while (true) {
            auto t = time(nullptr);

            auto m = get_min();

            while (t > m->time_start) {
                thread(m->func, m->arg).detach();
                del_min();
                m = get_min();
            }

            cout << "tick" << endl;

            this_thread::sleep_for(1000ms);
        }
    }

    Task * get_min() {
        return &tasks[task_index];
    }

    void del_min() {
        tasks[task_index--] = Task();
    }


public:
    void add(void (*func)(void *), void * arg, size_t time_start = 0) {
        cout << "add" << endl;

        auto t = Task(func, arg, time(nullptr) + time_start);

        if (task_index == 0) {
            tasks[task_index++] = t;
            return;
        }

        for (size_t i = 0; i < task_index; i++) {
            if (t.time_start < tasks[i].time_start) {
                for (size_t x = task_index; x <= task_index; x--) {
                   tasks[x] = tasks[x-1];
                }
                tasks[i] = t;
                task_index++;
                return;
            }
        }
        tasks[task_index++] = t;
    }


    Loop(size_t task_count_): task_count(task_count_) {
        tasks = new Task[task_count];
        thread(&Loop::worker, this).detach();
    }
};



void foo(void * arg) {
    auto a = (int*)arg;

    cout << *a << endl;
}




int main() {
    auto loop = Loop(1024);

    int i = 456;
    int i2 = 789;


    loop.add(foo, &i, 5);
    loop.add(foo, &i2, 10);
    loop.add(foo, &i, 25);


    system("pause");

    return 0;
}


