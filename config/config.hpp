#pragma once

#include <iostream>
#include <string>

namespace config
{

enum class Opcode : uint8_t
{
    ADD = 0x0,
    SUB = 0x1,
    MUL = 0x2,
    DIV = 0x3,
    AND = 0x4,
    OR = 0x5,
    XOR = 0x6,
    MOVC = 0x7,
    LOAD = 0x8,
    STORE = 0x9,
    BZ = 0xa,
    BNZ = 0xb,
    HALT = 0xc
};

class Debugging
{
  public:
    static const bool ENABLE_DEBUG_MESSAGES = true;
    /* Set this flag to true to enable cycle single-step mode */
    static const bool ENABLE_SINGLE_STEP = true;
};

class CPU
{
  public:
    static const int DATA_MEMORY_SIZE = 4096;
    static const int REG_FILE_SIZE = 16;
    static const int DEFAULT_REGISTER_VALUE = 0;
    // 32 bit register -> 4 Byte
    static const int REGISTER_SIZE_BITS = 32;
    static const int TOTAL_REGISTERS = 16;
    static const int TOTAL_MEMORY = 0xFFFFFFFF; // 4 Gigs
};

} // namespace config
