#pragma once

#include "../errors/index.hpp"
#include <regex>
#include <sstream>
#include <vector>

using namespace errors;
using namespace std;

namespace utils
{

inline Result<string, Error> getFileNameCMD(int argc, char* argv[])
{
    if (argc != 2)
    {
        return Result<string, Error>(Error("invalid arguments given"));
    }

    return Result<string, Error>(argv[1]);
}

inline string replaceCommasWithSpaces(const string& str)
{
    string newStr = str;
    replace(newStr.begin(), newStr.end(), ',', ' ');
    return newStr;
}

} // namespace utils