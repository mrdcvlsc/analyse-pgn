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

#if defined(__linux__)
#include <libgen.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <codecvt>
#include <locale>
#include <windows.h>
#endif

namespace apgnFileSys
{
    void deleteFile(const std::string& filename)
    {
        try {
            if (std::filesystem::remove(filename))
                std::cout << "...\n";
            else
                std::cout << "file " << filename << " not found.\n";
        }
        catch(const std::filesystem::filesystem_error& err)
        {
            throw std::system_error(err);
        }
    }

    std::string getExecpath()
    {
      #if defined(__linux__)
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX - 1);
        if (count == -1) {
            throw std::runtime_error(
                "Cannot determine executable path (readlink failed)");
        }
        result[count] = '\0';  // Null-terminate
        std::filesystem::path exePath(result);
        return exePath.parent_path().string();

        #elif defined(__APPLE__)
        uint32_t size = PATH_MAX;
        char result[PATH_MAX];
        if (_NSGetExecutablePath(result, &size) != 0) {
            throw std::runtime_error(
                "Cannot determine executable path (_NSGetExecutablePath failed)");
        }
        std::filesystem::path exePath(result);
        return exePath.parent_path().string();

        #elif defined(_WIN32)
        std::wstring wpath(MAX_PATH, L'\0');
        DWORD length =
            GetModuleFileNameW(NULL, wpath.data(), static_cast<DWORD>(wpath.size()));
        if (length == 0) {
            throw std::runtime_error(
                "Cannot determine executable path (GetModuleFileNameW failed)");
        }
        wpath.resize(length);
        std::filesystem::path exePath(wpath);
        return exePath.parent_path().string();

        #else
        throw std::runtime_error("Platform not supported");
        #endif
    }
}

#endif