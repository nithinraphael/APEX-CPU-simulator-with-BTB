#pragma once

#include <iostream>
#include <string>

namespace config
{

enum class Opcode : uint8_t
{

    ADD,
    ADDL,
    SUB,
    SUBL,
    MUL,
    MOVC,
    LOAD,
    LOADP,
    STORE,
    STOREP,
    AND,
    OR,
    XOR,
    CML,
    CMP,
    BZ,
    BNZ,
    BP,
    BNP,
    BN,
    BNN,
    JUMP,
    JALR,
    NOP,
    HALT
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
    static const int PROGRAM_COUNTER_START_ADDRESS = 4000;
};

} // namespace config
