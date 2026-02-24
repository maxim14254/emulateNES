#include "apu.h"
#include "qmath.h"
#include <cmath>
#include "global.h"
#include "bus.h"


APU::APU(double freq, int sampleRate, Bus* _bus, QObject *parent) : QIODevice(parent), m_freq(freq), m_sampleRate(sampleRate), bus(_bus)
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
        if(ring_buffer.size() > i)
        {
            out[i] = ring_buffer[i];
        }
        else
            out[i] = 0;
    }

    ring_buffer.clear();
    return samplesToWrite * bytesPerSample;
}

void APU::run(int cycles)
{
    for(int i = 0; i < cycles / 2; ++i)
    {
        ++frame_counter;

        bool bQuarterFrameClock = false;
        bool bHalfFrameClock = false;
        uint16_t target = bus->get_timer_purse1();

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
            pulse1.envelope.clock_envelope();
            //        pulse2_env.clock(pulse2_halt);
            //        noise_env.clock(noise_halt);
        }

        pulse1.sweep.enabled = bus->get_sweep_enabled_purse1();
        pulse1.sweep.period = bus->get_sweep_period_purse1();
        pulse1.sweep.negate = bus->get_sweep_negate_purse1();
        pulse1.sweep.shift = bus->get_sweep_shift_purse1();
        pulse1.sweep.reload = bus->get_sweep_reload_purse1();

        if (bHalfFrameClock)
        {
            //        pulse1_lc.clock(pulse1_enable, pulse1_halt);
            //        pulse2_lc.clock(pulse2_enable, pulse2_halt);
            //        noise_lc.clock(noise_enable, noise_halt);
            pulse1.sweep.clock(target, 0);
            //        pulse2_sweep.clock(pulse2_seq.reload, 1);
        }

        p1.freq = 1789773.0 / (16.0 * (double)(target + 1.0));
        p1.gain = (double)(pulse1.envelope.output - 1) / 16.0;

        pulse1.envelope.bLoop = bus->get_envelope_loop_purse1();
        pulse1.envelope.disable = bus->get_envelope_disable_purse1();
        pulse1.envelope.start = bus->get_envelope_start_purse1();
        pulse1.envelope.volume = bus->get_envelope_constant_volume_purse1();

        pulse1.length_counter = bus->get_length_counter_purse1();





        pulse1.freq = p1.freq.load();
        pulse1.duty = p1.duty.load();

//        pulse2.freq = p2.freq.load();
//        pulse2.duty = p2.duty.load();

//        tri.freq = t.freq.load();

//        noise.noiseFreq = n.noiseFreq.load();
//        noise.shortMode = n.shortMode.load();

        double purse1_sample = 0;

        if(pulse1_enable.load())
            purse1_sample = 0.18 * p1.gain * pulse1.process();


        double s = purse1_sample; /*+
            0.14 * pulse2.process() +
            0.10 * tri.process() +
            0.03 * noise.process();*/

        // мягкий клип, чтобы не хрустело
        s = std::tanh(1.5 * s);

        int v = int(std::lrint(s * 32767.0));
        v *= 0.2;

        v = std::clamp(v, -32768, 32767);

        ring_buffer.push_back(qint16(v));
    }



    //    p2.freq = 1789773.0 / (16.0 * (double)(bus->get_timer_purse2() + 1.0));
//    t.freq = 1789773.0 / (32.0 * (double)(bus->get_timer_triangle() + 1.0));
//    n.noiseFreq = 1789773.0 / (double)(NTSC_periods[bus->get_noise_period()]);
}
