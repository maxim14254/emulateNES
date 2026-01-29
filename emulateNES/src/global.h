#pragma once

#ifndef GLOBAL_H
#define GLOBAL_H


#include <iostream>
#include <functional>
#include <vector>
#include <mutex>
#include <atomic>

class CPU;

template<typename T>
struct INSTRUCTION
{
    std::function<void(T&)> func;
    std::string name;
    std::string addrmode;
};

inline std::vector<INSTRUCTION<CPU>> table_instructions;


#ifdef DEBUG_ON
inline std::mutex step_by_step_mutex;
inline std::atomic<bool> run_without_mutex = true;
#endif

#endif // GLOBAL_H
