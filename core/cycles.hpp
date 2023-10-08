#pragma once

#include "../config/index.hpp"
#include "../errors/index.hpp"
#include "../instruction/index.hpp"

using namespace errors;
using namespace instruction;

namespace core
{

inline void runPipeline()
{
    while (true)
    {
    }
}

inline Result<NoError, Error> fetch(Instruction inst)
{
    auto opcode = inst.opcode;

    if (opcode == config::Opcode::ADD)
    {
        cout << "ADD" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::ADDL)
    {
        cout << "ADDL" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::SUB)
    {
        cout << "SUB" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::SUBL)
    {
        cout << "SUBL" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::MUL)
    {
        cout << "MUL" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::MOVC)
    {
        cout << "MOVC" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::LOAD)
    {
        cout << "LOAD" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::LOADP)
    {
        cout << "LOADP" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::STORE)
    {
        cout << "STORE" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::STOREP)
    {
        cout << "STOREP" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::AND)
    {
        cout << "AND" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::OR)
    {
        cout << "OR" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::XOR)
    {
        cout << "XOR" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::CML)
    {
        cout << "CML" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::CMP)
    {
        cout << "CMP" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BZ)
    {
        cout << "BZ" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BNZ)
    {
        cout << "BNZ" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BP)
    {
        cout << "BP" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BNP)
    {
        cout << "BNP" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BN)
    {
        cout << "BN" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BNN)
    {
        cout << "BNN" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::JUMP)
    {
        cout << "JUMP" << endl;
        return Result<NoError, Error>(NoError());
    }
    if (opcode == config::Opcode::JALR)
    {
        cout << "JALR" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::NOP)
    {
        cout << "NOP" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::HALT)
    {
        cout << "HALT" << endl;
        return Result<NoError, Error>(NoError());
    }

    return Result<NoError, Error>(Error("Instruction not found"));
};

} // namespace core
