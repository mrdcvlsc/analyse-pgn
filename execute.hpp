#ifndef APGN_EXECUTE_HPP
#define APGN_EXECUTE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

namespace progams
{
    int get_pid()
    {
        return (int) getpid();
    }
}

#endif