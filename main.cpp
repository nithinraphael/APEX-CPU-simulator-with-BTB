#include "errors/index.hpp"
#include "fs/index.hpp"
#include "instruction/index.hpp"
#include "instruction/instruction.hpp"
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

    Result<Instruction, Error> inst = parseInstruction("SUB R3, R5");

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

    return 0;
}