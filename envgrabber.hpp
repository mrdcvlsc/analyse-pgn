#ifndef APGN_ENVGRABBER_HPP
#define APGN_ENVGRABBER_HPP

#include <iostream>
#include <regex>
#include <fstream>
#include <vector>
#include <exception>

namespace apgn_env
{
    /// returns the values of given variables from an file
    /// @param 1 : .env filename
    /// @param 2 : variables
    /// @param 3 : separator or assignment used in the env file
    std::vector<std::string> grab(const std::string& envfile, const std::vector<std::string>& variables, std::string sep="=")
    {
        std::string filetext;

        std::FILE *filereader = std::fopen(envfile.c_str(), "rb");
        if (filereader)
        {
            std::fseek(filereader, 0, SEEK_END);
            filetext.resize(std::ftell(filereader));
            std::rewind(filereader);
            std::fread(&filetext[0], 1, filetext.size(), filereader);
            std::fclose(filereader);
        }
        else throw std::invalid_argument("env filename provided not found");

        std::vector<std::string> values;

        for(size_t i=0; i<variables.size(); ++i)
        {
            std::regex regexp(variables[i]+sep+"\\S+");
            std::smatch match;

            if(std::regex_search(filetext,match,regexp))
            {
                std::string value = match[0];
                value.erase(0,variables[i].size()+sep.size());
                values.push_back(value);
            }
            else
            {
                throw std::runtime_error(
                    "in envgrabber.hpp->apgn_env::grab()\n"
                    "\tthe given env variable '"+variables[i]+"' does not exist in the '"+envfile+"' file\n"
                );
            }
        }

        return values;
    }

}

#endif