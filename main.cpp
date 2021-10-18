#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;


size_t get_time_now() {
    return chrono::system_clock::now().time_since_epoch().count();
}


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
    mutex * mux;

    [[noreturn]] void event_loop()  {
        Task * m;

        while (true) {
            auto t = get_time_now();

            mux->lock();
            m = get_last_task();

            //cout << m->time_start << " " << t << endl;

            while (task_index > 0 && m->time_start != 0 && t > m->time_start) {
                thread(m->func, m->arg).detach();
                del_last_task();
                m = get_last_task();
                cout << "." << endl;
            }
            mux->unlock();

            //cout << "tick" << endl;

            this_thread::sleep_for(1000ms);
        }
    }

    void del_last_task() {
        if (task_index > 0) {
            tasks[task_index].time_start = 0;
            task_index--;
        } else {
            tasks->time_start = 0;
        }
    }

    Task * get_last_task() {
        if (task_index == 0) {
            return tasks;
        }
        return tasks + (task_index - 1);
    }

public:
    void add(void (*func)(void *), void * arg, size_t time_start = 0) {
        cout << "add" << endl;

        auto t = Task(func, arg, get_time_now() + time_start * 10000000);

        mux->lock();

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

    Loop(size_t task_count_): task_count(task_count_) {
        mux = new mutex();
        tasks = new Task[task_count];
        thread(&Loop::event_loop, this).detach();
    }

    ~Loop() {
        delete[] tasks;
        delete mux;
    }
};


template<typename T, typename T_arg = T, typename T_val = T>
struct Coro {
    T * result = nullptr;
    T_arg * arg = nullptr;
    size_t arg_count = 1;
    size_t time_start = 0;
    Loop * loop;
    void (*func)(void *);
    bool ready = false;


    T * send() {
        loop->add(func,this, time_start);
        return get();
    }

    T * get() {
        while (true) {
            if (ready) {
                break;
            }
            this_thread::sleep_for(50ms);
        }
        return result;
    }

    void set(T_val val) {
        result = new T(val);
        ready = true;
    }

    Coro(Loop * loop_, void (*func_)(void *), T_arg * arg_, size_t time_start_): loop(loop_), func(func_), arg(arg_), time_start(time_start_) {}
};



void bar(void * c) {
    auto i = (Coro<string> *)c;
    string p = *i->arg;

    this_thread::sleep_for(3000ms);

    auto val = p + " + " + p;

    i->set(val);
}


void foo(void * arg) {
    auto loop = (Loop *)arg;

    auto i = string("Hello");

    auto c = Coro<string>(loop, bar, &i, 5);

    string * val = c.send();

    cout << "result " << val->c_str() << endl;

    loop->add(foo, arg, 5);
}


int main() {
    auto loop = Loop(1024);

    //loop.add(foo, &loop, 600);
    loop.add(foo, &loop, 5);
    loop.add(foo, &loop, 4);
    loop.add(foo, &loop, 6);
    loop.add(foo, &loop, 3);
    loop.add(foo, &loop, 7);

    system("pause");

    return 0;
}


