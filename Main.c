#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <TlHelp32.h>
#include <stdlib.h>

#define MAX_NAME 256

struct WinData {
    char lpWindowName[MAX_NAME + 1];
    DWORD dwProcessId;
    HANDLE hProcessHandle;
    HWND hwndWindowHandle;
    BOOL bTerminateProcess;
    UINT uExitCode;
} *winData = NULL;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts("Usage: ./ProcKiller.exe <process_name.exe>");
        return -1;
    }

    // Allocate memory for WinData structure
    winData = malloc(sizeof(struct WinData));
    if (winData == NULL) {
        printf("malloc failed for winData structure.\n");
        return -1;
    }

    // Copy contents of argv[1] into lpWindowName
    strncpy_s(winData->lpWindowName, sizeof(winData->lpWindowName), argv[1], MAX_NAME);

    // Find window handle
    winData->hwndWindowHandle = FindWindowA(NULL, winData->lpWindowName);
    if (winData->hwndWindowHandle == NULL) {
        printf("Unable to find window: %s\n", winData->lpWindowName);
        free(winData);
        return -1;
    }

    // Get process ID
    GetWindowThreadProcessId(winData->hwndWindowHandle, &winData->dwProcessId);
    if (winData->dwProcessId == 0) {
        printf("Unable to obtain PID!\n");
        free(winData);
        return -1;
    }

    // Open process
    winData->hProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, winData->dwProcessId);
    if (winData->hProcessHandle == NULL) {
        printf("Unable to open process! Error code: %lu\n", GetLastError());
        free(winData);
        return -1;
    }

    // Terminate process
    winData->uExitCode = 0;
    winData->bTerminateProcess = TerminateProcess(winData->hProcessHandle, winData->uExitCode);
    if (winData->bTerminateProcess == 0) {
        printf("Unable to terminate process! Error code: %lu\n", GetLastError());
        CloseHandle(winData->hProcessHandle);
        free(winData);
        return -1;
    }

    // Close process handle
    CloseHandle(winData->hProcessHandle);

    // Free allocated memory
    free(winData);

    return EXIT_SUCCESS;
}
