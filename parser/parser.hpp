#pragma once

#include "../config/index.hpp"
#include "../errors/index.hpp"
#include "../utils/index.hpp"

using namespace errors;
using namespace utils;

namespace parser
{

inline Result<config::Opcode, Error> parseOpcode(const string& instruction)
{
    string instModified = toUpperCase(removeSpaces(instruction));

    if (instModified == "ADD")
    {
        return Result<config::Opcode, Error>(config::Opcode::ADD);
    }

    if (instModified == "SUB")
    {
        return Result<config::Opcode, Error>(config::Opcode::SUB);
    }

    if (instModified == "MUL")
    {
        return Result<config::Opcode, Error>(config::Opcode::MUL);
    }

    if (instModified == "DIV")
    {
        return Result<config::Opcode, Error>(config::Opcode::DIV);
    }

    if (instModified == "AND")
    {
        return Result<config::Opcode, Error>(config::Opcode::AND);
    }

    if (instModified == "OR")
    {
        return Result<config::Opcode, Error>(config::Opcode::OR);
    }

    if (instModified == "XOR")
    {
        return Result<config::Opcode, Error>(config::Opcode::XOR);
    }

    if (instModified == "MOVC")
    {
        return Result<config::Opcode, Error>(config::Opcode::MOVC);
    }

    if (instModified == "LOAD")
    {
        return Result<config::Opcode, Error>(config::Opcode::LOAD);
    }

    if (instModified == "STORE")
    {
        return Result<config::Opcode, Error>(config::Opcode::STORE);
    }

    if (instModified == "BZ")
    {
        return Result<config::Opcode, Error>(config::Opcode::BZ);
    }

    if (instModified == "BNZ")
    {
        return Result<config::Opcode, Error>(config::Opcode::BNZ);
    }

    if (instModified == "HALT")
    {
        return Result<config::Opcode, Error>(config::Opcode::HALT);
    }

    return Result<config::Opcode, Error>(Error("Instruction not found"));
};

inline Result<NoError, Error> printOpcode(config::Opcode opcode)
{

    if (opcode == config::Opcode::ADD)
    {
        cout << "ADD" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::SUB)
    {
        cout << "SUB" << endl;

        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::MUL)
    {
        cout << "MUL" << endl;
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::DIV)
    {
        cout << "DIV" << endl;

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

    if (opcode == config::Opcode::STORE)
    {
        cout << "STORE" << endl;

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

    if (opcode == config::Opcode::HALT)
    {
        cout << "HALT" << endl;

        return Result<NoError, Error>(NoError());
    }

    return Result<NoError, Error>(Error("Instruction not found"));
};

} // namespace parser