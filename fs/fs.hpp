#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../errors/index.hpp"

using namespace std;
using namespace errors;

namespace fs
{

class FileUtil
{
  public:
    FileUtil(const string& filename) : file(filename)
    {
    }

    ~FileUtil()
    {
        if (file.is_open())
        {
            file.close();
        }
    }

    Result<vector<string>, Error> readLines()
    {
        if (!file.is_open())
        {
            return Result<vector<string>, Error>(Error("File not open"));
        }

        vector<string> lines;

        for (string line; getline(file, line);)
        {
            lines.push_back(line);
        }

        return Result<vector<string>, Error>(lines);
    }

  private:
    fstream file;
};

} // namespace fs
