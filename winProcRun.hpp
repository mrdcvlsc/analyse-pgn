#ifndef APGN_WINPROCRUNNER
#define APGN_WINPROCRUNNER

#if defined(_WIN32) || defined(_WIN64)

#include <iostream>
#include <exception>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <string>

namespace apgn
{
    std::string runAndGrabOutput(const char* cmdline_in, const std::vector<std::string>& cmdArguments)
    {
        PROCESS_INFORMATION processInfo;
        STARTUPINFOA startupInfo;
        SECURITY_ATTRIBUTES saAttr;

        HANDLE stdoutReadHandle = NULL;
        HANDLE stdoutWriteHandle = NULL;

        std::string outbuf;
        DWORD bytes_read;
        char tBuf[257];

        DWORD exitcode;

        std::string cmdline = cmdline_in;

        memset(&saAttr, 0, sizeof(saAttr));
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process's STDOUT.
        if (!CreatePipe(&stdoutReadHandle, &stdoutWriteHandle, &saAttr, 0)) // Buffer size set to 0 (default)
        {
            throw std::runtime_error("CreatePipe() Failed");
        }

        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (!SetHandleInformation(stdoutReadHandle, HANDLE_FLAG_INHERIT, 0))
        {
            throw std::runtime_error("SetHandleInformation() Failed");
        }

        memset(&startupInfo, 0, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        startupInfo.hStdOutput = stdoutWriteHandle;
        startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        startupInfo.dwFlags |= STARTF_USESTDHANDLES;

        // Properly construct the command line string with quotes
        std::string CMD_RUN_WARGS = "\"" + cmdline + "\"";
        for (const auto& arg : cmdArguments)
        {
            CMD_RUN_WARGS += " \"" + arg + "\"";
        }

        LPSTR CMDLINE_DATA = const_cast<LPSTR>(CMD_RUN_WARGS.c_str());

        if (!CreateProcessA(NULL, CMDLINE_DATA, NULL, NULL, TRUE,
            CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo))
        {
            CloseHandle(stdoutWriteHandle);
            CloseHandle(stdoutReadHandle);
            throw std::runtime_error("Creating Child Process Failed");
        }

        // Close the write handle to the pipe so the child process can't block on it
        CloseHandle(stdoutWriteHandle);

        // Read output
        outbuf.clear();
        while (true)
        {
            if (!ReadFile(stdoutReadHandle, tBuf, 256, &bytes_read, NULL))
            {
                if (GetLastError() == ERROR_BROKEN_PIPE) break; // Normal process termination
                throw std::runtime_error("ReadFile() Failed");
            }
            if (bytes_read > 0)
            {
                tBuf[bytes_read] = '\0';
                outbuf += tBuf;
            }
        }

        // Close the read handle
        CloseHandle(stdoutReadHandle);

        // Wait for the process to finish
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        // Get exit code
        if (!GetExitCodeProcess(processInfo.hProcess, &exitcode))
        {
            throw std::runtime_error("GetExitCodeProcess() Failed");
        }

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        return outbuf;
    }
}
#endif
#endif
