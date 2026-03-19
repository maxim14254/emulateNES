#pragma once

#include <algorithm>
#ifndef GLOBAL_H
#define GLOBAL_H


#include <functional>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <cmath>

class CPU;

template<typename T>
struct INSTRUCTION
{
    std::function<void(T&)> func;
    std::string name;
    std::string addrmode;
};

inline std::vector<INSTRUCTION<CPU>> table_instructions;

inline std::atomic<bool> start;

inline std::mutex update_frame_mutex;
inline bool _update = true;
inline std::mutex mutex_lock_frame_buffer;

inline std::condition_variable cv;
inline bool sssss = false;

//inline int16_t scanline = 0;
//inline int16_t cycle = 0;


#ifdef DEBUG_ON
inline std::mutex step_by_step_mutex;
inline std::atomic<bool> run_without_mutex = true;
inline bool pause = false;
#endif

#endif // GLOBAL_H
