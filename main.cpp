#include "core/index.hpp"
#include "core/register.hpp"
#include "errors/index.hpp"
#include "fs/index.hpp"
#include "instruction/index.hpp"
#include "instruction/instruction.hpp"
#include "parser/index.hpp"
#include "parser/parser.hpp"
#include "utils/index.hpp"

#include <iostream>
#include <ostream>

using namespace fs;
using namespace std;
using namespace utils;
using namespace instruction;

int main(int argc, char* argv[])
{
    Result<string, Error> filenameRes = getFileNameCMD(argc, argv);
    if (filenameRes.isError())
    {
        Error err = filenameRes.getError();
        err.print();
        exit(1);
    }

    FileUtil file(filenameRes.getValue());

    Result<vector<string>, Error> res = file.readLines();

    if (res.isOk())
    {
        for (const std::string& str : res.getValue())
        {
            std::cout << str << std::endl;
        }
    }

    Result<InstructionStr, Error> inst = parseInstructionStr("SUB R3, R5");

    cout << "Opcode: " << inst.getValue().opcode << endl;
    if (inst.getValue().operand1)
    {

        cout << "1: " << inst.getValue().operand1.value() << endl;
    }
    if (inst.getValue().operand2)
    {

        cout << "2: " << inst.getValue().operand2.value() << endl;
    }
    if (inst.getValue().operand3)
    {

        cout << "3: " << inst.getValue().operand3.value() << endl;
    }

    Result<InstructionStr, Error> inst2 = parseInstructionStr("SUB R3, R5");

    if (inst2.isOk())
    {
        auto op = parser::parseOpcode(inst2.getValue().opcode);
        if (op.isOk())
        {
            parser::printOpcode(op.getValue());
        }
    }

    RegisterFile& regFile = RegisterFile::instance();
    optional<Register*> result = regFile.get(0);

    if (result)
    {
        Register r = *result.value();
        r.set(8);
        r.print();
    }

    result = regFile.get(2);

    return 0;
}