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

    QString line;
    line.reserve(int(samplesToWrite) * 7); // грубо, чтобы меньше реаллокаций

    for (qint64 i = 0; i < samplesToWrite; ++i)
    {
        qint16 v;
        if(ring_buffer.read(&v, 1))
        {
            qint16 next = ring_buffer.next();

            if(v != last && next == last)
                v = last;

            last = v;
            out[i] = v;

            line += QString::number(v);
            line += ' ';
        }
        else
            out[i] = 0;
    }

    qDebug().noquote() << line;

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
            pulse2.sweep.clock(pulse2.sequencer.reload, 1);
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

        double p1_level = 0.0;
        if (pulse1_enable)
        {
            p1_level = amplitude1 * pulse1.process();

//            if (pulse1.length_counter > 0 && pulse1.sequencer.timer >= 8 && !pulse1.sweep.mute && pulse1.envelope.output > 2)
//            {
//                pulse1_output += (pulse1_sample - pulse1_output) * 0.5;
//                // Переводим из [-amp, +amp] → [0, 15]
//                p1_level = (pulse1_output * 0.5 + 0.5) * 15.0;
//                p1_level = std::clamp(p1_level, 0.0, 15.0);  // double, не int!
//            }
//            else
//            {
//                pulse1_output += (0.0 - pulse1_output) * 0.01;  // += !
//            }
        }

        double p2_level = 0.0;
        if (pulse2_enable)
        {
            p2_level = amplitude2 * pulse2.process();
//            if (pulse2.length_counter > 0 && pulse2.sequencer.timer >= 8 && !pulse2.sweep.mute && pulse2.envelope.output > 2)
//            {
//                pulse2_output += (pulse2_sample - pulse2_output) * 0.5;
//                p2_level = (pulse2_output * 0.5 + 0.5) * 15.0;
//                p2_level = std::clamp(p2_level, 0.0, 15.0);
//            }
//            else
//            {
//                pulse2_output += (0.0 - pulse2_output) * 0.01;
//            }
        }

        if(noise_enable)
        {
            noise.clock_timer();
            noise_output = noise.output();
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

        //double s = mix_nes(p1_level, p2_level, triangle_output, noise_output);
//        double p1 = pulse1_blep_sample * (envelope1.output / 15.0);
//        double p2 = pulse2_blep_sample * (envelope2.output / 15.0);
        double tri = (triangle.output() / 7.5) - 1.0;  // [0..15]→[-1..+1]
        double noi = (noise.output()   / 7.5) - 1.0;

        // Простой линейный микс с примерными весами NES:
        double s = 0.26 * (p1_level + p2_level) + 0.20 * tri + 0.15 * noi;

        s = onePoleLPF.process(s);
        s = std::tanh(0.8 * s);

        sampleAccum += m_sampleRate;

        while (sampleAccum >= 1789773.0)
        {
            sampleAccum -= 1789773.0;

            qint16 out_sample = (qint16)std::lrint(s * 32766.0);

            ring_buffer.write(out_sample, 1);
        }
    }
}

void APU::write_registers(uint16_t addr, uint8_t data)
{
    if(addr == 0x4000)
    {
        switch ((data & 0xC0) >> 6)
        {
        case 0x00:
            pulse1.duty = 0.125;
            pulse1.sequencer.sequence = 0b01000000;
            break;
        case 0x01:
            pulse1.duty = 0.25;
            pulse1.sequencer.sequence = 0b01100000;
            break;
        case 0x02:
            pulse1.duty = 0.5;
            pulse1.sequencer.sequence = 0b01111000;
            break;
        case 0x03:
            pulse1.duty = 0.75;
            pulse1.sequencer.sequence = 0b10011111;
            break;
        }

        pulse1.envelope.bLoop = (data & 0x20) > 0;
        pulse1.envelope.disable = (data & 0x10) > 0;
        pulse1.envelope.volume = (data & 0x0F);
    }
    else if(addr == 0x4001)
    {
        pulse1.sweep.enabled = (data & 0x80) > 0;
        pulse1.sweep.period = (data & 0x70) >> 4;
        pulse1.sweep.negate = (data & 0x08) > 0;
        pulse1.sweep.shift = data & 0x07;
        pulse1.sweep.reload = true;
    }
    else if(addr == 0x4002)
    {
        pulse1.sequencer.reload = (pulse1.sequencer.reload & 0xFF00) | data;
    }
    else if(addr == 0x4003)
    {
        pulse1.sequencer.reload = pulse1.sequencer.timer = (data & 0x07) << 8 | (pulse1.sequencer.reload & 0x00FF);;
        uint8_t length = (data & 0xF8) >> 3;

        if (pulse1_enable)
            pulse1.length_counter = LENGTH_TABLE[length];

        pulse1.envelope.start = true;
    }
    else if(addr == 0x4004)
    {
        switch ((data & 0xC0) >> 6)
        {
        case 0x00:
            pulse2.duty = 0.125;
            pulse2.sequencer.sequence = 0b01000000;
            break;
        case 0x01:
            pulse2.duty = 0.25;
            pulse2.sequencer.sequence = 0b01100000;
            break;
        case 0x02:
            pulse2.duty = 0.5;
            pulse2.sequencer.sequence = 0b01111000;
            break;
        case 0x03:
            pulse2.duty = 0.75;
            pulse2.sequencer.sequence = 0b10011111;
            break;
        }

        pulse2.envelope.bLoop = (data & 0x20) > 0;
        pulse2.envelope.disable = (data & 0x10) > 0;
        pulse2.envelope.volume = (data & 0x0F);
    }
    else if(addr == 0x4005)
    {
        pulse2.sweep.enabled = (data & 0x80) > 0;
        pulse2.sweep.period = (data & 0x70) >> 4;
        pulse2.sweep.negate = (data & 0x08) > 0;
        pulse2.sweep.shift = data & 0x07;
        pulse2.sweep.reload = true;
    }
    else if(addr == 0x4006)
    {
        pulse2.sequencer.reload = (pulse2.sequencer.reload & 0xFF00) | data;
    }
    else if(addr == 0x4007)
    {
        pulse2.sequencer.reload = pulse2.sequencer.timer = (data & 0x07) << 8 | (pulse2.sequencer.reload & 0x00FF);
        uint8_t length = (data & 0xF8) >> 3;

        if (pulse2_enable)
            pulse2.length_counter = LENGTH_TABLE[length];

        pulse2.envelope.start = true;;
    }
    else if(addr == 0x4008)
    {
        triangle.control_flag = (data & 0x80) > 0;
        triangle.linear_reload_value = data & 0x7F;
    }
    else if(addr == 0x400A)
    {
        triangle.reload = (triangle.reload & 0xFF00) | data;
    }
    else if(addr == 0x400B)
    {
        triangle.reload = (triangle.reload & 0x00FF) | ((data & 0x07) << 8);
        triangle.timer = triangle.reload;

        uint8_t length = (data & 0xF8) >> 3;

        if (triangle_enable)
            triangle.length_counter = LENGTH_TABLE[length];

         triangle.linear_reload_flag = true;
    }
    else if(addr == 0x400C)
    {
        noise.envelope.bLoop =  (data & 0x20) > 0;
        noise.envelope.disable =  (data & 0x10) > 0;
        noise.envelope.volume = (data & 0x0F);
    }
    else if(addr == 0x400E)
    {
        noise.shortMode = (data & 0x80) > 0;
        noise.periodIndex = data & 0x0F;
    }
    else if(addr == 0x400F)
    {
        if (noise_enable)
            noise.length_counter = LENGTH_TABLE[(data & 0xF8) >> 3];

        noise.envelope.start = true;
    }
    else if(addr == 0x4015)
    {
        pulse1_enable   = data & 0x01;
        pulse2_enable   = data & 0x02;
        triangle_enable = data & 0x04;  // bit 2
        noise_enable    = data & 0x08;  // bit 3
    }
}

double APU::mix_nes(double p1, double p2, double t, double n)
{
    double pulse = 0.0;

    if (p1 + p2 > 0.0)
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

    size_t next = (h + 1 == cap) ? 0 : h + 1;

    if (next == t)
        return false;          // буфер полон — дропаем

    buf[h] = val;
    head.store(next, std::memory_order_release);
    return true;
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

qint16 RingBufferSPSC::next()
{
    size_t t = tail.load(std::memory_order_relaxed);

    size_t next = t + 1;

    if (next == cap)
        next = 0;

    return buf[next];
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
