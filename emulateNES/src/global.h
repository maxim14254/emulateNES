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
                funcManip(sequence);
                output = sequence & 1u;
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

struct NoiseLFSR
{
    bool shortMode = false;
    uint8_t periodIndex = 0;

    Envelope envelope;
    uint8_t length_counter = 0;

    uint16_t timer = 0;
    uint16_t shiftReg = 1;

    int NTSC_periods[16]{4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};

    void clock_timer()
    {
        if (timer == 0)
        {
            timer = NTSC_periods[periodIndex];

            // LFSR update (15-bit). Feedback is XOR of bit0 and (bit1 or bit6).
            uint16_t bit0 = shiftReg & 0x0001;
            uint16_t tap  = shortMode ? ((shiftReg >> 6) & 0x0001)
                                            : ((shiftReg >> 1) & 0x0001);

            int16_t feedback = bit0 ^ tap;

            shiftReg >>= 1;
            shiftReg |= (feedback << 14);
        }
        else
           --timer;
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

    uint8_t output()
    {
        if (length_counter == 0)
            return 0;

        if (shiftReg & 0x0001)
            return 0;

        return envelope.output;
    }
};

struct Triangle
{
    uint16_t timer = 0;
    uint16_t reload = 0;

    uint8_t seq_index = 0;

    uint8_t length_counter = 0;

    uint8_t linear_counter = 0;
    uint8_t linear_reload_value = 0;
    bool linear_reload_flag = false;
    bool control_flag = false;

    uint8_t TRI_TABLE[32] = {
        15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
         0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
    };

    void clock_linear()
    {
        if (linear_reload_flag)
            linear_counter = linear_reload_value;
        else if (linear_counter > 0)
            linear_counter--;

        if (!control_flag)
            linear_reload_flag = false;
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

    void clock_timer()
    {
        if (timer == 0)
        {
            timer = reload;

            if (length_counter > 0 && linear_counter > 0)
                seq_index = (seq_index + 1) & 31;
        }
        else
        {
            timer--;
        }
    }

    uint8_t output() const
    {
        if (length_counter == 0 || linear_counter == 0)
            return 0;

        return TRI_TABLE[seq_index];
    }

    double sample() const
    {
        return (double)output() / 15.0;
    }
};


inline PulseBLEP pulse1, pulse2;
inline NoiseLFSR noise;
inline Triangle triangle;

inline bool pulse1_enable = false;
inline bool pulse2_enable = false;
inline bool noise_enable = false;
inline bool triangle_enable = false;


inline std::vector<INSTRUCTION<CPU>> table_instructions;

inline std::atomic<bool> start;

inline std::mutex update_frame_mutex;
inline bool _update = true;
inline std::mutex mutex_lock_frame_buffer;

inline std::condition_variable cv;

#ifdef DEBUG_ON
inline std::mutex step_by_step_mutex;
inline std::atomic<bool> run_without_mutex = true;
inline bool pause = false;
#endif

#endif // GLOBAL_H
