
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <thread>

using namespace std;

struct RES {
    HWND w;
    const char * k;
};


DWORD GetProcessByExeName(char * ExeName)
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

void activate(HWND hwnd) {
    Sleep(1000);
    cout << "activate" << endl;
    SetFocus(hwnd);
    //GetWindow(hwnd, GW_OWNER);
    //IsWindowVisible(hwnd);
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
        activate(hwnd);
        return 0;
    }

    //MessageBoxW(nullptr, str,L"text",0)

    Sleep(50);
    return 1;
}


int main(int argc, char *argv[]) {
    //for (int i = 0; i < argc; i++) {
    //    cout << argv[i] << endl;
   // }

    int t = atoi(argv[1]) * 1000;
    char * pat1 = argv[2];
    char * pat2 = argv[3];
    char * name = argv[4];
    string command = string(argv[5]);

    for (int i = 6; i < argc; i++) {
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

        Sleep(1000);
        cout << "Click! " << name << endl;
        keybd_event(VK_RETURN, 0, 0, 0);
        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

        Sleep(1000);

        w->k = pat2;
        EnumWindows(EnumWindowsProc, (LPARAM)w);


        keybd_event(VK_LWIN, 0, 0, 0);
        Sleep(300);
        keybd_event(VK_UP, 0, 0, 0);


        Sleep(300);
        keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);


        Sleep(t);
        check(name);

        Sleep(1000);
    }

    return 0;
}