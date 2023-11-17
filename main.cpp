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
#include <string>
#include <vector>

using namespace fs;
using namespace std;
using namespace utils;
using namespace instruction;
using namespace errors;
using namespace core;

const bool IS_FORWARDING = false;
const int NUM_SIMULATION_CYCLES = 200;
bool HALT_REACHED = false;

template <typename T> void putElementAtFront(std::queue<T>& myQueue, const T& element)
{
    std::queue<T> tempQueue;

    tempQueue.push(element);

    while (!myQueue.empty())
    {
        tempQueue.push(myQueue.front());
        myQueue.pop();
    }

    while (!tempQueue.empty())
    {
        myQueue.push(tempQueue.front());
        tempQueue.pop();
    }
}

void resetAndUpdatePNZ(int& P, int& N, int& Z, int result)
{
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
}

struct MemoryUpdates
{
    int address;
    int value;
};

struct RegisterUpdates
{
    string registerName;
    int value;
};

struct Pipe
{
    Instruction instruction;
    vector<MemoryUpdates> memoryUpdates;
    vector<RegisterUpdates> registerUpdates;
};

void printStageInstruction(const string stage, Instruction instruction)
{
    std::cout << stage << ": "
              << "I" << (pcToCodeMemoryIndex(instruction.address) + 1) << " (" << instruction.address << ") "
              << instruction.rawInstruction << std::endl;
}

void printAddressAndRawInstruction(const Pipe& pipe)
{
    std::cout << "(" << pipe.instruction.address << ") " << pipe.instruction.rawInstruction << std::endl;
}

void printQueueContents(const std::queue<Pipe>& queue)
{
    std::queue<Pipe> tempQueue = queue;

    while (!tempQueue.empty())
    {
        printAddressAndRawInstruction(tempQueue.front());
        tempQueue.pop();
    }
}

bool shouldStallHelper(const Instruction& current, const Instruction& previous)
{
    // Check for data hazards
    if (current.source1 && (current.source1 == previous.destination || current.source1 == previous.source2))
    {
        // Hazard: Source1 of current instruction depends on the result of the previous instruction
        return true;
    }

    if (current.source2 && (current.source2 == previous.destination || current.source2 == previous.source2))
    {
        // Hazard: Source2 of current instruction depends on the result of the previous instruction
        return true;
    }

    // Add more hazard conditions based on your specific requirements

    // If no hazard conditions are met, return false
    return false;
}

bool shouldStallHelperForwarding(const Instruction& current, const Instruction& previous)
{
    if (previous.opcode == config::Opcode::LOAD || previous.opcode == config::Opcode::LOADP ||
        previous.opcode == config::Opcode::STORE || previous.opcode == config::Opcode::STOREP)
    {

        // if (previous.opcode == config::Opcode::STOREP)
        // {
        //     cout << "STALL STOREP" << endl;
        //     if (current.source1 && (current.source1 == previous.source2))
        //     {
        //         // Hazard: Source1 of current instruction depends on the result of the previous instruction
        //         return true;
        //     }

        //     if (current.source2 && (current.source2 == previous.source2))
        //     {
        //         // Hazard: Source2 of current instruction depends on the result of the previous instruction
        //         return true;
        //     }

        //     return false;
        // }

        if (previous.opcode == config::Opcode::LOAD)
        {
            if (current.source1 && (current.source1 == previous.destination))
            {
                // Hazard: Source1 of current instruction depends on the result of the previous instruction
                return true;
            }

            if (current.source2 && (current.source2 == previous.destination))
            {
                // Hazard: Source2 of current instruction depends on the result of the previous instruction
                return true;
            }

            return false;
        }
        else if (previous.opcode == config::Opcode::LOADP)
        {
            if (current.source1 && (current.source1 == previous.destination || current.source1 == previous.source1))
            {
                // Hazard: Source1 of current instruction depends on the result of the previous instruction
                return true;
            }

            if (current.source2 && (current.source2 == previous.destination || current.source1 == previous.source1))
            {
                // Hazard: Source2 of current instruction depends on the result of the previous instruction
                return true;
            }

            return false;
        }

        return false;
    }

    return false;
}

bool shouldStall(queue<Pipe> fetchQueue, queue<Pipe> dRFQueue, queue<Pipe> exQueue)
{
    if (!fetchQueue.empty() && !dRFQueue.empty())
    {
        auto second = dRFQueue.front();
        auto first = fetchQueue.front();

        if (shouldStallHelper(first.instruction, second.instruction))
        {
            return true;
        }
    }

    if (!fetchQueue.empty() && !exQueue.empty())
    {

        auto second = exQueue.front();
        auto first = fetchQueue.front();

        if (shouldStallHelper(first.instruction, second.instruction))
        {
            return true;
        }
    }

    return false;
}

bool shouldStallForwarding(queue<Pipe> fetchQueue, queue<Pipe> dRFQueue, queue<Pipe> exQueue)
{
    if (!fetchQueue.empty() && !dRFQueue.empty())
    {
        auto second = dRFQueue.front();
        auto first = fetchQueue.front();

        if (shouldStallHelperForwarding(first.instruction, second.instruction))
        {
            return true;
        }
    }

    if (!dRFQueue.empty() && !exQueue.empty())
    {

        auto second = exQueue.front();
        auto first = dRFQueue.front();

        if (shouldStallHelperForwarding(first.instruction, second.instruction))
        {
            return true;
        }
    }

    return false;
}

Instruction createNOP()
{
    return Instruction{.rawInstruction = "NOP", .opcode = config::Opcode::NOP, .address = 0};
}

void performCommonOperations(queue<Pipe>& fetchQueue, queue<Pipe>& dRFQueue, int& PC, int address, int pcRelativeNumber)
{
    auto nop = createNOP();

    resetQueue(fetchQueue);
    resetQueue(dRFQueue);

    dRFQueue.push({nop, {}, {}});
    fetchQueue.push({nop, {}, {}});
    PC = pcToCodeMemoryIndex(address) + pcRelativeNumber;
}

void printQueues(queue<Pipe> fetchQueue, queue<Pipe> dRFQueue, queue<Pipe> exQueue, queue<Pipe> memQueue)
{
    cout << "Fetch Queue: (DECODE) " << endl;
    printQueueContents(fetchQueue);
    cout << "Decode/RF Queue: (EXECUTE) " << endl;
    printQueueContents(dRFQueue);
    cout << "Execute Queue: (MEMORY)" << endl;
    printQueueContents(exQueue);
    cout << "Memory Queue: (WRITEBACK)" << endl;
    printQueueContents(memQueue);
    cout << endl;
}

int main(int argc, char* argv[])
{
    // 0 corresponds with 4000
    int PC = 0;
    int NEXT_INSTRUCTION_AFTER_G = 4;
    int CYCLE = 0;
    bool STOP_FETCH = false;
    bool STALL = false;

    int P = 0;
    int N = 0;
    int Z = 0;

    queue<Pipe> fetchQueue;
    queue<Pipe> dRFQueue;
    queue<Pipe> exQueue;
    queue<Pipe> memQueue;

    RegisterFile& regFile = RegisterFile::instance();
    RegisterFileFake& regFileForward = RegisterFileFake::instance();
    Memory& memory = Memory::instance();
    BranchTargetBuffer btb;

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

    cout << "Instructions" << endl;

    vector<string> lines = fileRes.getValue();

    auto instructionsStrSeq = getInstructionStrSqFromLines(lines);
    auto [instructionMap, instructionSeq] =
        instructionSeqFromInstStrSeq(config::CPU::PROGRAM_COUNTER_START_ADDRESS, instructionsStrSeq);

    printInstructions(instructionSeq);

    while (CYCLE != NUM_SIMULATION_CYCLES)
    {
        cout << "\n"
             << "..... Cycle: " << CYCLE + 1 << " ....."
             << "\n"
             << endl;

        // printQueues(fetchQueue, dRFQueue, exQueue, memQueue);

        if (!STOP_FETCH)
        {

            auto instMemLocation = codeMemoryIndexToPC(PC);

            if (keyExistsInMap(instructionMap, instMemLocation))
            {
                auto instructionFETCH = instructionMap[instMemLocation];
                auto [btbHit, targetAddress] =
                    btb.predictBranchOutcome(instructionFETCH.opcode, instructionFETCH.address);
                bool mustStall = false;

                auto fakeFetchQueue = copyQueue(fetchQueue);
                fakeFetchQueue.push({instructionFETCH});

                if (btbHit)
                {
                    cout << "BTB HIT" << endl;

                    if (IS_FORWARDING)
                    {
                        mustStall = shouldStallForwarding(fakeFetchQueue, dRFQueue, exQueue);
                    }
                    else
                    {
                        mustStall = shouldStall(fakeFetchQueue, dRFQueue, exQueue);
                    }
                    if (!mustStall)
                    {
                        PC = pcToCodeMemoryIndex(targetAddress.value());
                    }
                }

                if (IS_FORWARDING)
                {
                    mustStall = shouldStallForwarding(fakeFetchQueue, dRFQueue, exQueue);
                }
                else
                {
                    mustStall = shouldStall(fakeFetchQueue, dRFQueue, exQueue);
                }

                if (!mustStall)
                {

                    if (!btbHit)
                    {
                        PC += 1;
                    }

                    fetchQueue.push({instructionFETCH});
                    // FETCH

                    printStageInstruction("FETCH", instructionFETCH);
                }
                else
                {
                    printStageInstruction("FETCH STALL", instructionFETCH);
                }
            }
        }

        if (CYCLE > 0 && !fetchQueue.empty())
        {
            STALL = false;
            auto instructionDECODE = fetchQueue.front();

            if (IS_FORWARDING)
            {
                STALL = shouldStallForwarding(fetchQueue, dRFQueue, exQueue);
            }
            else
            {
                STALL = shouldStall(fetchQueue, dRFQueue, exQueue);
            }

            if (!STALL)
            {

                if (instructionDECODE.instruction.opcode == config::Opcode::HALT)
                {
                    STOP_FETCH = true;
                }

                printStageInstruction("DECODE", instructionDECODE.instruction);

                auto opcode = instructionDECODE.instruction.opcode;
                if (opcode == config::Opcode::BP || opcode == config::Opcode::BNZ)
                {
                    btb.push(instructionDECODE.instruction.address, true, true, nullopt);
                }
                else if (opcode == config::Opcode::BZ || opcode == config::Opcode::BNP)
                {
                    btb.push(instructionDECODE.instruction.address, false, false, nullopt);
                }

                dRFQueue.push(instructionDECODE);
                fetchQueue.pop();
            }
            else
            {
                auto nop = createNOP();
                dRFQueue.push({nop, {}, {}});

                printStageInstruction("DECODE STALL", instructionDECODE.instruction);
            }
        }

        if (CYCLE > 1 && !dRFQueue.empty())
        {
            bool POP_DRF_QUEUE = true;
            auto instructionEXECUTE = dRFQueue.front();
            auto opcode = instructionEXECUTE.instruction.opcode;
            printStageInstruction("EXECUTE", instructionEXECUTE.instruction);

            if (opcode == config::Opcode::NOP || opcode == config::Opcode::HALT)
            {
                exQueue.push(instructionEXECUTE);
            }
            else if (opcode == config::Opcode::MOVC)
            {
                auto literal = instructionEXECUTE.instruction.literal.value();

                exQueue.push(Pipe{instructionEXECUTE.instruction,
                                  {},
                                  {{instructionEXECUTE.instruction.destination.value(), literal}}});
            }
            else if (opcode == config::Opcode::ADD || opcode == config::Opcode::SUB || opcode == config::Opcode::MUL ||
                     opcode == config::Opcode::AND || opcode == config::Opcode::OR || opcode == config::Opcode::XOR)
            {
                auto src1 = instructionEXECUTE.instruction.source1.value();
                auto src2 = instructionEXECUTE.instruction.source2.value();
                auto regSrc1 = regFile.getRegFromString(src1).value();
                auto regSrc2 = regFile.getRegFromString(src2).value();
                int result = 0;
                if (opcode == config::Opcode::ADD)
                {
                    result = regSrc1->get() + regSrc2->get();
                }
                else if (opcode == config::Opcode::SUB)
                {
                    result = regSrc1->get() - regSrc2->get();
                }
                else if (opcode == config::Opcode::MUL)
                {
                    result = regSrc1->get() * regSrc2->get();
                }
                else if (opcode == config::Opcode::AND)
                {
                    result = regSrc1->get() & regSrc2->get();
                }
                else if (opcode == config::Opcode::OR)
                {
                    result = regSrc1->get() | regSrc2->get();
                }
                else if (opcode == config::Opcode::XOR)
                {
                    result = regSrc1->get() ^ regSrc2->get();
                }

                resetAndUpdatePNZ(P, N, Z, result);

                exQueue.push(Pipe{instructionEXECUTE.instruction,
                                  {},
                                  {{instructionEXECUTE.instruction.destination.value(), result}}});
            }
            else if (opcode == config::Opcode::CML || opcode == config::Opcode::JUMP)
            {
                auto source1 = regFile.getRegFromString(instructionEXECUTE.instruction.source1.value()).value();
                auto literal = instructionEXECUTE.instruction.literal.value();

                if (opcode == config::Opcode::CML)
                {
                    resetAndUpdatePNZ(P, N, Z, source1->get() - literal);
                    exQueue.push(instructionEXECUTE);
                }
                else if (opcode == config::Opcode::JUMP)
                {
                    resetQueue(fetchQueue);
                    resetQueue(dRFQueue);
                    auto nop = createNOP();
                    dRFQueue.push({nop, {}, {}});
                    fetchQueue.push({nop, {}, {}});
                    auto location = source1->get() + literal;
                    PC = pcToCodeMemoryIndex(location);
                    POP_DRF_QUEUE = false;
                    STOP_FETCH = false;

                    exQueue.push(instructionEXECUTE);
                }
            }
            else if (opcode == config::Opcode::CMP)
            {
                auto source1 = regFile.getRegFromString(instructionEXECUTE.instruction.source1.value()).value();
                auto source2 = regFile.getRegFromString(instructionEXECUTE.instruction.source2.value()).value();
                int result = source1->get() - source2->get();
                resetAndUpdatePNZ(P, N, Z, result);
                exQueue.push(instructionEXECUTE);
            }
            else if (opcode == config::Opcode::BZ || opcode == config::Opcode::BNZ || opcode == config::Opcode::BP ||
                     opcode == config::Opcode::BNP || opcode == config::Opcode::BN || opcode == config::Opcode::BNN)
            {
                auto literal = instructionEXECUTE.instruction.literal.value();
                auto pcRelativeNumber = getRelativeNumber(literal);

                if (((opcode == config::Opcode::BNP && P == 0) || (opcode == config::Opcode::BP && P == 1) ||
                     (opcode == config::Opcode::BN && N == 1) || (opcode == config::Opcode::BNN && N == 0) ||
                     (opcode == config::Opcode::BZ && Z == 1) || (opcode == config::Opcode::BNZ && Z == 0)))
                {
                    // performCommonOperations(fetchQueue, dRFQueue, PC, instructionEXECUTE.instruction.address,
                    //                         pcRelativeNumber);
                    // cout << "PC" << PC << "  "
                    //      << pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber << endl;
                    // cout << fetchQueue.front().instruction.address << "  "
                    //      << pcToCodeMemoryIndex(fetchQueue.front().instruction.address) << endl;

                    // If instruction coming behind is equal not equal to the branch target

                    // cout << "=====  " << pcToCodeMemoryIndex(dRFQueue.back().instruction.address) << "  "
                    //      << dRFQueue.back().instruction.rawInstruction << " oooooo"
                    //      << "   fetch:" << fetchQueue.front().instruction.rawInstruction
                    //      << pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber << endl;

                    printQueues(fetchQueue, dRFQueue, exQueue, memQueue);
                    cout << "FETCH QUEUE" << fetchQueue.front().instruction.rawInstruction << endl;
                    cout << "Next instruction: " << dRFQueue.back().instruction.address << "  "
                         << pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber << "jjjjj"
                         << instructionMap[instructionEXECUTE.instruction.address + literal].rawInstruction << endl;

                    if (dRFQueue.back().instruction.address != 0)
                    {
                        if (pcToCodeMemoryIndex(dRFQueue.back().instruction.address) !=
                            pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber)
                        {
                            auto nop = createNOP();

                            resetQueue(fetchQueue);
                            resetQueue(dRFQueue);

                            dRFQueue.push({nop, {}, {}});
                            fetchQueue.push({nop, {}, {}});
                            PC = pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber;
                            POP_DRF_QUEUE = false;
                        }
                    }
                    else
                    {
                        if (pcToCodeMemoryIndex(fetchQueue.back().instruction.address) !=
                            pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber)
                        {
                            auto nop = createNOP();

                            resetQueue(fetchQueue);
                            resetQueue(dRFQueue);

                            dRFQueue.push({nop, {}, {}});
                            fetchQueue.push({nop, {}, {}});
                            PC = pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber;
                            POP_DRF_QUEUE = false;
                        }
                    }

                    auto opcode = instructionEXECUTE.instruction.opcode;
                    if (opcode == config::Opcode::BP || opcode == config::Opcode::BNZ)
                    {
                        btb.updateBTB(instructionEXECUTE.instruction.address, true,
                                      instructionEXECUTE.instruction.address + literal);
                    }
                    else if (opcode == config::Opcode::BZ || opcode == config::Opcode::BNP)
                    {
                        btb.updateBTB(instructionEXECUTE.instruction.address, true,
                                      instructionEXECUTE.instruction.address + literal);
                    }

                    cout << "BRANCHING TO: " << instructionEXECUTE.instruction.address + literal << endl;
                    STOP_FETCH = false;
                }
                else
                {
                    cout << "NOT BRANCHING" << endl;

                    btb.updateBTB(instructionEXECUTE.instruction.address, false,
                                  instructionEXECUTE.instruction.address + literal);

                    if (dRFQueue.back().instruction.address != 0)
                    {
                        if (pcToCodeMemoryIndex(dRFQueue.back().instruction.address) ==
                            pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber)
                        {

                            cout << instructionMap[instructionEXECUTE.instruction.address + literal].rawInstruction
                                 << endl;
                            cout << "NOT BRANCHING 1" << endl;
                            auto nop = createNOP();

                            resetQueue(fetchQueue);
                            resetQueue(dRFQueue);

                            dRFQueue.push({nop, {}, {}});
                            fetchQueue.push({nop, {}, {}});
                            PC = pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + 1;
                            POP_DRF_QUEUE = false;
                        }
                    }
                    else
                    {
                        if (pcToCodeMemoryIndex(fetchQueue.back().instruction.address) ==
                            pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + pcRelativeNumber)
                        {
                            cout << "NOT BRANCHING 2" << endl;
                            auto nop = createNOP();

                            resetQueue(fetchQueue);
                            resetQueue(dRFQueue);

                            dRFQueue.push({nop, {}, {}});
                            fetchQueue.push({nop, {}, {}});
                            PC = pcToCodeMemoryIndex(instructionEXECUTE.instruction.address) + 1;
                            POP_DRF_QUEUE = false;
                        }
                    }
                }

                exQueue.push(instructionEXECUTE);
            }
            else if (opcode == config::Opcode::ADDL || opcode == config::Opcode::SUBL)
            {
                auto regSrc1 = regFile.getRegFromString(instructionEXECUTE.instruction.source1.value()).value();
                auto literal = instructionEXECUTE.instruction.literal.value();
                int result = 0;

                if (opcode == config::Opcode::ADDL)
                {
                    result = (regSrc1->get() + literal);
                }
                else if (opcode == config::Opcode::SUBL)
                {
                    result = (regSrc1->get() - literal);
                }

                resetAndUpdatePNZ(P, N, Z, result);

                exQueue.push(Pipe{instructionEXECUTE.instruction,
                                  {},
                                  {{instructionEXECUTE.instruction.destination.value(), result}}});
            }
            else if (opcode == config::Opcode::LOAD || opcode == config::Opcode::LOADP)
            {
                auto regSrc1 = regFile.getRegFromString(instructionEXECUTE.instruction.source1.value()).value();
                auto literal = instructionEXECUTE.instruction.literal.value();

                if (opcode == config::Opcode::LOAD)
                {
                    auto result = memory.read(regSrc1->get() + literal).getValue();
                    exQueue.push(Pipe{instructionEXECUTE.instruction,
                                      {},
                                      {{instructionEXECUTE.instruction.destination.value(), result}}});
                }
                else if (opcode == config::Opcode::LOADP)
                {
                    auto result = memory.read(regSrc1->get() + literal).getValue();

                    exQueue.push(Pipe{instructionEXECUTE.instruction,
                                      {},
                                      {{instructionEXECUTE.instruction.destination.value(), result}

                                       ,
                                       {instructionEXECUTE.instruction.source1.value(), regSrc1->get() + 4}}});
                }
            }
            else if (opcode == config::Opcode::STORE || opcode == config::Opcode::STOREP)
            {
                auto regSrc1 = regFile.getRegFromString(instructionEXECUTE.instruction.source1.value()).value();
                auto regSrc2 = regFile.getRegFromString(instructionEXECUTE.instruction.source2.value()).value();
                auto literal = instructionEXECUTE.instruction.literal.value();

                if (opcode == config::Opcode::STORE)
                {
                    auto memAddress = regSrc2->get() + literal;
                    auto value = regSrc1->get();

                    exQueue.push(Pipe{instructionEXECUTE.instruction, {{memAddress, value}}, {}});
                }
                else if (opcode == config::Opcode::STOREP)
                {
                    auto memAddress = regSrc2->get() + literal;
                    auto value = regSrc1->get();

                    exQueue.push(Pipe{instructionEXECUTE.instruction,
                                      {{memAddress, value}},
                                      {{instructionEXECUTE.instruction.source2.value(), regSrc2->get() + 4}}});
                }
            }
            else if (opcode == config::Opcode::JALR)
            {
                auto regSrc1 = regFile.getRegFromString(instructionEXECUTE.instruction.source1.value()).value();
                auto literal = instructionEXECUTE.instruction.literal.value();

                PC = pcToCodeMemoryIndex(regSrc1->get() + literal);

                resetQueue(fetchQueue);
                resetQueue(dRFQueue);
                auto nop = createNOP();
                dRFQueue.push({nop, {}, {}});
                fetchQueue.push({nop, {}, {}});
                POP_DRF_QUEUE = false;
                STOP_FETCH = false;

                exQueue.push({instructionEXECUTE.instruction,
                              {},
                              {{instructionEXECUTE.instruction.destination.value(),
                                instructionEXECUTE.instruction.address + 4}}});
            }

            if (POP_DRF_QUEUE == true)
            {
                dRFQueue.pop();
            }

            if (IS_FORWARDING)
            {
                auto forwardingQueue = copyQueue(exQueue);

                auto instructionBack = forwardingQueue.back();

                // printStageInstruction("WRITEBACK", instructionWRITEBACK.instruction);

                for (auto& regUpdate : instructionBack.registerUpdates)
                {
                    regFile.setRegFromString(regUpdate.registerName, regUpdate.value);
                }
            }
        }

        if (CYCLE > 2 && !exQueue.empty())
        {
            auto instructionMEMORY = exQueue.front();

            printStageInstruction("MEMORY", instructionMEMORY.instruction);

            for (auto& memUpdate : instructionMEMORY.memoryUpdates)
            {
                cout << "--------" << endl;
                cout << memUpdate.address << " value" << memUpdate.value << endl;
                memory.write(memUpdate.address, memUpdate.value);
            }

            memQueue.push(instructionMEMORY);
            exQueue.pop();
        }

        if (CYCLE > 3 && !memQueue.empty())
        {
            auto instructionWRITEBACK = memQueue.front();

            printStageInstruction("WRITEBACK", instructionWRITEBACK.instruction);

            if (IS_FORWARDING)
            {
                {
                    for (auto& regUpdate : instructionWRITEBACK.registerUpdates)
                        regFileForward.setRegFromString(regUpdate.registerName, regUpdate.value);
                }
            }
            else
            {
                for (auto& regUpdate : instructionWRITEBACK.registerUpdates)
                    regFile.setRegFromString(regUpdate.registerName, regUpdate.value);
            }

            if (instructionWRITEBACK.instruction.opcode == config::Opcode::HALT)
            {
                HALT_REACHED = true;
            }

            memQueue.pop();
        }

        if (IS_FORWARDING)
        {
            regFileForward.print();
        }
        else
        {
            regFile.print();
        }

        // printQueues(fetchQueue, dRFQueue, exQueue, memQueue);

        cout << "P: " << P << " Z: " << Z << " N: " << N << endl;

        // Result<uint8_t, Error> result = memory.read(64); // Store the result in a variable
        // if (result.isOk())
        // {
        //     cout << "MEM READ: " << static_cast<int>(result.getValue()) << endl;
        // }

        btb.printBTB();

        CYCLE++;

        cin.get();

        if (HALT_REACHED)
        {
            exit(0);
        }
    }

    return 0;
}
