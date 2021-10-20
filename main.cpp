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

            while (task_index > 0 && m->time_start != 0 && t > m->time_start) {
                thread(m->func, m->arg).detach();
                del_last_task();
                m = get_last_task();
            }
            mux->unlock();

            this_thread::sleep_for(50ms);
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

    void tasks_re_alloc() {
        task_count *= 2;
        auto new_tasks = new Task[task_count];

        for (size_t i = 0; i < task_index; i++) {
            new_tasks[i] = tasks[i];
        }

        delete[] tasks;
        tasks = new_tasks;
    }

public:
    void add(void (*func)(void *), void * arg, size_t time_start = 0) {
        cout << "add" << endl;

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

    this_thread::sleep_for(100ms);

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

#include <Windows.h>
#include <tlhelp32.h>


DWORD GetProcessByExeName(char *ExeName)
{
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    if( hProcessSnap == INVALID_HANDLE_VALUE) {
        cout << "Error GetProcessByExeName" << endl;
        return false;
    }

    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (!strcmpi(pe32.szExeFile, ExeName)) {
                CloseHandle(hProcessSnap);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return 0;
}


void check(char * name) {
    DWORD PID = GetProcessByExeName(name);
    if (!PID) {
        cout << "Has not proc: " << name << endl;
    }

    else {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
        if (TerminateProcess(hProcess, 0)) {
            cout << "is kill: " << name << endl;
        }
        else {
            cout << "is not kill: " << name << endl;
        }
    }
}

void start(char * command) {
    system(command);
}



int main(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }

    if (argc < 4 || argc > 4) {
        cout << "error: need 3 arg" << endl;
        return 1;
    }

    char * name = argv[1];
    char * command = argv[2];
    int t = atoi(argv[3]);

    //char * name = "notepad.exe";
    //char * command = "%windir%\\system32\\notepad.exe";
    while(1) {
        thread(start, command).detach();;

        Sleep(1000);

        keybd_event(VK_RETURN, 0, 0, 0);
        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

        cout << "Start! " << name << endl;
        Sleep(t);
        check(name);
        Sleep(1000);
    }

    return 0;

    auto loop = Loop(1024);

    for (int i = 0; i < 100; i++) {
        loop.add(foo, &loop, i);
    }

    system("pause");

    return 0;
}


