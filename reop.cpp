
#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <thread>

using namespace std;

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


int main(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }

    int t = atoi(argv[1]) * 1000;
    char * name = argv[2];

    string command = string(argv[3]);

    for (int i = 4; i < argc; i++) {
        command += " " + string(argv[i]);
    }

    command = "\"" + command + "\"";

    char * command_c = (char *)command.c_str();

    //char * name = "notepad.exe";
    //char * command = "%windir%\\system32\\notepad.exe";
    while(true) {
        cout << "Start! " << name << " " << command_c << endl;
        thread(start, command_c).detach();;

        Sleep(1000 * 10);

        cout << "Click! " << name << endl;
        keybd_event(VK_RETURN, 0, 0, 0);
        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

        Sleep(t);
        check(name);

        Sleep(1000);
    }

    return 0;
}