#pragma once
#include "../config/index.hpp"
#include "register.hpp"
#include <optional>

using namespace config;
using namespace std;

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