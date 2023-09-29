#include "errors/index.hpp"
#include "fs/index.hpp"
#include "utils/index.hpp"

#include <iostream>

using namespace fs;
using namespace std;
using namespace utils;

int main(int argc, char* argv[])
{
    Result<string, Error> filenameRes = getFileNameCMD(argc, argv);
    if (filenameRes.isError())
    {
        Error err = filenameRes.getError();
        err.print();
        exit(1);
    }

    FileUtil file(filenameRes.getValue());

    Result<vector<string>, Error> res = file.readLines();

    if (res.isOk())
    {
        for (const std::string& str : res.getValue())
        {
            std::cout << str << std::endl;
        }
    }

    return 0;
}