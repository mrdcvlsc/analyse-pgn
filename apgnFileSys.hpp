#ifndef APGN_FILESYS_HPP
#define APGN_FILESYS_HPP

#include <iostream>
#include <filesystem>
#include <string>

#include <limits.h>

#if defined(__linux__)
#include <libgen.h>
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <locale>
#include <codecvt>
#endif

namespace apgnFileSys
{
    void deleteFile(const std::string& filename);
    std::string getExecpath();
}

#endif