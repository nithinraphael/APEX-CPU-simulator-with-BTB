#pragma once

#include "../errors/index.hpp"
#include "../utils/index.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace std;
using namespace errors;
using namespace utils;

namespace instruction
{

struct Instruction
{
    string opcode;
    optional<string> operand1;
    optional<string> operand2;
    optional<string> operand3;
};

inline Result<Instruction, Error> parseInstruction(const string& instruction)
{
    string replacedStr = replaceCommasWith(replaceSpacesWith(instruction, ","), ",");

    vector<string> instructionVec = splitString(replacedStr, ',');

    if (instructionVec.empty() || instructionVec.size() > 4)
    {
        return Result<Instruction, Error>(Error("invalid instruction"));
    }

    if (instructionVec.size() == 4)
    {
        return Result<Instruction, Error>(
            Instruction{instructionVec[0], instructionVec[1], instructionVec[2], instructionVec[3]});
    }

    if (instructionVec.size() == 3)
    {
        return Result<Instruction, Error>(Instruction{instructionVec[0], instructionVec[1], instructionVec[2]});
    }

    if (instructionVec.size() == 2)
    {
        return Result<Instruction, Error>(Instruction{instructionVec[0], instructionVec[1]});
    }

    return Result<Instruction, Error>(Instruction{instructionVec[0]});
}

} // namespace instruction
