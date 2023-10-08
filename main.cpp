#include "config/config.hpp"
#include "core/globals.hpp"
#include "core/index.hpp"
#include "core/memory.hpp"
#include "core/register.hpp"
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

template <typename T> std::queue<T> copyQueue(const std::queue<T>& original)
{
    std::queue<T> copiedQueue = original; // Create a copy using the copy constructor
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

bool shouldStall(queue<Instruction> fetchQueue, queue<Instruction> dRFQueue, queue<ExResult> exQueue)
{
    // Check if there are any instructions in dRFQueue that would cause stalls.
    if (!fetchQueue.empty() && !dRFQueue.empty())
    {
        auto first = dRFQueue.front();
        const Instruction& second = fetchQueue.front();
        // cout << "88888888" << endl;
        // printInstruction(first);
        // printInstruction(second);
        if (shouldStallHelper(first, second))
        {
            // printOpcode(first.opcode);
            // printOpcode(second.opcode);
            // cout << "11111111111111111" << endl;
            return true; // Stall due to data hazard.
        }
    }

    if (!fetchQueue.empty() && !exQueue.empty())
    {

        const ExResult& first = exQueue.front();
        const Instruction& second = fetchQueue.front();
        // cout << "999999999" << endl;

        if (shouldStallHelper(first.inst, second))
        {
            // cout << "222222222222222222" << endl;
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
    Memory& memory = Memory::instance();

    iSeq.printSeq(true);
    auto instructionSq = iSeq.getSequence();

    int pc = 0;
    int cycle = 0;
    bool stopFetch = false;
    queue<Instruction> fetchQueue;
    queue<Instruction> dRFQueue;
    queue<ExResult> exQueue;
    queue<ExResult> memQueue;
    queue<ExResult> wbQueue;
    int P = 0;
    int N = 0;
    int Z = 0;
    bool stall = false;

    // while (stopFetch == false || !wbQueue.empty())
    while (cycle != 40)
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
                fakeFetchQueue.push(inst);
                bool mustStall = shouldStall(fakeFetchQueue, dRFQueue, exQueue);

                // cout << "FETCH STALL:  " << mustStall << endl;
                if (!mustStall)
                {
                    fetchQueue.push(inst);
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

            auto insDRF = fetchQueue.front();
            if (!stall)
            {
                if (insDRF.opcode == config::Opcode::HALT)
                {
                    stopFetch = true;
                    // resetQueue(dRFQueue);
                    // resetQueue(fetchQueue);
                }
                dRFQueue.push(insDRF);
                cout << "Decode/RF: " << getInstructionString(insDRF) << endl;
                fetchQueue.pop();
            }
            else
            {
                auto inst = Instruction{config::Opcode::NOP};
                dRFQueue.push(inst);
                cout << "Decode/RF (Stalling): " << getInstructionString(insDRF) << endl;
            }
        }

        // EXECUTE
        if (cycle > 1 && !dRFQueue.empty())
        {
            auto ins = dRFQueue.front();
            dRFQueue.pop();

            if (ins.opcode == config::Opcode::NOP || ins.opcode == config::Opcode::HALT)
            {
                exQueue.push(ExResult{false, ins});
            }
            else if (ins.opcode == config::Opcode::MOVC)
            {
                auto reg = regFile.getRegFromString(ins.operand1.value());
                auto val = getNumberFromLiteral(ins.operand2.value()).getValue();
                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), val}});
            }
            else if (ins.opcode == config::Opcode::ADD)
            {

                auto regDest = regFile.getRegFromString(ins.operand1.value());
                auto regSrc1 = regFile.getRegFromString(ins.operand2.value());
                auto regSrc2 = regFile.getRegFromString(ins.operand3.value());

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
                cout << "444444444444" << endl;

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result}});
            }
            else if (ins.opcode == config::Opcode::SUB)
            {

                auto regDest = regFile.getRegFromString(ins.operand1.value());
                auto regSrc1 = regFile.getRegFromString(ins.operand2.value());
                auto regSrc2 = regFile.getRegFromString(ins.operand3.value());

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

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result}});
            }
            else if (ins.opcode == config::Opcode::STORE)
            {

                auto rsrc1 = regFile.getRegFromString(ins.operand1.value());
                auto rsrc2 = regFile.getRegFromString(ins.operand2.value());
                auto literal = getNumberFromLiteral(ins.operand3.value()).getValue();

                int memoryAddress = rsrc2.value()->get() + literal;

                int value = rsrc1.value()->get();

                exQueue.push(ExResult{true, ins, nullopt, MemoryInfo{memoryAddress, value}});
            }
            else if (ins.opcode == config::Opcode::STOREP)
            {

                auto rsrc1 = regFile.getRegFromString(ins.operand1.value());
                auto rsrc2 = regFile.getRegFromString(ins.operand2.value());
                auto literal = getNumberFromLiteral(ins.operand3.value()).getValue();

                int memoryAddress = rsrc2.value()->get() + literal;

                int value = rsrc1.value()->get();

                exQueue.push(ExResult{
                    true,
                    ins,
                    RegisterInfo{ins.operand2.value(), rsrc2.value()->get() + 4},
                    MemoryInfo{memoryAddress, value},
                });
            }
            else if (ins.opcode == config::Opcode::ADDL)
            {

                auto regSrc1 = regFile.getRegFromString(ins.operand2.value());
                auto num = getNumberFromLiteral(ins.operand3.value()).getValue();

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

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result}, nullopt});
            }
            else if (ins.opcode == config::Opcode::SUBL)
            {

                auto regSrc1 = regFile.getRegFromString(ins.operand2.value());
                auto num = getNumberFromLiteral(ins.operand3.value()).getValue();

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

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result}, nullopt});
            }
            else if (ins.opcode == config::Opcode::XOR)
            {

                auto regSrc1 = regFile.getRegFromString(ins.operand2.value());
                auto regSrc2 = regFile.getRegFromString(ins.operand3.value());

                auto num = getNumberFromLiteral(ins.operand3.value()).getValue();

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

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result}, nullopt});
            }
            else if (ins.opcode == config::Opcode::LOAD)
            {

                auto rDest = regFile.getRegFromString(ins.operand1.value());
                auto rsrc1 = regFile.getRegFromString(ins.operand2.value());
                auto literal = getNumberFromLiteral(ins.operand3.value()).getValue();

                Result<uint8_t, Error> result =
                    memory.read(rsrc1.value()->get() + literal); // Store the result in a variable

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result.getValue()}, nullopt});
            }
            else if (ins.opcode == config::Opcode::LOADP)
            {

                auto rDest = regFile.getRegFromString(ins.operand1.value());
                auto rsrc1 = regFile.getRegFromString(ins.operand2.value());
                auto literal = getNumberFromLiteral(ins.operand3.value()).getValue();

                Result<uint8_t, Error> result =
                    memory.read(rsrc1.value()->get() + literal); // Store the result in a variable

                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), result.getValue()}, nullopt,
                                      RegisterInfo{ins.operand2.value(), rsrc1.value()->get() + 4}});
            }
            else if (ins.opcode == config::Opcode::JALR)
            {
                resetQueue(fetchQueue);
                resetQueue(dRFQueue);
                auto inst = Instruction{config::Opcode::NOP};
                dRFQueue.push(inst);
                fetchQueue.push(inst);

                auto rsrc1 = regFile.getRegFromString(ins.operand2.value());
                auto literal = getNumberFromLiteral(ins.operand3.value()).getValue();

                auto value = rsrc1.value()->get() + literal;

                // pc -2 gives the address of instruction after JALR because we already flushed 2 inst..
                exQueue.push(ExResult{true, ins, RegisterInfo{ins.operand1.value(), (pc - 2)}, nullopt});
            }
            else if (ins.opcode == config::Opcode::JUMP)
            {
                resetQueue(fetchQueue);
                resetQueue(dRFQueue);
                auto inst = Instruction{config::Opcode::NOP};
                dRFQueue.push(inst);
                fetchQueue.push(inst);

                auto rsrc1 = regFile.getRegFromString(ins.operand1.value());
                auto literal = getNumberFromLiteral(ins.operand2.value()).getValue();

                auto value = rsrc1.value()->get() + literal;

                pc = value;

                exQueue.push(ExResult{false, ins});
            }
            else if (ins.opcode == config::Opcode::CMP)
            {
                auto R1 = regFile.getRegFromString(ins.operand1.value());
                auto R2 = regFile.getRegFromString(ins.operand2.value());

                int result = R1.value()->get() - R2.value()->get();

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

                exQueue.push(ExResult{false, ins});
            }
            else if (ins.opcode == config::Opcode::BNZ)
            {

                if (Z == 0)
                {
                    // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                    auto num = getNumberFromLiteral(ins.operand1.value()).getValue();
                    num = getRelativeNumber(num);

                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = Instruction{config::Opcode::NOP};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    // printQueue(dRFQueue);
                    // printQueue(exQueue);

                    cout << "nu,, " << num << "--- pc" << pc << endl;
                    pc = (pc + num) - 2 - 1;
                    cout << "nu,, " << num << "--- pc" << pc << endl;
                }

                exQueue.push(ExResult{false, ins});
            }
            else if (ins.opcode == config::Opcode::BNP)
            {

                if (P == 0)
                {

                    auto num = getNumberFromLiteral(ins.operand1.value()).getValue();
                    // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                    num = getRelativeNumber(num);
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = Instruction{config::Opcode::NOP};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    cout << "nu,, " << num << "--- pc" << pc << endl;
                    pc = (pc + num) - 2 - 1;
                    cout << "nu,, " << num << "--- pc" << pc << endl;
                }

                exQueue.push(ExResult{false, ins});
            }
            else if (ins.opcode == config::Opcode::BP)
            {

                if (P == 1)
                {

                    auto num = getNumberFromLiteral(ins.operand1.value()).getValue();
                    // auto num = getRelativeNumber(getNumberFromLiteral(ins.operand1.value()).getValue());
                    num = getRelativeNumber(num);
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto inst = Instruction{config::Opcode::NOP};
                    dRFQueue.push(inst);
                    fetchQueue.push(inst);

                    cout << "nu,, " << num << "--- pc" << pc << endl;
                    pc = (pc + num) - 2;
                    cout << "nu,, " << num << "--- pc" << pc << endl;

                    stopFetch = false;
                }

                exQueue.push(ExResult{false, ins});
            }

            cout << "Execute: " << getInstructionString(ins) << endl;
        }

        // MEMORY
        if (cycle > 2 && !exQueue.empty())
        {

            auto ins = exQueue.front();

            if (ins.hasUpdate)
            {
                if (ins.memoryInfo.has_value())
                {
                    cout << "88888888888" << ins.memoryInfo->source << ins.memoryInfo->value << endl;
                    memory.write(ins.memoryInfo->source, ins.memoryInfo->value);
                }
                if (ins.memoryInfo2.has_value())
                {
                    cout << "88888888888" << ins.memoryInfo2->source << ins.memoryInfo2->value << endl;
                    memory.write(ins.memoryInfo2->source, ins.memoryInfo2->value);
                }
            }
            memQueue.push(ins);
            exQueue.pop();
            cout << "Memory: " << getInstructionString(ins.inst) << endl;
        }

        // WRITEBACK
        if (cycle > 3 && !memQueue.empty())
        {
            auto ins = memQueue.front();
            if (ins.hasUpdate)
            {
                if (ins.regInfo.has_value())
                {
                    regFile.setRegFromString(ins.regInfo->source, ins.regInfo->value);
                }

                if (ins.regInfo2.has_value())
                {
                    regFile.setRegFromString(ins.regInfo2->source, ins.regInfo2->value);
                }
            }
            memQueue.pop();

            cout << "Writeback: " << getInstructionString(ins.inst) << endl;
        }
        regFile.print();

        cout << "P: " << P << " N: " << N << " Z: " << Z << " PC: " << pc - 1 << endl;

        // Result<uint8_t, Error> result = memory.read(12); // Store the result in a variable
        // if (result.isOk())
        // {
        //     cout << "MEM READ: " << static_cast<int>(result.getValue()) << endl;
        // }
        cin.get();

        cycle++;
    }

    return 0;
}
