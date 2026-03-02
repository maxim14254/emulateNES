#ifndef APU_H
#define APU_H

#include <QIODevice>
#include <deque>
#include "global.h"
#include <QtMultimedia/QAudioOutput>


class RingBufferSPSC
{

public:
    RingBufferSPSC(size_t capacity);

    bool write(qint16 val, size_t n);
    bool read(qint16* val, size_t n);
    size_t bytesAvailable() const;
    qint16 next();

private:
    std::vector<qint16> buf;
    size_t cap = 0;
    std::atomic<size_t> head {0}; // write index
    std::atomic<size_t> tail {0}; // read index

    size_t availableToRead() const;
    size_t availableToWrite() const;

};


class Bus;

class APU : public QIODevice
{
    Q_OBJECT

    struct OnePoleLPF
    {
        double a = 0.0;
        double y = 0.0;

        void setCutoff(double fc, double fs)
        {
            a = 1.0 - std::exp(-2.0 * M_PI * fc / fs);
        }

        double process(double x)
        {
            y += a * (x - y);
            return y;
        }
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
                        if (channel == 0) // pulse1
                            target -= (change + 1);
                        else              // pulse2
                            target -= change;
                    }
                    else
                    {
                        target += change;   // ← sweep UP
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

public:
    explicit APU(double freq, double sampleRate, Bus* _bus, QAudioOutput* sink, QObject *parent = nullptr);

    qint64 readData(char *data, qint64 maxlen) override;

    void start() { open(QIODevice::ReadOnly); }
    void stop()  { close(); }

    qint64 writeData(const char *, qint64) override { return 0; }
    qint64 bytesAvailable() const override { return static_cast<qint64>(ring_buffer.bytesAvailable()) + QIODevice::bytesAvailable(); }

    void run(int cycles);
    void write_registers(uint16_t addr, uint8_t data);

private:
    double m_sampleRate = 48000.0;
    double sampleAccum = 0.0;
    qint16 last = 0;

    Bus* bus;
    QAudioOutput* sink;

    int frame_counter = 0;
    RingBufferSPSC ring_buffer{13500};
    OnePoleLPF onePoleLPF;

    double pulse1_output = 0;
    double pulse2_output = 0;
    uint8_t noise_output = 0;
    uint8_t triangle_output = 0;

    PulseBLEP pulse1, pulse2;
    NoiseLFSR noise;
    Triangle triangle;

    bool pulse1_enable = false;
    bool pulse2_enable = false;
    bool noise_enable = false;
    bool triangle_enable = false;

    double mix_nes(double p1, double p2, double t, double n);

    uint8_t LENGTH_TABLE[32] =
    {
        10,254,20, 2,40, 4,80, 6,160, 8,60,10,14,12,26,14,
        12,16,24,18,48,20,96,22,192,24,72,26,16,28,32,30
    };

};

#endif // APU_H
