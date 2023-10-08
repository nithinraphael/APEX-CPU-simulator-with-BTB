#pragma once

#include "../config/index.hpp"
#include "../errors/index.hpp"
#include "../utils/index.hpp"
#include <_types/_uint16_t.h>
#include <_types/_uint8_t.h>
#include <bitset>
#include <cstdint>
#include <format>
#include <iostream>
#include <string>

using namespace std;
using namespace utils;

namespace core
{
class Register
{
  public:
    explicit Register(uint8_t index) : value(config::CPU::DEFAULT_REGISTER_VALUE), index(index)
    {
    }

    static Result<Register, Error> fromString(const string& regString)
    {
        string str = toUpperCase(removeSpaces(regString));

        if (str.empty() || str[0] != 'R')
        {
            return Result<Register, Error>(Error("Invalid register string"));
        }

        try
        {
            int index = stoi(str.substr(1));
            return Result<Register, Error>(Register(index));
        }
        catch (const invalid_argument& e)
        {
            return Result<Register, Error>(Error("Invalid register string"));
        }
    }

    static Result<int, Error> getIndexFromString(const string& regString)
    {
        string str = toUpperCase(removeSpaces(regString));

        if (str.empty() || str[0] != 'R')
        {
            return Result<int, Error>(Error("Invalid register string"));
        }

        try
        {
            int index = stoi(str.substr(1));
            return Result<int, Error>(index);
        }
        catch (const invalid_argument& e)
        {
            return Result<int, Error>(Error("Invalid register string"));
        }
    }

    void set(int val)
    {
        value = val;
    }

    const int get() const
    {
        return value;
    }

    void add(const Register& reg)
    {
        value = value + reg.get();
    }

    void sub(const Register& reg)
    {
        value = value - reg.get();
    }

    bitset<config::CPU::REGISTER_SIZE_BITS> getBin() const
    {
        return intToBitset(value);
    }

    void print() const
    {
        cout << "Register name: " << getRegisterName() << endl;
        printBin();
        printDec();
    }

    void printBin() const
    {
        cout << "Register Value Bin: " << getBin() << endl;
    }

    void printDec() const
    {
        cout << "Register Value Dec: " << value << endl;
    }

    string getRegisterName() const
    {
        return "R" + to_string(index);
    }

  private:
    int value;
    uint8_t index;
};
} // namespace core