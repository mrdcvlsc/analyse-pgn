#include <iostream>
#include "envgrabber.hpp"

int main()
{
    std::vector<std::string> env_variables = apgn_env::grab("env.txt",{
        "CHESS_ENGINE",
        "THREADS"
    });

    for(auto e: env_variables)
        std::cout<<e<<"\n";

    return 0;
}