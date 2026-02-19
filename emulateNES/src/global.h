#pragma once

#ifndef GLOBAL_H
#define GLOBAL_H


#include <functional>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

class CPU;

template<typename T>
struct INSTRUCTION
{
    std::function<void(T&)> func;
    std::string name;
    std::string addrmode;
};

struct ChannelParams {
    std::atomic<double> freq {440.0};
    std::atomic<double> duty {0.5};
    std::atomic<double> gain {0.0}; // 0..1
};

struct NoiseParams {
    std::atomic<double> noiseFreq {6000.0};
    std::atomic<bool> shortMode {false};
    std::atomic<double> gain {0.0};
};

inline std::vector<INSTRUCTION<CPU>> table_instructions;

inline std::atomic<bool> start;

inline std::mutex update_frame_mutex;
inline bool _update = true;
inline std::mutex mutex_lock_frame_buffer;

inline std::condition_variable cv;
inline std::atomic<int> harmonics = 2;

inline ChannelParams p1, p2, t;
inline NoiseParams n;


#ifdef DEBUG_ON
inline std::mutex step_by_step_mutex;
inline std::atomic<bool> run_without_mutex = true;
inline bool pause = false;
#endif

#endif // GLOBAL_H
