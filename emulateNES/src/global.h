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
inline std::atomic<int> numb_table_for_debug = 0;


#ifdef DEBUG_ON
inline std::mutex step_by_step_ppu_mutex;
inline std::mutex step_by_step_cpu_mutex;
inline std::mutex step_by_step_scanline_mutex;

inline std::atomic<bool> run_without_ppu_mutex = true;
inline std::atomic<bool> run_without_cpu_mutex = true;
inline std::atomic<bool> run_without_scanline_mutex = true;
inline std::atomic<int> go_scanline = -1;

inline bool pause_ppu = false;
inline bool pause_cpu = false;
inline bool pause_scanline = false;
#endif

#endif // GLOBAL_H
