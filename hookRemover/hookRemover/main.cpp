#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>

const DWORD OFFSET_STRUCT = 0xe000; // hard coded offset from the struct (DLL)

BOOL WriteToAddress(HANDLE hProcess, LPCVOID lpBaseAddress, bool value) {
    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, (LPVOID)lpBaseAddress, &value, sizeof(value), &bytesWritten) && (bytesWritten == sizeof(value));
}

BOOL ReadIntFromAddress(HANDLE hProcess, LPCVOID lpBaseAddress, int* buffer, SIZE_T bufferSize) {
    SIZE_T bytesRead;
    return ReadProcessMemory(hProcess, lpBaseAddress, buffer, bufferSize, &bytesRead) && (bytesRead > 0);
}

std::vector<MODULEENTRY32> EnumerateModules(DWORD processID) {
    std::vector<MODULEENTRY32> modules;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 me32;
        me32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &me32)) {
            do {
                modules.push_back(me32);
            } while (Module32Next(hSnapshot, &me32));
        }
        CloseHandle(hSnapshot);
    }
    return modules;
}

int main() {
    DWORD processID;
    std::cout << "Enter the process ID: ";
    std::cin >> processID;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::vector<MODULEENTRY32> modules = EnumerateModules(processID);

    for (const auto& moduleEntry : modules) {
        int buffer = 0;
        HMODULE module = moduleEntry.hModule;
        LPCVOID intAddr = (LPCVOID)((uintptr_t)module + OFFSET_STRUCT + 0x4);
        if (ReadIntFromAddress(hProcess, intAddr, &buffer, sizeof(buffer))) {
            LPCVOID boolAddress = (LPCVOID)((uintptr_t)module + OFFSET_STRUCT);
            if (buffer == 1337536)
                if (WriteToAddress(hProcess, boolAddress, true))
                    std::cout << "Unhooked! exiting -> Address wrote: " << boolAddress << std::endl;
        }
    }

    CloseHandle(hProcess);
    return 0;
}
