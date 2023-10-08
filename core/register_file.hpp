#pragma once
#include "../config/index.hpp"
#include "register.hpp"
#include <iostream>
#include <optional>

using namespace config;
using namespace std;

namespace core
{
class RegisterFile
{
  public:
    static RegisterFile& instance()
    {
        static RegisterFile instance;
        return instance;
    }

    RegisterFile(const RegisterFile&) = delete;
    RegisterFile& operator=(const RegisterFile&) = delete;

    optional<Register*> get(int index)
    {
        if (index >= 0 && index < config::CPU::TOTAL_REGISTERS)
        {
            return registers[index].get();
        }
        return nullopt;
    }

    optional<Register*> getRegFromString(const std::string& regStr)
    {

        auto registerIndex = Register::getIndexFromString(regStr);
        if (registerIndex.isOk())
        {
            int index = registerIndex.getValue();
            return get(index);
        }
        return nullopt;
    }

    // TODO use result
    void setRegFromString(const std::string& regStr, int value)
    {

        auto registerIndex = Register::getIndexFromString(regStr);
        if (registerIndex.isOk())
        {
            int index = registerIndex.getValue();
            auto regRes = get(index);

            if (regRes.has_value())
            {
                regRes.value()->set(value);
            }
            else
            {
                cout << "Some register error" << endl;
            }
        }
    }

    void print()
    {
        cout << "............... Register File ..............." << endl;
        for (int i = 0; i < config::CPU::TOTAL_REGISTERS; i++)
        {
            if (auto registerPtr = get(i))
            {
                std::cout << "R" << i << " [" << (*registerPtr)->get() << "]  | ";
                if (i == (config::CPU::TOTAL_REGISTERS - 1) / 2)
                {
                    cout << "\n";
                }
            }
        }
        cout << endl;
        cout << "............................................." << endl;
    }

  private:
    RegisterFile()
    {
        for (int i = 0; i < config::CPU::TOTAL_REGISTERS; i++)
        {
            registers[i] = make_unique<Register>(i);
        }
    }

    unique_ptr<Register> registers[config::CPU::TOTAL_REGISTERS];
};
} // namespace core