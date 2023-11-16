#pragma once

#include "../config/index.hpp"
#include "../core/index.hpp"
#include "../errors/index.hpp"
#include "../parser/index.hpp"
#include "../utils/index.hpp"
#include <_types/_uint32_t.h>
#include <_types/_uint8_t.h>
#include <iostream>
#include <map>
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
    string rawInstruction;
    string opcode;
    optional<string> operand1;
    optional<string> operand2;
    optional<string> operand3;
};

struct Instruction
{
    string rawInstruction;
    int address;
    config::Opcode opcode;
    optional<string> destination;
    optional<string> source1;
    optional<string> source2;
    optional<int> literal;
};

struct PCInstruction
{
    uint32_t PC;
    Instruction instruction;
};

inline void printInstruction(const Instruction& instruction)
{
    cout << "ADDRESS: " << instruction.address << endl;
    cout << "RAW: " << instruction.rawInstruction << endl;
    cout << "Opcode: " << parser::getOpcodeStr(instruction.opcode) << endl;

    if (instruction.destination)
    {
        cout << "  Destination: " << instruction.destination.value() << endl;
    }
    if (instruction.source1)
    {
        cout << "  Source1: " << instruction.source1.value() << endl;
    }
    if (instruction.source2)
    {
        cout << "  Source2: " << instruction.source2.value() << endl;
    }
    if (instruction.literal)
    {
        cout << "  Literal: " << instruction.literal.value() << endl;
    }
    cout << "---------------------" << endl;
}

inline void printInstructions(const std::vector<Instruction>& instructions)
{
    for (const auto& instr : instructions)
    {
        printInstruction(instr);
    }
}

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
        return Result<InstructionStr, Error>(InstructionStr{.opcode = instructionVec[0],
                                                            .operand1 = instructionVec[1],
                                                            .operand2 = instructionVec[2],
                                                            .operand3 = instructionVec[3],
                                                            .rawInstruction = instruction});
    }

    if (instructionVec.size() == 3)
    {
        return Result<InstructionStr, Error>(InstructionStr{

            .opcode = instructionVec[0],
            .operand1 = instructionVec[1],
            .operand2 = instructionVec[2],
            .rawInstruction = instruction});
    }

    if (instructionVec.size() == 2)
    {
        return Result<InstructionStr, Error>(
            InstructionStr{.opcode = instructionVec[0], .operand1 = instructionVec[1], .rawInstruction = instruction});
    }

    return Result<InstructionStr, Error>(InstructionStr{.opcode = instructionVec[0], .rawInstruction = instruction});
}

inline vector<InstructionStr> getInstructionStrSqFromLines(const vector<string> lines)
{
    vector<InstructionStr> result;

    for (auto& line : lines)
    {
        auto instructionStr = parseInstructionLine(line);

        result.push_back(instructionStr.getValue());
    }

    return result;
}

inline void printInstructionStr(const InstructionStr instStr)
{
    cout << "RAW: " << instStr.rawInstruction << "\n";

    cout << "OPCODE: " << instStr.opcode << " ";

    if (instStr.operand1.has_value())
    {
        cout << "OP-1: " << instStr.operand1.value() << " ";
    }

    if (instStr.operand2.has_value())
    {
        cout << "OP-2: " << instStr.operand2.value() << " ";
    }

    if (instStr.operand3.has_value())
    {
        cout << "OP-3: " << instStr.operand3.value() << " ";
    }

    cout << endl;
}

inline void printInstructionStrSeq(const vector<InstructionStr> instructionStrSeq)
{
    for (const auto& instructionStr : instructionStrSeq)
    {
        printInstructionStr(instructionStr);
    }
}

inline tuple<map<int, Instruction>, vector<Instruction>> instructionSeqFromInstStrSeq(
    const int addressStartFrom, const vector<InstructionStr> instructionStrSeq)
{
    vector<Instruction> instructionList;
    int startAddress = addressStartFrom;
    map<int, Instruction> instructionMap;

    for (const auto& instructionStr : instructionStrSeq)
    {
        auto opcode = parser::parseOpcode(instructionStr.opcode).getValue();
        auto rawInstruction = instructionStr.rawInstruction;
        Instruction inst;

        if (opcode == config::Opcode::ADD || opcode == config::Opcode::SUB || opcode == config::Opcode::MUL ||
            opcode == config::Opcode::AND || opcode == config::Opcode::OR || opcode == config::Opcode::XOR)
        {
            inst = Instruction{.destination = instructionStr.operand1,
                               .source1 = instructionStr.operand2,
                               .source2 = instructionStr.operand3};
        }

        if (opcode == config::Opcode::STORE || opcode == config::Opcode::STOREP)
        {
            inst = Instruction{.source1 = instructionStr.operand1,
                               .source2 = instructionStr.operand2,
                               .literal = getNumberFromLiteral(instructionStr.operand3.value()).getValue()};
        };

        if (opcode == config::Opcode::LOAD || opcode == config::Opcode::LOADP)
        {
            inst = Instruction{.destination = instructionStr.operand1,
                               .source1 = instructionStr.operand2,
                               .literal = getNumberFromLiteral(instructionStr.operand3.value()).getValue()};
        };

        if (opcode == config::Opcode::CML || opcode == config::Opcode::JUMP)
        {
            inst = Instruction{.source1 = instructionStr.operand1,
                               .literal = getNumberFromLiteral(instructionStr.operand2.value()).getValue()};
        }

        if (opcode == config::Opcode::MOVC)
        {
            inst = Instruction{.destination = instructionStr.operand1,
                               .literal = getNumberFromLiteral(instructionStr.operand2.value()).getValue()};
        }

        if (opcode == config::Opcode::CMP)
        {
            inst = Instruction{.source1 = instructionStr.operand1, .source2 = instructionStr.operand2};
        };

        if (opcode == config::Opcode::BZ || opcode == config::Opcode::BNZ || opcode == config::Opcode::BP ||
            opcode == config::Opcode::BNP || opcode == config::Opcode::BN || opcode == config::Opcode::BNN)
        {
            inst = Instruction{.literal = getNumberFromLiteral(instructionStr.operand1.value()).getValue()};
        }

        if (opcode == config::Opcode::JALR)
        {
            inst = Instruction{.destination = instructionStr.operand1,
                               .source1 = instructionStr.operand2,
                               .literal = getNumberFromLiteral(instructionStr.operand3.value()).getValue()};
        }

        if (opcode == config::Opcode::NOP || opcode == config::Opcode::HALT)
        {
            inst = Instruction{};
        }

        if (opcode == config::Opcode::ADDL || opcode == config::Opcode::SUBL)
        {
            inst = Instruction{
                .destination = instructionStr.operand1,
                .source1 = instructionStr.operand2,
                .literal = getNumberFromLiteral(instructionStr.operand3.value()).getValue(),
            };
        }

        inst.address = startAddress;
        inst.opcode = opcode;
        inst.rawInstruction = rawInstruction;

        instructionList.push_back(inst);
        instructionMap[startAddress] = inst;

        startAddress += 4;
    }
    return make_tuple(instructionMap, instructionList);
}

} // namespace instruction

// inline Result<vector<Instruction>, Error> getInstructionSequence(vector<string> instructionStrs)
// {
//     vector<Instruction> instructionList;

//     for (const string& str : instructionStrs)
//     {
//         Result<InstructionStr, Error> parseRes = parseInstructionLine(str);

//         if (parseRes.isError())
//         {
//             return Result<vector<Instruction>, Error>(parseRes.getError());
//         }

//         Result<config::Opcode, Error> instRes = parser::parseOpcode(parseRes.getValue().opcode);

//         if (instRes.isError())
//         {
//             return Result<vector<Instruction>, Error>(instRes.getError());
//         }
//         Instruction ins = Instruction{instRes.getValue(), parseRes.getValue().operand1, parseRes.getValue().operand2,
//                                       parseRes.getValue().operand3};
//         instructionList.push_back(ins);
//     };

//     return Result<vector<Instruction>, Error>(instructionList);
// };

// namespace instruction

// inline void printInstructionList(vector<Instruction> instructionsList, bool debug)
// {
//     cout << "--------------------" << endl;
//     if (debug)
//     {
//         for (size_t index = 0; const auto& inst : instructionsList)
//         {
//             cout << "PC[" << index << "]: ";

//             printOpcode(inst.opcode, false);
//             cout << " ";

//             cout << (inst.operand1.has_value() ? inst.operand1.value() : "") << " "
//                  << (inst.operand2.has_value() ? inst.operand2.value() : "") << " "
//                  << (inst.operand3.has_value() ? inst.operand3.value() : "") << endl;

//             index++;
//         }
//     }
//     else
//     {
//         for (size_t index = 0; const auto& inst : instructionsList)
//         {
//             cout << "Program counter: (" << codeMemoryIndexToPC(index) << ") ";

//             printOpcode(inst.opcode, false);
//             cout << " ";

//             cout << (inst.operand1.has_value() ? inst.operand1.value() : "") << " "
//                  << (inst.operand2.has_value() ? inst.operand2.value() : "") << " "
//                  << (inst.operand3.has_value() ? inst.operand3.value() : "") << endl;

//             if (inst.operand1.has_value())
//             {
//                 cout << "Operand1: " << inst.operand1.value() << " ";
//             }

//             if (inst.operand2.has_value())
//             {
//                 cout << "Operand2: " << inst.operand2.value() << " ";
//             }

//             if (inst.operand3.has_value())
//             {
//                 cout << "Operand3: " << inst.operand3.value() << " ";
//             }
//             cout << endl;

//             index++;
//         }
//     }
//     cout << "--------------------" << endl;
// }

// inline void printInstruction(Instruction inst, bool newline = true)
// {
//     printOpcode(inst.opcode, false);
//     cout << " ";

//     cout << (inst.operand1.has_value() ? inst.operand1.value() : "") << " "
//          << (inst.operand2.has_value() ? inst.operand2.value() : "") << " "
//          << (inst.operand3.has_value() ? inst.operand3.value() : "");

//     if (newline)
//     {
//         cout << endl;
//     }
// }

// inline string getInstructionString(const Instruction& inst)
// {
//     string instructionString;

//     instructionString += getOpcodeStr(inst.opcode) + " ";

//     if (inst.operand1.has_value())
//     {
//         instructionString += inst.operand1.value() + " ";
//     }
//     if (inst.operand2.has_value())
//     {
//         instructionString += inst.operand2.value() + " ";
//     }
//     if (inst.operand3.has_value())
//     {
//         instructionString += inst.operand3.value();
//     }

//     return instructionString;
// }
// namespace instruction
