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

public:
    explicit APU(double freq, double sampleRate, Bus* _bus, QAudioOutput* sink, QObject *parent = nullptr);

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

    Bus* bus;
    QAudioOutput* sink;

    int frame_counter = 0;
    RingBufferSPSC ring_buffer{13500};
    OnePoleLPF onePoleLPF;

    double pulse1_output = 0;
    double pulse2_output = 0;
    uint8_t noise_output = 0;
    uint8_t triangle_output = 0;

    double mix_nes(uint8_t p1, uint8_t p2, uint8_t t, uint8_t n);

};

#endif // APU_H
