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

//struct ChannelParams {
//    std::atomic<double> freq {440.0};
//    std::atomic<double> duty {0.5};
//    std::atomic<double> gain {0.0}; // 0..1
//    uint32_t sequence {0x00000000};
//};

struct Sequencer
{
    uint32_t sequence = 0;
    uint32_t new_sequence = 0;
    uint16_t timer = 0;
    uint16_t reload = 0;

    uint8_t output = 0;

    uint8_t clock(bool bEnable, std::function<void(uint32_t &s)> funcManip)
    {
        if (bEnable)
        {
            if (timer == 0)
            {
                timer = reload;
                funcManip(sequence );
                output = sequence  & 1u;
            }
            else
            {
                timer--;
            }
        }
        return output;
    }
};


struct Envelope
{
    bool start = false;
    bool disable = false;
    bool bLoop = false;

    uint16_t divider_count = 0;
    uint16_t volume = 0;
    uint16_t output = 0;
    uint16_t decay_count = 0;

    void clock()
    {
        if (!start)
        {
            if (divider_count == 0)
            {
                divider_count = volume;

                if (decay_count == 0)
                {
                    if (bLoop)
                    {
                        decay_count = 15;
                    }

                }
                else
                    decay_count--;
            }
            else
                divider_count--;
        }
        else
        {
            start = false;
            decay_count = 15;
            divider_count = volume;
        }

        if (disable)
        {
            output = volume;
        }
        else
        {
            output = decay_count;
        }
    }
};

struct Sweep
{
    bool enabled = false;
    uint8_t period = 0;
    bool negate = false;
    uint8_t shift = 0;

    // внутреннее состояние sweep
    bool reload = false;
    uint16_t change = 0;
    uint8_t timer = 0;
    bool mute = false;

    bool clock(uint16_t& target, bool channel)
    {
        bool changed = false;
        change = target >> shift;

        if (timer == 0 && enabled && shift > 0 && !mute)
        {
            if (target >= 8 && change < 0x07FF)
            {
                if (negate)
                {
                    target -= change - channel;
                }
                else
                {
                    target += change;
                }
                changed = true;
            }
        }

        //if (enabled)
        {
            if (timer == 0 || reload)
            {
                timer = period;
                reload = false;
            }
            else
                timer--;

            mute = (target < 8) || (target > 0x7FF);
        }

        return changed;
    }
};


struct PulseBLEP
{
    double phase = 0.0;
    double freq = 440;
    double sr = 48000.0;
    double duty = 0.25;
    uint8_t length_counter = 0;

    Envelope envelope;
    Sweep sweep;
    Sequencer sequencer;

    double process()
    {
        double dt = freq / sr;
        duty = std::clamp(duty, 0.01, 0.99);

        double t = phase;
        double y = (t < duty) ? 1.0 : -1.0;

        y += poly_blep(t, dt);
        double td = t - duty;

        if (td < 0.0)
            td += 1.0;

        y -= poly_blep(td, dt);

        phase += dt;

        if (phase >= 1.0)
            phase -= 1.0;

        return y;
    }

    double poly_blep(double t, double dt)
    {
        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0;
        }
        if (t > 1.0 - dt)
        {
            t = (t - 1.0) / dt;
            return t * t + t + t + 1.0;
        }

        return 0.0;
    }

    uint8_t clock_counter(bool bEnable, bool bHalt)
    {
        if (!bEnable)
            length_counter = 0;
        else
            if (length_counter > 0 && !bHalt)
                --length_counter;

        return length_counter;
    }
};

inline PulseBLEP pulse1, pulse2;
inline std::atomic<bool> pulse1_enable = false;

struct NoiseParams {
    std::atomic<double> noiseFreq {6000.0};
    std::atomic<bool> shortMode {false};
    std::atomic<double> gain {0.0};
};

inline NoiseParams n;

inline std::vector<INSTRUCTION<CPU>> table_instructions;

inline std::atomic<bool> start;

inline std::mutex update_frame_mutex;
inline bool _update = true;
inline std::mutex mutex_lock_frame_buffer;

inline std::condition_variable cv;
inline std::atomic<int> harmonics = 2;

#ifdef DEBUG_ON
inline std::mutex step_by_step_mutex;
inline std::atomic<bool> run_without_mutex = true;
inline bool pause = false;
#endif

#endif // GLOBAL_H
