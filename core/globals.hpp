#pragma once
#include "../instruction/index.hpp"
#include <memory>
#include <queue>

using namespace std;
using namespace instruction;

namespace core
{
class InstructionSequence
{
  public:
    static InstructionSequence& instance()
    {
        static InstructionSequence instance; // Initialize sequence in the constructor
        return instance;
    }

    InstructionSequence(const InstructionSequence&) = delete;
    InstructionSequence& operator=(const InstructionSequence&) = delete;

    void addInstruction(const Instruction& instruction)
    {
        sequence->push_back(instruction);
    }

    void init(vector<Instruction> seq)
    {
        sequence = make_unique<vector<Instruction>>(seq);
    }

    const vector<Instruction>& getSequence() const
    {
        return *sequence;
    }

    const void printSeq(bool isDebug)
    {
        printInstructionList(*sequence, isDebug);
    }

  private:
    InstructionSequence()
    {
    }

    unique_ptr<vector<Instruction>> sequence;
};

}; // namespace core
