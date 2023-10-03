#pragma once

#include "../config/index.hpp"
#include <bitset>
#include <sstream>
using namespace std;

inline string bitsetToHex(bitset<config::CPU::REGISTER_SIZE_BITS> number)
{
    stringstream res;
    res << hex << uppercase << number.to_ulong();
    return res.str();
}

inline bitset<config::CPU::REGISTER_SIZE_BITS> intToBitset(int number)
{
    return bitset<config::CPU::REGISTER_SIZE_BITS>(number);
}
