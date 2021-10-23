#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <iomanip>
#include <Shlwapi.h>
#pragma comment( lib, "shlwapi.lib")


BOOL InjectDLL(DWORD procID, const char* dllPath);
template <typename ... T>
__forceinline void print_bad(const char* format, T const& ... args)
{
    std::cout <<"[!] ";
    printf(format, args ...);
}

template <typename ... T>
__forceinline void print_info(const char* format, T const& ... args)
{
    std::cout << "[*] ";
    printf(format, args ...);

}

template <typename ... T>
__forceinline void print_good(const char* format, T const& ... args)
{
    std::cout <<"[+] ";
    printf(format, args ...);
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
                    print_good("Process : 0x%lX\n", pE);
                    break;
                }
            } while (Process32Next(hSnap, &pE));
        }
    }
    CloseHandle(hSnap);
    return procId;
}


BOOL InjectDLL(DWORD procID,const char *dllPath)
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
int main(void)
{
    std::string pname,dllpath;
    print_info("process name (The name of process to inject ) :");
    std::cin >> pname;
    print_info("dll path (Full path to the desired dll ) : ");
    std::cin >> dllpath;
    system("cls");

    if (PathFileExists(dllpath.c_str())== FALSE)
    {
        print_bad ("DLL File does NOT exist!" );
        return EXIT_FAILURE;
    }
    DWORD procId = 0;
    procId = GetProcId(pname.c_str());
    if (procId == NULL)
    {
        print_bad("Process Not found (0x%lX)\n", GetLastError());
        print_info("Here is a list of available process \n", GetLastError());
        Sleep(3500);
        system("cls");
        GetProcId("skinjbir", 0b10100111001);
    }
    else 
        InjectDLL(procId, dllpath.c_str());
    system("pause");

    return EXIT_SUCCESS;
}
