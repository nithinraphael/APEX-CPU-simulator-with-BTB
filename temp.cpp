#include "config/config.hpp"
#include "core/globals.hpp"
#include "core/index.hpp"
#include "core/memory.hpp"
#include "core/register.hpp"
#include "core/register_file.hpp"
#include "errors/index.hpp"
#include "fs/index.hpp"
#include "instruction/all_instructions.hpp"
#include "instruction/index.hpp"
#include "instruction/instruction.hpp"
#include "parser/index.hpp"
#include "parser/parser.hpp"
#include "utils/index.hpp"
#include "utils/utils.hpp"

#include <_types/_uint32_t.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <ostream>
#include <queue>
#include <vector>

using namespace fs;
using namespace std;
using namespace utils;
using namespace instruction;
using namespace errors;
using namespace core;

const bool IS_FORWARDING = false;

template <typename T> void resetQueue(std::queue<T>& q)
{
    std::queue<T> empty;
    std::swap(q, empty);
}

void printQueue(const std::queue<Instruction>& q)
{
    std::queue<Instruction> tempQueue = q; // Create a copy of the queue

    std::cout << "Queue contents: ";
    while (!tempQueue.empty())
    {
        printOpcode(tempQueue.front().opcode);
        cout << endl;
        tempQueue.pop();
    }
    std::cout << std::endl;
}

struct InstructionRes
{
    Instruction Instruction;
    bool isPlaceholder;
    int Dest;
    int Value;
};

struct RegisterInfo
{
    string source;
    int value;
};

struct MemoryInfo
{
    int source;
    int value;
};

struct ExResult
{
    bool hasUpdate;
    Instruction inst;
    optional<RegisterInfo> regInfo = nullopt;
    optional<MemoryInfo> memoryInfo = nullopt;
    // BAD BAD CODE
    optional<RegisterInfo> regInfo2 = nullopt;
    optional<MemoryInfo> memoryInfo2 = nullopt;
};

struct InstructionInfo
{
    int address;
    // BAD CODE
    Instruction instruction;
};

struct InstructionInfoExResult
{
    int address;
    // BAD CODE
    ExResult exResult;
};

template <typename T> std::queue<T> copyQueue(const std::queue<T>& original)
{
    std::queue<T> copiedQueue = original;
    return copiedQueue;
}

// int pc = 0;
// int cycle = 0;
// bool stopFetch = false;

void printQueue2(const std::queue<ExResult>& q)
{
    std::queue<ExResult> tempQueue = q; // Create a copy of the queue

    std::cout << "Queue contents: ";
    while (!tempQueue.empty())
    {
        printOpcode(tempQueue.front().inst.opcode);
        cout << endl;
        tempQueue.pop();
    }
    std::cout << std::endl;
}

bool shouldStallHelper(const Instruction& first, const Instruction& second)
{

    // Check if operand1 of the first instruction is not null and
    // if it matches operand2 or operand3 of the second instruction.

    // cout << "--------------" << endl;
    // printOpcode(first.opcode);
    // cout << endl;
    // printOpcode(second.opcode);
    // cout << endl;
    // cout << "--------------" << endl;

    // Parked instructions-> CMP, BNZ

    if (first.opcode == config::Opcode::MOVC)
    {
        if (second.opcode == config::Opcode::ADD || second.opcode == config::Opcode::SUB)
        {
            if (first.operand1.value() == second.operand2.value() || first.operand1.value() == second.operand3.value())
            {
                return true;
            }
        }
        else if (second.opcode == config::Opcode::MOVC)
        {
            return false;
        }
        else if (second.opcode == config::Opcode::STORE || second.opcode == config::Opcode::ADDL ||
                 second.opcode == config::Opcode::STOREP)
        {

            if (first.operand1.value() == second.operand1.value() || first.operand1.value() == second.operand2.value())
            {
                return true;
            }
        }
        else if (second.opcode == config::Opcode::JALR)
        {
            // see operand 2
            if (first.operand1.value() == second.operand2.value())
            {
                return true;
            }
        }

        return false;
    }
    else if (first.opcode == config::Opcode::SUB)
    {
        if (second.opcode == config::Opcode::CMP)
        {
            if (first.operand1.value() == second.operand1.value() || first.operand1.value() == second.operand2.value())
            {
                return true;
            }
        }

        if (second.opcode == config::Opcode::ADD)
        {
            if (first.operand1.value() == second.operand2.value() || first.operand1.value() == second.operand3.value())
            {
                return true;
            }
        }
    }
    else if (first.opcode == config::Opcode::ADDL)
    {
        if (second.opcode == config::Opcode::SUB)
        {
            if (first.operand1.value() == second.operand2.value() || first.operand1.value() == second.operand3.value())
            {
                return true;
            }
        }

        if (second.opcode == config::Opcode::LOADP)
        {
            if (first.operand1.value() == second.operand1.value() || first.operand1.value() == second.operand2.value())
            {
                return true;
            }
        }

        if (second.opcode == Opcode::CMP)
        {
            if (first.operand1 == second.operand1 || first.operand1 == second.operand2)
            {
                return true;
            }
        }
    }
    else if (first.opcode == config::Opcode::STOREP)
    {
        if (second.opcode == config::Opcode::LOADP)
        {
            if (first.operand1.value() == second.operand2.value() ||
                first.operand1.value() == second.operand2.value() ||
                first.operand2.value() == second.operand2.value() || first.operand2.value() == second.operand2.value())
            {
                return true;
            }
        }

        if (second.opcode == config::Opcode::ADDL)
        {

            if (first.operand1.value() == second.operand2.value() || first.operand2.value() == second.operand2.value())
            {
                return true;
            }
        }
    }
    else if (first.opcode == config::Opcode::LOADP)
    {
        if (second.opcode == config::Opcode::XOR)
        {
            if (first.operand1.value() == second.operand2.value() ||
                first.operand1.value() == second.operand2.value() ||
                first.operand2.value() == second.operand2.value() || first.operand2.value() == second.operand2.value())
            {
                return true;
            }
        }

        if (second.opcode == config::Opcode::STOREP)
        {
            if (first.operand1.value() == second.operand2.value() ||
                first.operand1.value() == second.operand2.value() ||
                first.operand2.value() == second.operand2.value() || first.operand2.value() == second.operand2.value())
            {
                return true;
            }
        }
    }
    else if (first.opcode == config::Opcode::XOR)
    {
        if (second.opcode == config::Opcode::STOREP)
        {
            cout << "***************" << endl;
            if (first.operand1.value() == second.operand1.value() || first.operand1.value() == second.operand2.value())
            {
                return true;
            }
        }
    }

    // If none of the above conditions match, there is no stall.
    return false;
}

bool shouldStall(queue<InstructionInfo> fetchQueue, queue<InstructionInfo> dRFQueue,
                 queue<InstructionInfoExResult> exQueue)
{
    // Check if there are any instructions in dRFQueue that would cause stalls.
    if (!fetchQueue.empty() && !dRFQueue.empty())
    {
        auto first = dRFQueue.front();
        auto second = fetchQueue.front();

        if (shouldStallHelper(first.instruction, second.instruction))
        {
            return true;
        }
    }

    if (!fetchQueue.empty() && !exQueue.empty())
    {

        auto first = exQueue.front();
        auto second = fetchQueue.front();

        if (shouldStallHelper(first.exResult.inst, second.instruction))
        {
            return true; // Stall due to data hazard.
        }
    }

    // No stalls detected.
    return false;
}

bool shouldStallFETCH(Instruction second, queue<Instruction> dRFQueue)
{
    // Check if there are any instructions in dRFQueue that would cause stalls.
    if (!dRFQueue.empty())
    {
        const Instruction& first = dRFQueue.front();

        if (shouldStallHelper(first, second))
        {
            // printOpcode(first.opcode);
            // printOpcode(second.opcode);
            // cout << "11111111111111111" << endl;
            return true; // Stall due to data hazard.
        }
    }

    // No stalls detected.
    return false;
}

int main(int argc, char* argv[])
{

    int numSimulations = 100;

    Result<string, Error> filenameRes = getFileNameCMD(argc, argv);
    if (filenameRes.isError())
    {
        filenameRes.getError().print();
        exit(1);
    }

    FileUtil file(filenameRes.getValue());

    Result<vector<string>, Error> fileRes = file.readLines();

    if (fileRes.isError())
    {
        fileRes.getError().print();
        exit(1);
    }

    Result<vector<Instruction>, Error> instructionListRes = getInstructionSequence(fileRes.getValue());
    if (instructionListRes.isError())
    {
        instructionListRes.getError().print();
        exit(1);
    }

    InstructionSequence& iSeq = InstructionSequence::instance();
    iSeq.init(instructionListRes.getValue());

    RegisterFile& regFile = RegisterFile::instance();
    RegisterFileFake& regFileFake = RegisterFileFake::instance();
    Memory& memory = Memory::instance();

    iSeq.printSeq(true);
    auto instructionSq = iSeq.getSequence();

    int pc = 0;
    int cycle = 0;
    bool stopFetch = false;
    queue<InstructionInfo> fetchQueue;
    queue<InstructionInfo> dRFQueue;
    queue<InstructionInfoExResult> exQueue;
    queue<InstructionInfoExResult> memQueue;
    queue<InstructionInfoExResult> wbQueue;
    int P = 0;
    int N = 0;
    int Z = 0;
    bool stall = false;

    // while (stopFetch == false || !wbQueue.empty())
    while (cycle != numSimulations)
    {
        cout << "\n---------- Cycle " << cycle + 1 << " ----------" << endl;

        // cout << "------- QUEUE contents"
        //      << "---------" << endl;
        // cout << "F" << endl;
        // printQueue(fetchQueue);
        // cout << endl;
        // cout << "D/RF" << endl;
        // printQueue(dRFQueue);
        // cout << endl;

        // cout << "EX" << endl;
        // printQueue2(exQueue);
        // cout << endl;

        // cout << "MEM" << endl;
        // printQueue2(memQueue);
        // cout << endl;

        // cout << "WB" << endl;
        // printQueue2(wbQueue);
        // cout << endl;

        // FETCH
        if (!stopFetch)
        {

            cout << "++++++++++++++++++" << endl;
            if (pc < instructionSq.size())
            {
                auto inst = instructionSq[pc];
                auto fakeFetchQueue = copyQueue(fetchQueue);
                fakeFetchQueue.push(InstructionInfo{pc, inst});
                bool mustStall = shouldStall(fakeFetchQueue, dRFQueue, exQueue);

                if (IS_FORWARDING)
                {
                    mustStall = false;
                }

                // cout << "FETCH STALL:  " << mustStall << endl;
                if (!mustStall)
                {
                    fetchQueue.push(InstructionInfo{pc, inst});
                    pc++;
                    cout << "Fetch: " << getInstructionString(inst) << endl;
                }
                else
                {
                    cout << "Fetch: " << getInstructionString(inst) << endl;
                }
            }
        }

        // DECODE
        if (cycle > 0 && !fetchQueue.empty())
        {
            stall = shouldStall(fetchQueue, dRFQueue, exQueue);
            // cout << "DECODE STALL:  " << stall << endl;

            if (IS_FORWARDING)
            {
                stall = false;
            }

            auto insDRF = fetchQueue.front();
            if (!stall)
            {
                if (insDRF.instruction.opcode == config::Opcode::HALT)
                {
                    stopFetch = true;
                    // resetQueue(dRFQueue);
                    // resetQueue(fetchQueue);
                }
                dRFQueue.push(insDRF);
                cout << "Decode/RF: " << getInstructionString(insDRF.instruction) << endl;
                fetchQueue.pop();
            }
            else
            {
                auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                dRFQueue.push(inst);
                cout << "Decode/RF (Stalling): " << getInstructionString(insDRF.instruction) << endl;
            }
        }

        // EXECUTE
        if (IS_FORWARDING)
        {
            if (cycle > 1 && !dRFQueue.empty())
            {
                auto ins = dRFQueue.front();
                dRFQueue.pop();

                if (ins.instruction.opcode == config::Opcode::NOP || ins.instruction.opcode == config::Opcode::HALT)
                {
                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::MOVC)
                {
                    auto reg = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto val = getNumberFromLiteral(ins.instruction.operand2.value()).getValue();
                    exQueue.push({ins.address, ExResult{true, ins.instruction,
                                                        RegisterInfo{ins.instruction.operand1.value(), val}}});
                }
                else if (ins.instruction.opcode == config::Opcode::ADD)
                {
                    printInstruction(ins.instruction);

                    auto regDest = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto regSrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto regSrc2 = regFileFake.getRegFromString(ins.instruction.operand3.value());

                    int result = regSrc1.value()->get() + regSrc2.value()->get();

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }
                    cout << "444444444444" << regSrc1.value()->get() << "jjjjj" << regSrc2.value()->get() << endl;

                    exQueue.push({ins.address, ExResult{true, ins.instruction,
                                                        RegisterInfo{ins.instruction.operand1.value(), result}}});
                }
                else if (ins.instruction.opcode == config::Opcode::SUB)
                {

                    auto regDest = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto regSrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto regSrc2 = regFileFake.getRegFromString(ins.instruction.operand3.value());

                    int result = regSrc1.value()->get() - regSrc2.value()->get();

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push({ins.address, ExResult{true, ins.instruction,
                                                        RegisterInfo{ins.instruction.operand1.value(), result}}});
                }
                else if (ins.instruction.opcode == config::Opcode::STORE)
                {

                    auto rsrc1 = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc2 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int memoryAddress = rsrc2.value()->get() + literal;

                    int value = rsrc1.value()->get();

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction, nullopt, MemoryInfo{memoryAddress, value}}});
                }
                else if (ins.instruction.opcode == config::Opcode::STOREP)
                {

                    auto rsrc1 = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc2 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int memoryAddress = rsrc2.value()->get() + literal;

                    int value = rsrc1.value()->get();

                    exQueue.push(
                        {ins.address, ExResult{
                                          true,
                                          ins.instruction,
                                          RegisterInfo{ins.instruction.operand2.value(), rsrc2.value()->get() + 4},
                                          MemoryInfo{memoryAddress, value},
                                      }});
                }
                else if (ins.instruction.opcode == config::Opcode::ADDL)
                {

                    auto regSrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto num = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int result = regSrc1.value()->get() + num;

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction,
                                               RegisterInfo{ins.instruction.operand1.value(), result}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::SUBL)
                {

                    auto regSrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto num = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int result = regSrc1.value()->get() - num;

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction,
                                               RegisterInfo{ins.instruction.operand1.value(), result}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::XOR)
                {

                    auto regSrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto regSrc2 = regFileFake.getRegFromString(ins.instruction.operand3.value());

                    auto num = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int result = regSrc1.value()->get() ^ regSrc2.value()->get();

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction,
                                               RegisterInfo{ins.instruction.operand1.value(), result}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::LOAD)
                {

                    auto rDest = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    Result<uint8_t, Error> result =
                        memory.read(rsrc1.value()->get() + literal); // Store the result in a variable

                    exQueue.push(
                        {ins.address,
                         ExResult{true, ins.instruction,
                                  RegisterInfo{ins.instruction.operand1.value(), result.getValue()}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::LOADP)
                {

                    auto rDest = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    Result<uint8_t, Error> result =
                        memory.read(rsrc1.value()->get() + literal); // Store the result in a variable

                    exQueue.push({ins.address,
                                  ExResult{true, ins.instruction,
                                           RegisterInfo{ins.instruction.operand1.value(), result.getValue()}, nullopt,
                                           RegisterInfo{ins.instruction.operand2.value(), rsrc1.value()->get() + 4}}});
                }
                else if (ins.instruction.opcode == config::Opcode::JALR)
                {
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    auto rsrc1 = regFileFake.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    auto value = rsrc1.value()->get() + literal;

                    // pc -2 gives the address of instruction after JALR because we already flushed 2 inst..
                    cout << "PC  -2------- JALR  " << pc - 2 << "   " << ins.address << endl;
                    stopFetch = false;

                    exQueue.push({inst.address, ExResult{true, ins.instruction,
                                                         RegisterInfo{ins.instruction.operand1.value(),
                                                                      codeMemoryIndexToPC(ins.address + 1)},
                                                         nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::JUMP)
                {
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    auto rsrc1 = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand2.value()).getValue();

                    auto value = rsrc1.value()->get() + literal;

                    cout << "VVVVVJHGHJ JKHJ ILK 0L  " << value << "  ooooooooooo" << endl;
                    pc = pcToCodeMemoryIndex(value);
                    cout << "VVVVVJHGHJ JKHJ ILK JL  " << pc << "  ooooooooooo" << endl;

                    exQueue.push({inst.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::CMP)
                {
                    auto R1 = regFileFake.getRegFromString(ins.instruction.operand1.value());
                    auto R2 = regFileFake.getRegFromString(ins.instruction.operand2.value());

                    int result = R1.value()->get() - R2.value()->get();

                    cout << "RESULTTTTTTTTT  " << result << "  " << R1.value()->get() << " &&&   " << R2.value()->get()
                         << endl;

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }

                    if (result < 0)
                    {
                        N = 1;
                    }

                    if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::BNZ)
                {

                    if (Z == 0)
                    {
                        // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                        auto num = getNumberFromLiteral(ins.instruction.operand1.value()).getValue();
                        num = getRelativeNumber(num);

                        resetQueue(fetchQueue);
                        resetQueue(dRFQueue);
                        auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                        dRFQueue.push(inst);
                        fetchQueue.push(inst);

                        // printQueue(dRFQueue);
                        // printQueue(exQueue);
                        stopFetch = false;

                        cout << "nu,, " << num << "--- pc" << pc << endl;
                        // pc = (pc + num) - 2;
                        pc = (ins.address + num);
                        cout << "nu,, " << num << "--- pc" << pc << endl;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::BNP)
                {

                    if (P == 0)
                    {

                        auto num = getNumberFromLiteral(ins.instruction.operand1.value()).getValue();
                        // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                        num = getRelativeNumber(num);
                        resetQueue(fetchQueue);
                        resetQueue(dRFQueue);
                        auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                        dRFQueue.push(inst);
                        fetchQueue.push(inst);

                        cout << "nu,, " << num << "--- pc BNP" << pc << endl;
                        pc = (ins.address + num);
                        cout << "nu,, " << num << "--- pc" << pc << endl;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::BP)
                {

                    if (P == 1)
                    {

                        auto num = getNumberFromLiteral(ins.instruction.operand1.value()).getValue();
                        // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                        num = getRelativeNumber(num);
                        resetQueue(fetchQueue);
                        resetQueue(dRFQueue);
                        auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                        dRFQueue.push(inst);
                        fetchQueue.push(inst);

                        cout << "nu,, " << num << "--- pc" << pc << endl;
                        pc = (ins.address + num);
                        cout << "nu,, " << num << "--- pc" << pc << endl;

                        stopFetch = false;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }

                cout << "Execute: " << getInstructionString(ins.instruction) << endl;

                // forwarding hack
                if (IS_FORWARDING)
                {
                    auto fakeExecQueue = copyQueue(exQueue);

                    auto insFake = fakeExecQueue.back();
                    printInstruction(insFake.exResult.inst);
                    if (insFake.exResult.hasUpdate)
                    {
                        if (insFake.exResult.regInfo.has_value())
                        {
                            regFileFake.setRegFromString(insFake.exResult.regInfo->source,
                                                         insFake.exResult.regInfo->value);
                        }

                        if (insFake.exResult.regInfo2.has_value())
                        {
                            regFileFake.setRegFromString(insFake.exResult.regInfo2->source,
                                                         insFake.exResult.regInfo2->value);
                        }
                    }
                    // regFileFake.print();
                }
            }
        }
        else
        {
            if (cycle > 1 && !dRFQueue.empty())
            {
                auto ins = dRFQueue.front();
                dRFQueue.pop();

                if (ins.instruction.opcode == config::Opcode::NOP || ins.instruction.opcode == config::Opcode::HALT)
                {
                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::MOVC)
                {
                    auto reg = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto val = getNumberFromLiteral(ins.instruction.operand2.value()).getValue();
                    exQueue.push({ins.address, ExResult{true, ins.instruction,
                                                        RegisterInfo{ins.instruction.operand1.value(), val}}});
                }
                else if (ins.instruction.opcode == config::Opcode::ADD)
                {
                    printInstruction(ins.instruction);

                    auto regDest = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto regSrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto regSrc2 = regFile.getRegFromString(ins.instruction.operand3.value());

                    int result = regSrc1.value()->get() + regSrc2.value()->get();

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }
                    cout << "444444444444" << regSrc1.value()->get() << "jjjjj" << regSrc2.value()->get() << endl;

                    exQueue.push({ins.address, ExResult{true, ins.instruction,
                                                        RegisterInfo{ins.instruction.operand1.value(), result}}});
                }
                else if (ins.instruction.opcode == config::Opcode::SUB)
                {

                    auto regDest = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto regSrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto regSrc2 = regFile.getRegFromString(ins.instruction.operand3.value());

                    int result = regSrc1.value()->get() - regSrc2.value()->get();

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push({ins.address, ExResult{true, ins.instruction,
                                                        RegisterInfo{ins.instruction.operand1.value(), result}}});
                }
                else if (ins.instruction.opcode == config::Opcode::STORE)
                {

                    auto rsrc1 = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc2 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int memoryAddress = rsrc2.value()->get() + literal;

                    int value = rsrc1.value()->get();

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction, nullopt, MemoryInfo{memoryAddress, value}}});
                }
                else if (ins.instruction.opcode == config::Opcode::STOREP)
                {

                    auto rsrc1 = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc2 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int memoryAddress = rsrc2.value()->get() + literal;

                    int value = rsrc1.value()->get();

                    exQueue.push(
                        {ins.address, ExResult{
                                          true,
                                          ins.instruction,
                                          RegisterInfo{ins.instruction.operand2.value(), rsrc2.value()->get() + 4},
                                          MemoryInfo{memoryAddress, value},
                                      }});
                }
                else if (ins.instruction.opcode == config::Opcode::ADDL)
                {

                    auto regSrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto num = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int result = regSrc1.value()->get() + num;

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction,
                                               RegisterInfo{ins.instruction.operand1.value(), result}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::SUBL)
                {

                    auto regSrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto num = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int result = regSrc1.value()->get() - num;

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction,
                                               RegisterInfo{ins.instruction.operand1.value(), result}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::XOR)
                {

                    auto regSrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto regSrc2 = regFile.getRegFromString(ins.instruction.operand3.value());

                    auto num = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    int result = regSrc1.value()->get() ^ regSrc2.value()->get();

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }
                    else if (result < 0)
                    {
                        N = 1;
                    }
                    else if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push(
                        {ins.address, ExResult{true, ins.instruction,
                                               RegisterInfo{ins.instruction.operand1.value(), result}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::LOAD)
                {

                    auto rDest = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    Result<uint8_t, Error> result =
                        memory.read(rsrc1.value()->get() + literal); // Store the result in a variable

                    exQueue.push(
                        {ins.address,
                         ExResult{true, ins.instruction,
                                  RegisterInfo{ins.instruction.operand1.value(), result.getValue()}, nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::LOADP)
                {

                    auto rDest = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto rsrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    Result<uint8_t, Error> result =
                        memory.read(rsrc1.value()->get() + literal); // Store the result in a variable

                    exQueue.push({ins.address,
                                  ExResult{true, ins.instruction,
                                           RegisterInfo{ins.instruction.operand1.value(), result.getValue()}, nullopt,
                                           RegisterInfo{ins.instruction.operand2.value(), rsrc1.value()->get() + 4}}});
                }
                else if (ins.instruction.opcode == config::Opcode::JALR)
                {
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    auto rsrc1 = regFile.getRegFromString(ins.instruction.operand2.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand3.value()).getValue();

                    auto value = rsrc1.value()->get() + literal;

                    // pc -2 gives the address of instruction after JALR because we already flushed 2 inst..
                    cout << "PC  -2------- JALR  " << pc - 2 << "   " << ins.address << endl;
                    stopFetch = false;

                    exQueue.push({inst.address, ExResult{true, ins.instruction,
                                                         RegisterInfo{ins.instruction.operand1.value(),
                                                                      codeMemoryIndexToPC(ins.address + 1)},
                                                         nullopt}});
                }
                else if (ins.instruction.opcode == config::Opcode::JUMP)
                {
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    auto rsrc1 = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto literal = getNumberFromLiteral(ins.instruction.operand2.value()).getValue();

                    auto value = rsrc1.value()->get() + literal;

                    cout << "VVVVVJHGHJ JKHJ ILK 0L  " << value << "  ooooooooooo" << endl;
                    pc = pcToCodeMemoryIndex(value);
                    cout << "VVVVVJHGHJ JKHJ ILK JL  " << pc << "  ooooooooooo" << endl;

                    exQueue.push({inst.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::CMP)
                {
                    auto R1 = regFile.getRegFromString(ins.instruction.operand1.value());
                    auto R2 = regFile.getRegFromString(ins.instruction.operand2.value());

                    int result = R1.value()->get() - R2.value()->get();

                    cout << "RESULTTTTTTTTT  " << result << "  " << R1.value()->get() << " &&&   " << R2.value()->get()
                         << endl;

                    P = 0;
                    N = 0;
                    Z = 0;

                    if (result > 0)
                    {
                        P = 1;
                    }

                    if (result < 0)
                    {
                        N = 1;
                    }

                    if (result == 0)
                    {
                        Z = 1;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::BNZ)
                {

                    if (Z == 0)
                    {
                        // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                        auto num = getNumberFromLiteral(ins.instruction.operand1.value()).getValue();
                        num = getRelativeNumber(num);

                        resetQueue(fetchQueue);
                        resetQueue(dRFQueue);
                        auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                        dRFQueue.push(inst);
                        fetchQueue.push(inst);

                        // printQueue(dRFQueue);
                        // printQueue(exQueue);
                        stopFetch = false;

                        cout << "nu,, " << num << "--- pc" << pc << endl;
                        // pc = (pc + num) - 2;
                        pc = (ins.address + num);
                        cout << "nu,, " << num << "--- pc" << pc << endl;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::BNP)
                {

                    if (P == 0)
                    {

                        auto num = getNumberFromLiteral(ins.instruction.operand1.value()).getValue();
                        // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                        num = getRelativeNumber(num);
                        resetQueue(fetchQueue);
                        resetQueue(dRFQueue);
                        auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                        dRFQueue.push(inst);
                        fetchQueue.push(inst);

                        cout << "nu,, " << num << "--- pc BNP" << pc << endl;
                        pc = (ins.address + num);
                        cout << "nu,, " << num << "--- pc" << pc << endl;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }
                else if (ins.instruction.opcode == config::Opcode::BP)
                {

                    if (P == 1)
                    {

                        auto num = getNumberFromLiteral(ins.instruction.operand1.value()).getValue();
                        // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                        num = getRelativeNumber(num);
                        resetQueue(fetchQueue);
                        resetQueue(dRFQueue);
                        auto inst = InstructionInfo{99, Instruction{config::Opcode::NOP}};
                        dRFQueue.push(inst);
                        fetchQueue.push(inst);

                        cout << "nu,, " << num << "--- pc" << pc << endl;
                        pc = (ins.address + num);
                        cout << "nu,, " << num << "--- pc" << pc << endl;

                        stopFetch = false;
                    }

                    exQueue.push({ins.address, ExResult{false, ins.instruction}});
                }

                cout << "Execute: " << getInstructionString(ins.instruction) << endl;
            }
        }

        // MEMORY
        if (cycle > 2 && !exQueue.empty())
        {

            auto ins = exQueue.front();

            if (ins.exResult.hasUpdate)
            {
                if (ins.exResult.memoryInfo.has_value())
                {
                    cout << "88888888888" << ins.exResult.memoryInfo->source << ins.exResult.memoryInfo->value << endl;
                    memory.write(ins.exResult.memoryInfo->source, ins.exResult.memoryInfo->value);
                }
                if (ins.exResult.memoryInfo2.has_value())
                {
                    cout << "88888888888" << ins.exResult.memoryInfo2->source << ins.exResult.memoryInfo2->value
                         << endl;
                    memory.write(ins.exResult.memoryInfo2->source, ins.exResult.memoryInfo2->value);
                }
            }
            memQueue.push(ins);
            exQueue.pop();
            cout << "Memory: " << getInstructionString(ins.exResult.inst) << endl;
        }

        // WRITEBACK
        if (cycle > 3 && !memQueue.empty())
        {
            auto ins = memQueue.front();
            if (ins.exResult.hasUpdate)
            {
                if (ins.exResult.regInfo.has_value())
                {
                    regFile.setRegFromString(ins.exResult.regInfo->source, ins.exResult.regInfo->value);
                }

                if (ins.exResult.regInfo2.has_value())
                {
                    regFile.setRegFromString(ins.exResult.regInfo2->source, ins.exResult.regInfo2->value);
                }
            }
            memQueue.pop();

            cout << "Writeback: " << getInstructionString(ins.exResult.inst) << endl;
        }
        regFile.print();

        cout << "P: " << P << " N: " << N << " Z: " << Z << " PC: " << pc << endl;

        Result<uint8_t, Error> result = memory.read(1004); // Store the result in a variable
        if (result.isOk())
        {
            cout << "MEM READ: " << static_cast<int>(result.getValue()) << endl;
        }
        Result<uint8_t, Error> result2 = memory.read(1008); // Store the result in a variable
        if (result.isOk())
        {
            cout << "MEM READ: " << static_cast<int>(result2.getValue()) << endl;
        }

        Result<uint8_t, Error> result3 = memory.read(1012); // Store the result in a variable
        if (result.isOk())
        {
            cout << "MEM READ: " << static_cast<int>(result3.getValue()) << endl;
        }

        cin.get();

        cycle++;
    }

    return 0;
}
