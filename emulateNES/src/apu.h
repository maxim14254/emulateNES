#ifndef APU_H
#define APU_H

#include <QIODevice>
#include <deque>


struct Envelope
{
    bool start = false;
    bool disable = false;
    bool bLoop = false;

    uint16_t divider_count = 0;
    uint16_t volume = 0;
    uint16_t output = 0;
    uint16_t decay_count = 0;

    void clock_envelope()
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
    double freq = 440.0;
    double timer = 0;
    double sr = 48000.0;
    double duty = 0.25;
    uint8_t length_counter = 0;

    Envelope envelope;
    Sweep sweep;

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
};

struct TriangleClean
{

    double phase = 0.0;
    double freq = 440.0;
    double sr = 48000.0;
    Envelope envelope;
    Sweep sweep;

    double process()
    {
        double dt = freq / sr;
        double t = phase;

        double y = 4.0 * std::abs(t - 0.5) - 1.0;
        y = -y;

        phase += dt;

        if (phase >= 1.0)
            phase -= 1.0;

        return y;
    }
};

struct NoiseLFSR
{
    uint16_t lfsr = 1u;
    double sr = 48000.0;
    double noiseFreq = 8000.0;
    double phase = 0.0;
    bool shortMode = false;
    double last = 0.0;
    Envelope envelope;
    Sweep sweep;

    double process()
    {
        double dt = noiseFreq / sr;
        phase += dt;
        if (phase >= 1.0)
        {
            phase -= 1.0;

            uint16_t b0 = lfsr & 1u;
            uint16_t tap = shortMode ? ((lfsr >> 6) & 1u) : ((lfsr >> 1) & 1u);
            uint16_t fb = b0 ^ tap;

            lfsr >>= 1;
            lfsr |= (fb << 14);
            last = (b0 ? 1.0 : -1.0);
        }

        return last;
    }
};


class Bus;

class APU : public QIODevice
{
    Q_OBJECT

public:
    explicit APU(double freq, int sampleRate, Bus* _bus, QObject *parent = nullptr);

    qint64 readData(char *data, qint64 maxlen) override;

    void start() { open(QIODevice::ReadOnly); }
    void stop()  { close(); }

    qint64 writeData(const char *, qint64) override { return 0; }
    qint64 bytesAvailable() const override { return 4096 + QIODevice::bytesAvailable(); }

    void run(int cycles);

private:
    double m_freq = 440.0;
    int m_sampleRate = 48000;
    double m_phase = 0.0;

    PulseBLEP pulse1, pulse2;
    TriangleClean tri;
    NoiseLFSR noise;

    Bus* bus;

    int frame_counter = 0;
    std::deque<qint16> ring_buffer;

    int NTSC_periods[16]{4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
};

#endif // APU_H
