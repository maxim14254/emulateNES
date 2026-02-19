#ifndef APU_H
#define APU_H

#include <QIODevice>



struct Envelope
{
    bool loop = false;              // он же length_halt для pulse/noise
    bool constant_volume = false;   // D4
    uint8_t period = 0;             // D0..D3 (если constant_volume=0)
    uint8_t constant = 0;           // D0..D3 (если constant_volume=1)

    // внутреннее состояние огибающей
    uint8_t divider = 0;
    uint8_t decay_level = 0;
    bool start = false;

    uint8_t envelope_volume()
    {
        if (constant_volume)
            return constant & 0x0F;
        return decay_level & 0x0F;
    }

    void clock_envelope()
    {
        if (start)
        {
            start = false;
            decay_level = 15;
            divider = period;
            return;
        }

        if (divider == 0)
        {
            divider = period;

            if (decay_level == 0)
            {
                if (loop)
                    decay_level = 15;
            }
            else
            {
                decay_level--;
            }
        }
        else
        {
            divider--;
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
    uint8_t divider = 0;
    bool reload = false;
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

    void run();

private:
    double m_freq = 440.0;
    int m_sampleRate = 48000;
    double m_phase = 0.0;

    PulseBLEP pulse1, pulse2;
    TriangleClean tri;
    NoiseLFSR noise;

    Bus* bus;

    int NTSC_periods[16]{4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
};

#endif // APU_H
