#define STATUS_UNSUCCESSFUL 0xC0000001
#include <Windows.h>
#include <MinHook.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <winternl.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

typedef NTSTATUS(NTAPI* NtCreateFile_t)(
    PHANDLE            FileHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK   IoStatusBlock,
    PLARGE_INTEGER     AllocationSize,
    ULONG              FileAttributes,
    ULONG              ShareAccess,
    ULONG              CreateDisposition,
    ULONG              CreateOptions,
    PVOID              EaBuffer,
    ULONG              EaLength
    );

typedef NTSTATUS(NTAPI* NtDelayExecution_t)(
    BOOL               Alertable,
    PLARGE_INTEGER     DelayInterval
    );

NtDelayExecution_t fpNtDelayExecution = nullptr;
NtCreateFile_t fpNtCreateFile = nullptr;

std::string UnicodeStringToString(PUNICODE_STRING unicodeString)
{
    std::wstring ws(unicodeString->Buffer, unicodeString->Length / sizeof(WCHAR));
    std::string s(ws.begin(), ws.end());
    return s;
}

typedef struct MM_HOOK_STRUCT {
    bool AvoidHook = false;
    int ValueToCheck = 1337536;
};

MM_HOOK_STRUCT MMgStruct;

NTSTATUS NTAPI HookedNtCreateFile(
    PHANDLE            FileHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK   IoStatusBlock,
    PLARGE_INTEGER     AllocationSize,
    ULONG              FileAttributes,
    ULONG              ShareAccess,
    ULONG              CreateDisposition,
    ULONG              CreateOptions,
    PVOID              EaBuffer,
    ULONG              EaLength
)
{
    if (ObjectAttributes && ObjectAttributes->ObjectName)
    {
        std::string fileName = UnicodeStringToString(ObjectAttributes->ObjectName);

        if (fileName != "\\??\\C:\\Log.txt" && fileName.starts_with("\\??\\C:\\Windows\\Prefetch\\SERVER")) // here you need to change to your file, in my case "SERVER"
        {
            std::ofstream log("C:\\Log.txt", std::ios::app);
            if (log.is_open())
            {
                log << "CREATING OUR PROGRAM, LETS NULL THIS MONKEY: " << fileName << std::endl;
                log << "RETURNED STATUS_UNSUCCESS! " << std::endl;
                log.close();

                return STATUS_UNSUCCESSFUL;
            }
        }
    }

    return fpNtCreateFile(
        FileHandle,
        DesiredAccess,
        ObjectAttributes,
        IoStatusBlock,
        AllocationSize,
        FileAttributes,
        ShareAccess,
        CreateDisposition,
        CreateOptions,
        EaBuffer,
        EaLength
    );
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, [](LPVOID lpParam) -> DWORD {
            HMODULE hModule = static_cast<HMODULE>(lpParam);

            if (MH_Initialize() != MH_OK)
                return FALSE;

            if (MH_CreateHookApi(L"ntdll", "NtCreateFile", &HookedNtCreateFile, reinterpret_cast<LPVOID*>(&fpNtCreateFile)) != MH_OK)
                return FALSE;

            if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
                return FALSE;

            /*HMODULE baseAddress = hModule;

            std::ofstream log("C:\\Log.txt", std::ios::app);
            if (log.is_open())
            {
                if (baseAddress) {
                    log << "baseAddress: 0x" << baseAddress << std::endl;
                    PVOID structAddress = &MMgStruct;
                    log << "structAddress: 0x" << structAddress << std::endl;
                    PVOID relativeStructAddress = (PVOID)((uintptr_t)structAddress - (uintptr_t)baseAddress);
                    log << "Pointer to globalStruct (relative to module base): 0x" << relativeStructAddress << std::endl;

                    uintptr_t avoidHookAddress = reinterpret_cast<uintptr_t>(&MMgStruct.AvoidHook);
                    uintptr_t hookIDStringAddress = reinterpret_cast<uintptr_t>(&MMgStruct.ValueToCheck);
                    uintptr_t relativeAvoidHookAddress = avoidHookAddress - (uintptr_t)baseAddress;
                    uintptr_t relativeHookIDStringAddress = hookIDStringAddress - (uintptr_t)baseAddress;

                    log << "Pointer to AvoidHook (relative to module base): 0x" << std::hex << relativeAvoidHookAddress << std::endl;
                    log << "Pointer to HookIDString (relative to module base): 0x" << std::hex << relativeHookIDStringAddress << std::endl;

                    log << MMgStruct.ValueToCheck << std::endl;
                }
                log << "TICKING!!!" << std::endl;
                log.close();
            }*/ // debug <3

            while (!MMgStruct.AvoidHook)
            {
                Sleep(1000);
            }

            MH_DisableHook(MH_ALL_HOOKS);
            MH_Uninitialize();
            FreeLibraryAndExitThread(hModule, 0);

            return 0;
            }, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
