#ifndef APGN_WINPROCRUNNER
#define APGN_WINPROCRUNNER

#if defined(_WIN32) || defined(_WIN64)

#include <iostream>
#include <exception>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <string.h>

namespace apgn
{
    std::string runAndGrabOutput(const char * cmdline_in, const std::vector<std::string> cmdArguments);
}
#endif
#endif