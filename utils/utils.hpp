#pragma once

#include "../errors/index.hpp"

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
} // namespace utils