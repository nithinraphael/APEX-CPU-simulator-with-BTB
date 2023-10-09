#pragma once

#include "../config/index.hpp"
#include "../errors/index.hpp"
#include <__algorithm/remove_if.h>
#include <algorithm>
#include <cctype>
#include <locale>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace errors;
using namespace std;

namespace utils
{

inline Result<string, Error> getFileNameCMD(int argc, char* argv[])
{
    if (argc < 1)
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

inline string removeSpaces(const string& str)
{
    string s = str;
    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
    return s;
}

inline string toUpperCase(const string& s)
{
    string str = s;

    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

inline Result<int, Error> getNumberFromLiteral(const string& s)
{
    string str = removeSpaces(s);

    try
    {
        int num = stoi(str.substr(1));
        return Result<int, Error>(num);
    }
    catch (const invalid_argument& e)
    {
        return Result<int, Error>(Error("Invalid Literal string"));
    }
}

// Program Counter to Code Memory Index 4000 -> 0
inline int pcToCodeMemoryIndex(int pc)
{
    return (pc - config::CPU::PROGRAM_COUNTER_START_ADDRESS) / 4;
}

inline int getRelativeNumber(int num)
{
    return num / 4;
}

// Code Memory Index to Program Counter 0 -> 4000
inline int codeMemoryIndexToPC(int memoryIndex)
{
    return memoryIndex * 4 + config::CPU::PROGRAM_COUNTER_START_ADDRESS;
}

} // namespace utils