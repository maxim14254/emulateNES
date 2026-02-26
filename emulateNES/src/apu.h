#ifndef APU_H
#define APU_H

#include <QIODevice>
#include <deque>
#include "global.h"



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


class RingBufferSPSC
{

public:
    RingBufferSPSC(size_t capacity);

    bool write(qint16 val, size_t n);
    bool read(qint16* val, size_t n);
    size_t bytesAvailable() const;

private:
    std::vector<qint16> buf;
    const size_t cap = 0;
    std::atomic<size_t> head {0}; // write index
    std::atomic<size_t> tail {0}; // read index

    size_t availableToRead() const;
    size_t availableToWrite() const;

};


class Bus;

class APU : public QIODevice
{
    Q_OBJECT

public:
    explicit APU(double freq, double sampleRate, Bus* _bus, QObject *parent = nullptr);

    qint64 readData(char *data, qint64 maxlen) override;

    void start() { open(QIODevice::ReadOnly); }
    void stop()  { close(); }

    qint64 writeData(const char *, qint64) override { return 0; }
    qint64 bytesAvailable() const override { return static_cast<qint64>(ring_buffer.bytesAvailable()) + QIODevice::bytesAvailable(); }

    void run(int cycles);

private:
    double m_sampleRate = 48000.0;
    double sampleAccum = 0.0;
    qint16 last = 0;

    //PulseBLEP pulse1, pulse2;
    TriangleClean tri;
    NoiseLFSR noise;

    Bus* bus;

    int frame_counter = 0;
    RingBufferSPSC ring_buffer{48000};
    double pulse1_output = 0;

    int NTSC_periods[16]{4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
};

#endif // APU_H
