#pragma once

#include "../config/index.hpp"
#include "../errors/index.hpp"
#include "../utils/index.hpp"
#include <iostream>
#include <string>

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

    if (instModified == "ADDL")
    {
        return Result<config::Opcode, Error>(config::Opcode::ADDL);
    }

    if (instModified == "SUB")
    {
        return Result<config::Opcode, Error>(config::Opcode::SUB);
    }

    if (instModified == "SUBL")
    {
        return Result<config::Opcode, Error>(config::Opcode::SUBL);
    }

    if (instModified == "MUL")
    {
        return Result<config::Opcode, Error>(config::Opcode::MUL);
    }

    if (instModified == "MOVC")
    {
        return Result<config::Opcode, Error>(config::Opcode::MOVC);
    }

    if (instModified == "LOAD")
    {
        return Result<config::Opcode, Error>(config::Opcode::LOAD);
    }

    if (instModified == "LOADP")
    {
        return Result<config::Opcode, Error>(config::Opcode::LOADP);
    }

    if (instModified == "STORE")
    {
        return Result<config::Opcode, Error>(config::Opcode::STORE);
    }

    if (instModified == "STOREP")
    {
        return Result<config::Opcode, Error>(config::Opcode::STOREP);
    }

    if (instModified == "AND")
    {
        return Result<config::Opcode, Error>(config::Opcode::AND);
    }

    if (instModified == "OR")
    {
        return Result<config::Opcode, Error>(config::Opcode::OR);
    }

    if (instModified == "EX-OR")
    {
        return Result<config::Opcode, Error>(config::Opcode::XOR);
    }

    if (instModified == "CML")
    {
        return Result<config::Opcode, Error>(config::Opcode::CML);
    }

    if (instModified == "CMP")
    {
        return Result<config::Opcode, Error>(config::Opcode::CMP);
    }

    if (instModified == "BZ")
    {
        return Result<config::Opcode, Error>(config::Opcode::BZ);
    }

    if (instModified == "BNZ")
    {
        return Result<config::Opcode, Error>(config::Opcode::BNZ);
    }

    if (instModified == "BP")
    {
        return Result<config::Opcode, Error>(config::Opcode::BP);
    }

    if (instModified == "BNP")
    {
        return Result<config::Opcode, Error>(config::Opcode::BNP);
    }

    if (instModified == "BN")
    {
        return Result<config::Opcode, Error>(config::Opcode::BN);
    }

    if (instModified == "BNN")
    {
        return Result<config::Opcode, Error>(config::Opcode::BNN);
    }

    if (instModified == "JUMP")
    {
        return Result<config::Opcode, Error>(config::Opcode::JUMP);
    }
    if (instModified == "JALR")
    {
        return Result<config::Opcode, Error>(config::Opcode::JALR);
    }

    if (instModified == "NOP")
    {
        return Result<config::Opcode, Error>(config::Opcode::NOP);
    }

    if (instModified == "HALT")
    {
        return Result<config::Opcode, Error>(config::Opcode::HALT);
    }

    return Result<config::Opcode, Error>(Error("Instruction not found"));
};

inline Result<NoError, Error> printOpcode(config::Opcode opcode, bool newLine = true)
{

    if (opcode == config::Opcode::ADD)
    {
        cout << "ADD";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::ADDL)
    {
        cout << "ADDL";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::SUB)
    {
        cout << "SUB";

        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::SUBL)
    {
        cout << "SUBL";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::MUL)
    {
        cout << "MUL";

        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::MOVC)
    {
        cout << "MOVC";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::LOAD)
    {
        cout << "LOAD";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::LOADP)
    {
        cout << "LOADP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::STORE)
    {
        cout << "STORE";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::STOREP)
    {
        cout << "STOREP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::AND)
    {
        cout << "AND";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::OR)
    {
        cout << "OR";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::XOR)
    {
        cout << "EX-OR";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::CML)
    {
        cout << "CML";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::CMP)
    {
        cout << "CMP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BZ)
    {
        cout << "BZ";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BNZ)
    {
        cout << "BNZ";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BP)
    {
        cout << "BP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BNP)
    {
        cout << "BNP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BN)
    {
        cout << "BN";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::BNN)
    {
        cout << "BNN";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::JUMP)
    {
        cout << "JUMP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }
    if (opcode == config::Opcode::JALR)
    {
        cout << "JALR";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::NOP)
    {
        cout << "NOP";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    if (opcode == config::Opcode::HALT)
    {
        cout << "HALT";
        if (newLine)
        {
            cout << endl;
        }
        return Result<NoError, Error>(NoError());
    }

    return Result<NoError, Error>(Error("Instruction not found"));
};

inline string getOpcodeStr(config::Opcode opcode)
{

    if (opcode == config::Opcode::ADD)
    {
        return "ADD";
    }

    if (opcode == config::Opcode::ADDL)
    {
        return "ADDL";
    }

    if (opcode == config::Opcode::SUB)
    {
        return "SUB";
    }

    if (opcode == config::Opcode::SUBL)
    {
        return "SUBL";
    }

    if (opcode == config::Opcode::MUL)
    {
        return "MUL";
    }

    if (opcode == config::Opcode::MOVC)
    {
        return "MOVC";
    }

    if (opcode == config::Opcode::LOAD)
    {
        return "LOAD";
    }

    if (opcode == config::Opcode::LOADP)
    {
        return "LOADP";
    }

    if (opcode == config::Opcode::STORE)
    {
        return "STORE";
    }

    if (opcode == config::Opcode::STOREP)
    {
        return "STOREP";
    }

    if (opcode == config::Opcode::AND)
    {
        return "AND";
    }

    if (opcode == config::Opcode::OR)
    {
        return "OR";
    }

    if (opcode == config::Opcode::XOR)
    {
        return "EX-OR";
    }

    if (opcode == config::Opcode::CML)
    {
        return "CML";
    }

    if (opcode == config::Opcode::CMP)
    {
        return "CMP";
    }

    if (opcode == config::Opcode::BZ)
    {
        return "BZ";
    }

    if (opcode == config::Opcode::BNZ)
    {
        return "BNZ";
    }

    if (opcode == config::Opcode::BP)
    {
        return "BP";
    }

    if (opcode == config::Opcode::BNP)
    {
        return "BNP";
    }

    if (opcode == config::Opcode::BN)
    {
        return "BN";
    }

    if (opcode == config::Opcode::BNN)
    {
        return "BNN";
    }

    if (opcode == config::Opcode::JUMP)
    {
        return "JUMP";
    }
    if (opcode == config::Opcode::JALR)
    {
        return "JALR";
    }

    if (opcode == config::Opcode::NOP)
    {
        return "NOP";
    }

    if (opcode == config::Opcode::HALT)
    {
        return "HALT";
    }

    return "BAD_OPCODE";
};

} // namespace parser