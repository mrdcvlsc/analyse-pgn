#ifndef APGN_CHECK_HPP
#define APGN_CHECK_HPP

#include <iostream>
#include <filesystem>
#include <string>

namespace apgn_check
{
    bool validNumber(const std::string& str)
    {
        std::string::const_iterator i = str.begin();
        while (i != str.end() && std::isdigit(*i)) ++i;
        return !str.empty() && i == str.end();
    }

    void errorFileNotFound(const std::string& filename)
    {
        std::FILE *filereader = std::fopen(filename.c_str(), "rb");
        if(!filereader)
        {
            throw std::invalid_argument(
                "The given file '"+filename+"' was not found"
            );
        }
    }

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

    /// checks the subject_string if it's last substring is equal to the given patter_string
    /// @param 2
    /// @return true if equal, false if not
    bool match_last(const std::string& subject_string, const std::string& pattern_string)
    {
        size_t slen = subject_string.size(), plen = pattern_string.size();

        if(slen<plen) return false;
        
        if(subject_string.substr(slen-plen,plen)==pattern_string)
            return true;

        return false;
    }

    std::string removeFormat(const std::string& filename, const std::string& format)
    {
        return filename.substr(0,filename.size()-format.size());
    }

    std::string removeFilename(const std::string& filename)
    {
        for(size_t i=1; i<filename.size(); ++i)
        {
            if(filename[filename.size()-i]=='\\' || filename[filename.size()-i]=='/')
            {
                return filename.substr(0,filename.size()-i);
            }
        }
        return "";
    }
}

#endif