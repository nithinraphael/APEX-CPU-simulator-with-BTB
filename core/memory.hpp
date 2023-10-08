#pragma once
#include "../config/index.hpp"
#include "../errors/index.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace errors;

namespace core
{
class Memory
{
  public:
    static Memory& instance()
    {
        static Memory instance;
        return instance;
    }

    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    Result<NoError, Error> write(uint32_t address, uint8_t data)
    {
        if (address > 4096)
        {
            return Result<NoError, Error>(Error("Memory out of bounds"));
        }

        memory->insert(memory->begin() + address, data);
        return Result<NoError, Error>(NoError());
    }

    Result<uint8_t, Error> read(uint32_t address)
    {
        if (address > 4096)
        {
            return Result<uint8_t, Error>(Error("Memory out of bounds"));
        }

        return Result<uint8_t, Error>((*memory)[address]);
    }

  private:
    Memory()
    {
        memory = make_unique<vector<uint8_t>>(4096);
    }

    unique_ptr<vector<uint8_t>> memory;
};
} // namespace core