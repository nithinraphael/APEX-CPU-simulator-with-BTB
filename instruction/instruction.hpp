#pragma once

#include "../config/index.hpp"
#include "../core/index.hpp"
#include "../errors/index.hpp"
#include "../parser/index.hpp"
#include "../utils/index.hpp"
#include <_types/_uint32_t.h>
#include <_types/_uint8_t.h>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

using namespace std;
using namespace errors;
using namespace utils;
using namespace parser;
using namespace core;

namespace instruction
{

struct InstructionStr
{
    string opcode;
    optional<string> operand1;
    optional<string> operand2;
    optional<string> operand3;
};

struct Instruction
{
    config::Opcode opcode;
    optional<string> operand1;
    optional<string> operand2;
    optional<string> operand3;
};

struct PCInstruction
{
    uint32_t PC;
    Instruction instruction;
};

// inline Result<Instruction, Error> parseInstructionStr(const InstructionStr& instructionStr)
// {
//     auto opcode = parseOpcode(instructionStr.opcode);
//     if (instructionStr.operand1)
//     {
//         Register::fromString(instructionStr.operand1.value());
//     }
//     else
//     {
//         // try to get literal

//     }
// }

inline Result<InstructionStr, Error> parseInstructionLine(const string& instruction)
{
    string replacedStr = replaceCommasWith(replaceSpacesWith(instruction, ","), ",");

    vector<string> instructionVec = splitString(replacedStr, ',');

    if (instructionVec.empty() || instructionVec.size() > 4)
    {
        return Result<InstructionStr, Error>(Error("invalid instruction"));
    }

    if (instructionVec.size() == 4)
    {
        return Result<InstructionStr, Error>(
            InstructionStr{instructionVec[0], instructionVec[1], instructionVec[2], instructionVec[3]});
    }

    if (instructionVec.size() == 3)
    {
        return Result<InstructionStr, Error>(InstructionStr{instructionVec[0], instructionVec[1], instructionVec[2]});
    }

    if (instructionVec.size() == 2)
    {
        return Result<InstructionStr, Error>(InstructionStr{instructionVec[0], instructionVec[1]});
    }

    return Result<InstructionStr, Error>(InstructionStr{instructionVec[0]});
}

inline Result<vector<Instruction>, Error> getInstructionSequence(vector<string> instructionStrs)
{
    vector<Instruction> instructionList;

    for (const string& str : instructionStrs)
    {
        Result<InstructionStr, Error> parseRes = parseInstructionLine(str);

        if (parseRes.isError())
        {
            return Result<vector<Instruction>, Error>(parseRes.getError());
        }

        Result<config::Opcode, Error> instRes = parser::parseOpcode(parseRes.getValue().opcode);

        if (instRes.isError())
        {
            return Result<vector<Instruction>, Error>(instRes.getError());
        }
        Instruction ins = Instruction{instRes.getValue(), parseRes.getValue().operand1, parseRes.getValue().operand2,
                                      parseRes.getValue().operand3};
        instructionList.push_back(ins);
    };

    return Result<vector<Instruction>, Error>(instructionList);

}; // namespace instruction

inline void printInstructionList(vector<Instruction> instructionsList, bool debug)
{
    cout << "--------------------" << endl;
    if (debug)
    {
        for (size_t index = 0; const auto& inst : instructionsList)
        {
            cout << "PC[" << index << "]: ";

            printOpcode(inst.opcode, false);
            cout << " ";

            cout << (inst.operand1.has_value() ? inst.operand1.value() : "") << " "
                 << (inst.operand2.has_value() ? inst.operand2.value() : "") << " "
                 << (inst.operand3.has_value() ? inst.operand3.value() : "") << endl;

            index++;
        }
    }
    else
    {
        for (size_t index = 0; const auto& inst : instructionsList)
        {
            cout << "Program counter: (" << codeMemoryIndexToPC(index) << ") ";

            printOpcode(inst.opcode, false);
            cout << " ";

            cout << (inst.operand1.has_value() ? inst.operand1.value() : "") << " "
                 << (inst.operand2.has_value() ? inst.operand2.value() : "") << " "
                 << (inst.operand3.has_value() ? inst.operand3.value() : "") << endl;

            if (inst.operand1.has_value())
            {
                cout << "Operand1: " << inst.operand1.value() << " ";
            }

            if (inst.operand2.has_value())
            {
                cout << "Operand2: " << inst.operand2.value() << " ";
            }

            if (inst.operand3.has_value())
            {
                cout << "Operand3: " << inst.operand3.value() << " ";
            }
            cout << endl;

            index++;
        }
    }
    cout << "--------------------" << endl;
}

inline void printInstruction(Instruction inst, bool newline = true)
{
    printOpcode(inst.opcode, false);
    cout << " ";

    cout << (inst.operand1.has_value() ? inst.operand1.value() : "") << " "
         << (inst.operand2.has_value() ? inst.operand2.value() : "") << " "
         << (inst.operand3.has_value() ? inst.operand3.value() : "");

    if (newline)
    {
        cout << endl;
    }
}

inline string getInstructionString(const Instruction& inst)
{
    string instructionString;

    instructionString += getOpcodeStr(inst.opcode) + " ";

    if (inst.operand1.has_value())
    {
        instructionString += inst.operand1.value() + " ";
    }
    if (inst.operand2.has_value())
    {
        instructionString += inst.operand2.value() + " ";
    }
    if (inst.operand3.has_value())
    {
        instructionString += inst.operand3.value();
    }

    return instructionString;
}

} // namespace instruction
