#include "apu.h"
#include "qmath.h"
#include <cmath>
#include "global.h"
#include "bus.h"


APU::APU(double freq, double sampleRate, Bus* _bus, QObject *parent) : QIODevice(parent), m_sampleRate(sampleRate), bus(_bus)
{
    pulse1.sr = m_sampleRate;
    pulse2.sr = m_sampleRate;
    tri.sr = m_sampleRate;
    noise.sr = m_sampleRate;

}

qint64 APU::readData(char *data, qint64 maxlen)
{
    // 16-bit mono
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
            out[i] = last;

    }

   // ring_buffer.clear();
    return samplesToWrite * bytesPerSample;
}

void APU::run(int cycles)
{
    for(int i = 0; i < cycles; ++i)
    {
        ++frame_counter;

        bool bQuarterFrameClock = false;
        bool bHalfFrameClock = false;
        //pulse1.sequencer.reload = bus->get_timer_purse1();

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

//        pulse1.envelope.bLoop = bus->get_envelope_loop_purse1();
//        pulse1.envelope.disable = bus->get_envelope_disable_purse1();
//        pulse1.envelope.start = bus->get_envelope_start_purse1();
//        pulse1.envelope.volume = bus->get_envelope_constant_volume_purse1();

        if (bQuarterFrameClock)
        {
            pulse1.envelope.clock();
            //        pulse2_env.clock(pulse2_halt);
            //        noise_env.clock(noise_halt);
        }

//        pulse1.sweep.enabled = bus->get_sweep_enabled_purse1();
//        pulse1.sweep.period = bus->get_sweep_period_purse1();
//        pulse1.sweep.negate = bus->get_sweep_negate_purse1();
//        pulse1.sweep.shift = bus->get_sweep_shift_purse1();
//        pulse1.sweep.reload = bus->get_sweep_reload_purse1();
//        pulse1.length_counter = bus->get_length_counter_purse1();


        if (bHalfFrameClock)
        {
            pulse1.clock_counter(pulse1_enable.load(), pulse1.envelope.bLoop);
            //        pulse2_lc.clock(pulse2_enable, pulse2_halt);
            //        noise_lc.clock(noise_enable, noise_halt);
            pulse1.sweep.clock(pulse1.sequencer.reload, 0);
            //        pulse2_sweep.clock(pulse2_seq.reload, 1);
        }

        if ((i & 1) == 0)
        {
            pulse1.sequencer.clock(pulse1_enable.load(), [](uint32_t &s)
            {
                s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
            });
        }

        pulse1.freq= 1789773.0 / (16.0 * (double)(pulse1.sequencer.reload + 1.0));
        double amplitude = (double)(pulse1.envelope.output) / 15.0;
       // pulse1.duty = p1.duty.load();

//        pulse2.freq = p2.freq.load();
//        pulse2.duty = p2.duty.load();

//        tri.freq = t.freq.load();

//        noise.noiseFreq = n.noiseFreq.load();
//        noise.shortMode = n.shortMode.load();

        //pulse1_output = 0;
        if(pulse1_enable.load())
        {
            double purse1_sample = 0;
            purse1_sample = 0.18 * amplitude * pulse1.process();

            if (pulse1.length_counter > 0 && pulse1.sequencer.timer >= 8 && !pulse1.sweep.mute && pulse1.envelope.output > 2)
                pulse1_output += (purse1_sample - pulse1_output) * 0.5;
            else
                pulse1_output = 0;

            //pulse1_output *= 0.2;
        }


        double s = pulse1_output; /*+
            0.14 * pulse2.process() +
            0.10 * tri.process() +
            0.03 * noise.process();*/

        // мягкий клип, чтобы не хрустело
        s = std::tanh(1.5 * s);

        sampleAccum += m_sampleRate;

        while (sampleAccum >= 1789773.0)
        {
            sampleAccum -= 1789773.0;

            // посчитать mix 's' и записать ОДИН семпл
            qint16 out_sample = (qint16)std::lrint(s * 32767.0);
            ring_buffer.write(out_sample, 1);
        }
    }



//    p2.freq = 1789773.0 / (16.0 * (double)(bus->get_timer_purse2() + 1.0));
//    t.freq = 1789773.0 / (32.0 * (double)(bus->get_timer_triangle() + 1.0));
//    n.noiseFreq = 1789773.0 / (double)(NTSC_periods[bus->get_noise_period()]);
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

    buf[h] = val;

    if (next == t)
    {
        size_t nt = t + 1;

        if (nt == cap)
            nt = 0;

        tail.store(nt, std::memory_order_release);
    }

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
    const size_t h = head.load(std::memory_order_acquire);
    const size_t t = tail.load(std::memory_order_acquire);

    if (h >= t)
        return h - t;

    return cap - (t - h);
}
