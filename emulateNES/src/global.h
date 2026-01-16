#pragma once

#ifndef GLOBAL_H
#define GLOBAL_H


#include <iostream>
#include <functional>
#include <vector>

class CPU;

template<typename T>
struct INSTRUCTION
{
    std::function<void(T&)> func;
    std::string name;
    std::string addrmode;
};

inline std::vector<INSTRUCTION<CPU>> table_instructions;

#endif // GLOBAL_H
