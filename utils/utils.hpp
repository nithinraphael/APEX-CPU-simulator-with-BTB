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

inline vector<string> splitString(const string& input, char splitter)
{
    vector<string> words;
    istringstream strStream(input);
    string str;

    while (getline(strStream, str, splitter))
    {
        words.push_back(str);
    }

    return words;
}

inline string replaceSpacesWith(const string& input, const string& with)
{
    regex pattern("\\s+");

    return regex_replace(input, pattern, with);
}

inline string replaceCommasWith(const string& input, const string& with)
{
    regex pattern(",+");
    return regex_replace(input, pattern, with);
}

} // namespace utils