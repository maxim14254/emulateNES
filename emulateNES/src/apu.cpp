#include "apu.h"
#include <cmath>
#include "global.h"
#include "bus.h"
#include "QDebug"



APU::APU(double freq, double sampleRate, Bus* _bus, QAudioOutput* _sink, QObject *parent)
    : QIODevice(parent),
      m_sampleRate(sampleRate),
      bus(_bus),
      sink(_sink)
{
    pulse2.sr = pulse1.sr = 1789773.0;
    onePoleLPF.setCutoff(14000.0, m_sampleRate);
}

qint64 APU::readData(char *data, qint64 maxlen)
{
     if (maxlen <= 0)
         return 0;

    int bytesPerSample = 2;
    qint64 samplesToWrite = maxlen / bytesPerSample;
    qint16* out = reinterpret_cast<qint16*>(data);

    for (qint64 i = 0; i < samplesToWrite; ++i)
    {
        qint16 v;
        if(ring_buffer.read(&v, 1))
        {
            last = v;
            out[i] = v;
        }
        else
            out[i] = 0;

    }

    return samplesToWrite * bytesPerSample;
}

void APU::run(int cycles)
{
    for(int i = 0; i < cycles; ++i)
    {
        ++frame_counter;

        bool bQuarterFrameClock = false;
        bool bHalfFrameClock = false;

        if (frame_counter == 3729)
        {
            bQuarterFrameClock = true;
        }
        else if (frame_counter == 7457)
        {
            bQuarterFrameClock = true;
            bHalfFrameClock = true;
        }
        else if (frame_counter == 11186)
        {
            bQuarterFrameClock = true;
        }
        else if (frame_counter == 14916)
        {
            bQuarterFrameClock = true;
            bHalfFrameClock = true;
            frame_counter = 0;
        }

        if (bQuarterFrameClock)
        {
            pulse1.envelope.clock();
            pulse2.envelope.clock();
            noise.envelope.clock();
            triangle.clock_linear();
        }

        if (bHalfFrameClock)
        {
            pulse1.clock_counter(pulse1_enable, pulse1.envelope.bLoop);
            pulse2.clock_counter(pulse2_enable, pulse2.envelope.bLoop);
            noise.clock_counter(noise_enable, noise.envelope.bLoop);
            triangle.clock_counter(triangle_enable, triangle.control_flag);

            pulse1.sweep.clock(pulse1.sequencer.reload, 0);
            pulse2.sweep.clock(pulse2.sequencer.reload, 0);
        }

        if ((i & 1) == 0)
        {
            pulse1.sequencer.clock(pulse1_enable, [](uint32_t &s)
            {
                s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
            });

            pulse2.sequencer.clock(pulse2_enable, [](uint32_t &s)
            {
                s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
            });
        }

        pulse1.freq = 1789773.0 / (16.0 * (double)(pulse1.sequencer.reload + 1.0));
        double amplitude1 = (double)(pulse1.envelope.output) / 15.0;

        pulse2.freq = 1789773.0 / (16.0 * (double)(pulse2.sequencer.reload + 1.0));
        double amplitude2 = (double)(pulse2.envelope.output) / 15.0;

        if(pulse1_enable)
        {
            double pulse1_sample = 0;
            pulse1_sample = amplitude1 * pulse1.process();

            if (pulse1.length_counter > 0 && pulse1.sequencer.timer >= 8 && !pulse1.sweep.mute && pulse1.envelope.output > 2)
                pulse1_output += (pulse1_sample - pulse1_output) * 0.5;
            else
                pulse1_output = (0.0 - pulse1_output) * 0.01;
        }

        if(pulse2_enable)
        {
            double pulse2_sample = 0;
            pulse2_sample = amplitude2 * pulse2.process();

            if (pulse2.length_counter > 0 && pulse2.sequencer.timer >= 8 && !pulse2.sweep.mute && pulse2.envelope.output > 2)
                pulse2_output += (pulse2_sample - pulse2_output) * 0.5;
            else
                pulse2_output = (0.0 - pulse2_output) * 0.01;
        }

        if(noise_enable)
        {
            noise.clock_timer();
            noise_output = noise.output() ;
        }
        else
            noise_output = 0;

        if(triangle_enable)
        {
            triangle.clock_timer();
            triangle_output = triangle.output();
        }
        else
            triangle_output = 0;


        double p1_level = (pulse1_output * 0.5 + 0.5) * 15.0;
        double p2_level = (pulse2_output * 0.5 + 0.5) * 15.0;

        double s = mix_nes(p1_level, p2_level, triangle_output, noise_output);
        s = onePoleLPF.process(s);

        s = std::tanh(0.8 * s);

        sampleAccum += m_sampleRate;

        while (sampleAccum >= 1789773.0)
        {
            sampleAccum -= 1789773.0;

            qint16 out_sample = (qint16)std::lrint(s * 32767.0);
            ring_buffer.write(out_sample, 1);
        }
    }
}

double APU::mix_nes(uint8_t p1, uint8_t p2, uint8_t t, uint8_t n)
{
    double pulse = 0.0;

    if (p1 || p2)
        pulse = 95.88 / ((8128.0 / (p1 + p2)) + 100.0);

    double tnd = 0.0;
    double tnd_in = (t / 8227.0) + (n / 12241.0);

    if (tnd_in > 0.0)
        tnd = 159.79 / ((1.0 / tnd_in) + 100.0);

    return pulse + tnd;
}





RingBufferSPSC::RingBufferSPSC(size_t capacity) : buf(capacity), cap(capacity)
{

}

bool RingBufferSPSC::write(qint16 val, size_t n)
{

    size_t h = head.load(std::memory_order_relaxed);
    size_t t = tail.load(std::memory_order_acquire);

    size_t next = h + 1;

    if (next == cap)
        next = 0;

    if (next == t)
    {
        //return false;
        size_t nt = t + 1;

        if (nt == cap)
            nt = 0;

        tail.store(nt, std::memory_order_release);
    }

    buf[h] = val;
    head.store(next, std::memory_order_release);

    return true;
}

bool RingBufferSPSC::read(qint16* val, size_t n)
{
    size_t h = head.load(std::memory_order_acquire);
    size_t t = tail.load(std::memory_order_relaxed);

    if (t == h)
        return false;

    *val = buf[t];

    size_t next = t + 1;

    if (next == cap)
        next = 0;

    tail.store(next, std::memory_order_release);

    return true;
}

size_t RingBufferSPSC::bytesAvailable() const
{
    return availableToRead() * sizeof(qint16);
}

size_t RingBufferSPSC::availableToWrite() const
{
    return cap - availableToRead();
}

size_t RingBufferSPSC::availableToRead() const
{
    size_t h = head.load(std::memory_order_acquire);
    size_t t = tail.load(std::memory_order_acquire);

    if (h >= t)
        return h - t;

    return cap - (t - h);
}
