#ifndef APGN_PATH_HPP
#define APGN_PATH_HPP

#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>

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
        // windows version of getting the executable path, not the working directory
        #endif
    }
}
#endif