#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <iomanip>
#include <stdio.h>
#include <Shlwapi.h>
#pragma comment( lib, "shlwapi.lib")

template <typename ... T>
__forceinline void print_bad(const char* format, T const& ... args)
{
    printf("[\033[91m!\033[0m]\033[33m ");
    printf(format, args ...);
    printf("\033[0m");
}

template <typename ... T>
__forceinline void print_good(const char* format, T const& ... args)
{
    printf("[\033[92m+\033[0m]\033[33m ");
    printf(format, args ...);
    printf("\033[0m");
}

DWORD GetProcId(const char* pn,unsigned short int fi = 0b1101)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pE;
        pE.dwSize = sizeof(pE);

        if (Process32First(hSnap, &pE))
        {
            if (!pE.th32ProcessID)
                Process32Next(hSnap, &pE);
            do
            {
                if (fi == 0b10100111001)
                    std::cout << pE.szExeFile << u8"\x9\x9\x9" << pE.th32ProcessID << std::endl;
                if (!_stricmp(pE.szExeFile, pn))
                {
                    procId = pE.th32ProcessID;
                    print_good("proces : 0x%lX\n", pE);
                    break;
                }
            } while (Process32Next(hSnap, &pE));
        }
    }
    CloseHandle(hSnap);
    return procId;
}


BOOL InjectDLL(DWORD procID,const char *procName,const char *dllPath)
{
    BOOL WPM = 0;

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);
    if (hProc != INVALID_HANDLE_VALUE)
    {
        if (hProc && hProc != INVALID_HANDLE_VALUE)
        {
            void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            WPM = WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
            if (WPM)
            {
                print_good("DLL Injected Succesfully 0x%lX\n", WPM);
                HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
                if (hThread)
                {
                    print_good("Thread Created Succesfully 0x%lX\n", hThread);
                    CloseHandle(hThread);
                    CloseHandle(hProc);
                }
                else 
                    print_bad("Failed to run thread (0x%lX)\n", GetLastError());
            }
        }
    }
    return 0;
}
int main()
{
    const char* procName = "ac_client.exe";
    const char* dllPath = "C:\\Users\\anas\\Desktop\\86.dll";
    if (PathFileExists(dllPath)== FALSE)
    {
        std::cerr << "[!]DLL file does NOT exist!" << std::endl;
        return EXIT_FAILURE;
    }
    DWORD procId = 0;
    procId = GetProcId(procName);
    if (procId == NULL)
    {
        std::cout << "Process Not found ...\nHere is a list of available process" << std::endl;
        GetProcId("skinjbir", 0b10100111001);
    }
    else 
        InjectDLL(procId, procName, dllPath);

}