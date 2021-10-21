#ifndef APGN_WINPROCRUNNER
#define APGN_WINPROCRUNNER

#include <iostream>
#include <exception>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <string.h>

namespace apgn
{
    std::string runAndGrabOutput(const char * cmdline_in, const std::vector<std::string> cmdArguments)
    {
        PROCESS_INFORMATION processInfo;
        STARTUPINFOA startupInfo; 
        SECURITY_ATTRIBUTES saAttr; 

        HANDLE stdoutReadHandle = NULL;
        HANDLE stdoutWriteHandle = NULL;

        // char cmdline[256];
        std::string cmdline;
        // char outbuf[32768];
        std::string outbuf;
        DWORD bytes_read;
        char tBuf[257];

        DWORD exitcode;

        std::string cmdBuffer;

        cmdline = std::string(cmdline_in);

        // strcpy_s(cmdline, sizeof(cmdline), cmdline_in);

        memset(&saAttr, 0, sizeof(saAttr));
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
        saAttr.bInheritHandle = TRUE; 
        saAttr.lpSecurityDescriptor = NULL; 

        // Create a pipe for the child process's STDOUT. 
        if (!CreatePipe(&stdoutReadHandle, &stdoutWriteHandle, &saAttr, 5000))
        {
            printf("CreatePipe: %u\n", GetLastError());
            throw std::runtime_error("CreatePipe() Failed");
        }

        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (!SetHandleInformation(stdoutReadHandle, HANDLE_FLAG_INHERIT, 0))
        {
            printf("SetHandleInformation: %u\n", GetLastError());
            throw std::runtime_error("SetHandleInformation() RETURN FALSE");
        }

        memset(&startupInfo, 0, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        // startupInfo.hStdError = stdoutWriteHandle; // if I remove this this might not print the turns
        startupInfo.hStdOutput = stdoutWriteHandle;
        startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        startupInfo.dwFlags |= STARTF_USESTDHANDLES;

        // memset(&processInfo, 0, sizeof(processInfo));  // Not actually necessary

        std::string CMD_RUN_WARGS = cmdline.c_str();

        if(!cmdArguments.empty() && cmdArguments.size()!=1)
        {
            for(size_t i=1; i<cmdArguments.size(); ++i)
            {
                CMD_RUN_WARGS.push_back(' ');
                CMD_RUN_WARGS.append(cmdArguments[i]);
            }
        }

        // LPSTR CMDLINE_DATA = const_cast<LPSTR>(cmdline.c_str());
        LPSTR CMDLINE_DATA = const_cast<LPSTR>(CMD_RUN_WARGS.c_str());

        if (!CreateProcessA(NULL, CMDLINE_DATA, NULL, NULL, TRUE,
            CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, NULL, 0, &startupInfo, &processInfo))
        {
            printf("CreateProcessA: %u\n", GetLastError());
            throw std::runtime_error("Creating Child Process Failed");
        }

        CloseHandle(stdoutWriteHandle);

        // strcpy_s(outbuf, sizeof(outbuf), "");
        outbuf = std::string("");

        size_t loadingDot = 1;
        for (size_t counter=1;;counter++) {
            if (!ReadFile(stdoutReadHandle, tBuf, 256, &bytes_read, NULL))
            {
                // printf("ReadFile: %u\n", GetLastError());
                break;
            }
            if (bytes_read > 0)
            {
                tBuf[bytes_read] = '\0';
                // strcat_s(outbuf, sizeof(outbuf), tBuf);
                outbuf += std::string(tBuf);
            }
            std::string dotsLoad('.',loadingDot);
            std::cout<<"analysing moves("<<counter<<")"<<std::string(loadingDot,'.')<<"\n";
            if(loadingDot==10) loadingDot = 0;
            loadingDot++;
        }

        if (WaitForSingleObject(processInfo.hProcess, INFINITE) != WAIT_OBJECT_0)
        {
            printf("WaitForSingleObject: %u\n", GetLastError());
            throw std::runtime_error("WaitForSingleObject() Failed");
        }

        if (!GetExitCodeProcess(processInfo.hProcess, &exitcode))
        {
            printf("GetExitCodeProcess: %u\n", GetLastError());
            throw std::runtime_error("GetExitCodeProcess() Failed");
        }

        CloseHandle( processInfo.hProcess );
        CloseHandle( processInfo.hThread );

        return outbuf;
    }
}
#endif

// int main(int argc, char** argv)
// {
//     std::string grabbed = runAndGrabOutput("test.exe",{"one two"});

//     std::cout<<"\n\nThis is From the parent : \n";
//     std::cout<<grabbed<<"\n";
//     return 0;
// }