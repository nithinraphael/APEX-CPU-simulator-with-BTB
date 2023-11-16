#pragma once

#include "../config/index.hpp"
#include "../errors/index.hpp"
#include <__algorithm/remove_if.h>
#include <algorithm>
#include <cctype>
#include <locale>
#include <map>
#include <queue>
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
// replaces multiple commas with one comma
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

inline void printStringVector(const vector<string>& vec)
{
    for (const string& str : vec)
    {
        cout << str << " "
             << "\n";
    }
    cout << endl;
}

inline template <typename T> void resetQueue(std::queue<T>& q)
{
    std::queue<T> empty;
    std::swap(q, empty);
}

inline template <typename KeyType, typename ValueType>
bool keyExistsInMap(const std::map<KeyType, ValueType>& mapToTest, const KeyType& keyToCheck)
{
    return mapToTest.find(keyToCheck) != mapToTest.end();
}

template <typename T> std::queue<T> copyQueue(const std::queue<T>& originalQueue)
{
    std::queue<T> copiedQueue;

    // Deep copy the original queue
    std::queue<T> tempQueue = originalQueue;
    while (!tempQueue.empty())
    {
        // Get the front element of the original queue
        T frontElement = tempQueue.front();

        // Pop the element from the temporary queue
        tempQueue.pop();

        // Copy the element and push it onto the new queue
        copiedQueue.push(frontElement);
    }

    return copiedQueue;
}

} // namespace utils