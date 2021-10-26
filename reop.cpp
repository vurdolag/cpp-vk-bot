
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <thread>

using namespace std;

struct RES {
    HWND w;
    const char * k;
};


DWORD GetProcessByExeName(char * ExeName) {
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

void activate(HWND hwnd, int param) {
    Sleep(1000);
    cout << "Show!" << endl;
    ShowWindow (hwnd, SW_HIDE);
    Sleep(300);
    ShowWindow(hwnd, param);
    Sleep(1000);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto r = (RES*)lParam;

    WCHAR str[32];

    auto b = GetWindowTextW(hwnd, str, 32);
    if (!b) {
        return 1;
    }

    printf("%ls\n", str);

    wstring v = wstring(str);

    WCHAR pat[32];

    auto ptr = r->k;
    for (int i = 0; i < 32; i++) {
        pat[i] = ptr[i];
    }

    wstring p = pat;

    int x = v.find(p);

    printf("%i : %ls : %ls\n\n", x, pat, v.c_str());

    if (x != -1) {
        printf("Find -> %i : %ls : %ls\n\n", x, pat, v.c_str());
        r->w = hwnd;
        return 0;
    }

    //MessageBoxW(nullptr, str,L"text",0)

    Sleep(100);
    return 1;
}

void click_enter(char * name) {
    Sleep(1000);
    cout << "Click! " << name << endl;
    keybd_event(VK_RETURN, 0, 0, 0);
    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

    Sleep(1000);
}

void Close(HWND hwnd) {
    cout << "Close!" << endl;
    Sleep(300);
    SendMessageA(hwnd, WM_CLOSE, 0, 0);
    Sleep(300);
}

int main(int argc, char *argv[]) {
    cout << "params :" << endl;
    for (int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }
    cout << "# # # # # # # # # # # # #" << endl;

    int t = atoi(argv[1]) * 1000;
    char * pat1 = argv[2];
    char * pat2 = argv[3];
    char * pat3 = argv[4];
    char * name = argv[5];
    string command = string(argv[6]);

    for (int i = 7; i < argc; i++) {
        command += " " + string(argv[i]);
    }
    command = "\"" + command + "\"";
    char * command_c = (char *)command.c_str();

    RES * w = new RES;

    while(true) {
        cout << "Start! " << name << " " << command_c << endl;
        thread(start, command_c).detach();

        Sleep(1000 * 13);

        w->k = pat1;
        EnumWindows(EnumWindowsProc, (LPARAM)w);
        activate(w->w, SW_MAXIMIZE);

        Sleep(1000);

        click_enter(name);

        w->k = pat2;
        EnumWindows(EnumWindowsProc, (LPARAM)w);
        activate(w->w, SW_MAXIMIZE);

        w->k = pat3;
        EnumWindows(EnumWindowsProc, (LPARAM)w);
        activate(w->w, SW_HIDE);

        Sleep(t);

        activate(w->w, SW_MAXIMIZE);
        Close(w->w);

        Sleep(1000);
    }

    return 0;
}