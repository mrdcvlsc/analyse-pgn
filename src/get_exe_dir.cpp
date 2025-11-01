#include "get_exe_dir.hpp"
#include "logger.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <sstream>

#if defined(__linux__)
#include <linux/limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#include <codecvt>
#include <locale>
#include <stdlib.h>
#include <windows.h>
#endif

std::string get_exe_dir() {
#if defined(__linux__)
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX - 1);

    if (count == -1) {
        DEBUG_ERR("Cannot determine executable path (readlink failed)");
    }

    result[count] = '\0'; // Null-terminate
    std::filesystem::path exePath(result);
    DEBUG_LOG("get_exe_path() return -> " + exePath.parent_path().string());
    return exePath.parent_path().string();

#elif defined(__APPLE__)
    uint32_t size = PATH_MAX;
    char result[PATH_MAX];

    if (_NSGetExecutablePath(result, &size) != 0) {
        DEBUG_ERR("Cannot determine executable path (_NSGetExecutablePath failed)");
    }

    std::filesystem::path exePath(result);
    DEBUG_LOG("get_exe_path() return -> " + exePath.parent_path().string());
    return exePath.parent_path().string();

#elif defined(_WIN32)
    std::wstring wpath(MAX_PATH, L'\0');
    DWORD length = GetModuleFileNameW(NULL, wpath.data(), static_cast<DWORD>(wpath.size()));

    if (length == 0) {
        DEBUG_ERR("Cannot determine executable path (GetModuleFileNameW failed)");
    }

    wpath.resize(length);
    std::filesystem::path exePath(wpath);
    DEBUG_LOG("get_exe_path() return -> " + exePath.parent_path().string());
    return exePath.parent_path().string();

#else
    DEBUG_ERR("platform not supported");
#endif
}