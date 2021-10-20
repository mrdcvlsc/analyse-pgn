#ifndef APGN_PATH_HPP
#define APGN_PATH_HPP

#include <iostream>
#include <limits.h>

#if defined(__linux__)
#include <libgen.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <locale>
#include <codecvt>
#endif

namespace apgn
{
    std::string get_execpath()
    {
        #if defined(__linux__)
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        const char *path;
        if (count != -1) {
            path = dirname(result);
        }
        return path;
        #elif defined(_WIN32)
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);

        std::wstring string_to_convert = path;

        //setup converter
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;

        //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
        std::string converted_str = converter.to_bytes( string_to_convert );
        
        return converted_str.substr(0,converted_str.size()-9);
        #endif
    }
}
#endif