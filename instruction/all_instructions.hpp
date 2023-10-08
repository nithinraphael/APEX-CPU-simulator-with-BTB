// #pragma once
// #include "../core/index.hpp"
// #include <variant>

// namespace instruction
// {

// struct ADD
// {
//     ADD(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // SUB instruction: Subtracts the value in the second source register from the first source register and stores the
// // result in the destination register
// struct SUB
// {
//     SUB(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // MUL instruction: Multiplies values in source registers and stores the result in the destination register
// struct MUL
// {
//     MUL(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // DIV instruction: Divides the value in the first source register by the second source register and stores the
// result
// // in the destination register
// struct DIV
// {
//     DIV(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // AND instruction: Performs a bitwise AND operation between source registers and stores the result in the
// destination
// // register
// struct AND
// {
//     AND(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // OR instruction: Performs a bitwise OR operation between source registers and stores the result in the destination
// // register
// struct OR
// {
//     OR(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // XOR instruction: Performs a bitwise XOR operation between source registers and stores the result in the
// destination
// // register
// struct XOR
// {
//     XOR(Register& destReg, const Register& srcReg1, const Register& srcReg2)
//         : dest(destReg), source1(srcReg1), source2(srcReg2)
//     {
//     }

//     Register& dest;
//     const Register& source1;
//     const Register& source2;
// };

// // MOVC instruction: Moves a constant value into the destination register
// struct MOVC
// {
//     MOVC(Register& destReg, int constantValue) : dest(destReg), value(constantValue)
//     {
//     }

//     Register& dest;
//     int value;
// };

// // LOAD instruction: Loads the value from memory address (sourceReg + offset) and stores it in the destination
// register struct LOAD
// {
//     LOAD(Register& destReg, const Register& srcReg, int offset) : dest(destReg), source(srcReg), offsetValue(offset)
//     {
//     }

//     Register& dest;
//     const Register& source;
//     int offsetValue;
// };

// // STORE instruction: Stores the value from the source register into memory address (destReg + offset)
// struct STORE
// {
//     STORE(const Register& srcReg, const Register& destReg, int offset)
//         : source(srcReg), dest(destReg), offsetValue(offset)
//     {
//     }

//     const Register& source;
//     const Register& dest;
//     int offsetValue;
// };

// // BZ instruction: Branches to the specified label if the condition (R == 0) is met
// struct BZ
// {
//     BZ(const Register& conditionReg, const std::string& labelName) : condition(conditionReg), label(labelName)
//     {
//     }

//     const Register& condition;
//     std::string label;
// };

// // BNZ instruction: Branches to the specified label if the condition (R != 0) is met
// struct BNZ
// {
//     BNZ(const Register& conditionReg, const std::string& labelName) : condition(conditionReg), label(labelName)
//     {
//     }

//     const Register& condition;
//     std::string label;
// };

// // HALT instruction: Halts the execution of the program
// struct HALT
// {
//     HALT()
//     {
//     }
// };

// } // namespace instruction

// namespace instruction
// {
// // using Instruction = variant<ADD, SUB, MUL, DIV, AND, OR, XOR, MOVC, LOAD, STORE, BZ, BNZ, HALT>;
// }