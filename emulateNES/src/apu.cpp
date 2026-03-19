#include "apu.h"
#include <cmath>
#include "global.h"
#include "bus.h"
#include "QDebug"
#include "cpu.h"
#include "blargg_source.h"


int samplesToWrite = 0;

int prg_reader(void* _prg_reader_data, nes_addr_t addr)
{
    Bus* bus = static_cast<Bus*>(_prg_reader_data);
    return bus->read_cpu(addr, false);
}

APU::APU(double sampleRate, Bus* _bus, QAudioOutput* _sink)
    : m_sampleRate(sampleRate),
      bus(_bus),
      sink(_sink),
      square1( &square_synth ),
      square2( &square_synth )
{
    tempo_ = 1.0;
    dmc.apu = this;
    dmc.prg_reader = prg_reader;
    dmc.prg_reader_data = bus;

    oscs [0] = &square1;
    oscs [1] = &square2;
    oscs [2] = &triangle;
    oscs [3] = &noise;
    oscs [4] = &dmc;

    output( NULL );
    volume( 1.0 );
    reset( false );

    blip.set_sample_rate((long)m_sampleRate, 250);
    blip.clock_rate(1789773);
    output(&blip);

    audioDev = sink->start();
    qint64 freeBytes = sink->bytesFree();
    qint64 bytesToWrite = freeBytes & ~qint64(1);
    if (bytesToWrite <= 0)
        return;

    samplesToWrite = int(bytesToWrite / 2);

    temp.resize(samplesToWrite);
}

void APU::run_(uint64_t cycles)
{
    require( cycles >= last_dmc_cycles );
    if ( cycles > next_dmc_read_cycles() )
    {
        nes_time_t start = last_dmc_cycles;
        last_dmc_cycles = cycles;
        dmc.run( start, cycles );
    }
}

template<class T>
inline void zero_apu_osc( T* osc, nes_time_t time )
{
    Blip_Buffer* output = osc->output;
    int last_amp = osc->last_amp;
    osc->last_amp = 0;
    if ( output && last_amp )
        osc->synth.offset( time, -last_amp, output );
}

void APU::end_frame(uint64_t cycles, uint64_t old_cycles)
{
    uint64_t delta = cycles - old_cycles;
    if ( delta > last_cycles )
        run( delta );

    if ( dmc.nonlinear )
    {
        zero_apu_osc(&square1, last_cycles );
        zero_apu_osc(&square2, last_cycles );
        zero_apu_osc(&triangle, last_cycles );
        zero_apu_osc(&noise, last_cycles );
        zero_apu_osc(&dmc, last_cycles );
    }

    // make times relative to new frame
    last_cycles -= delta;
    require( last_cycles >= 0 );

    last_dmc_cycles -= delta;
    require( last_dmc_cycles >= 0 );

    if ( next_irq != no_irq )
    {
        next_irq -= delta;
        check( next_irq >= 0 );
    }
    if ( dmc.next_irq != no_irq )
    {
        dmc.next_irq -= delta;
        check( dmc.next_irq >= 0 );
    }
    if ( earliest_irq_ != no_irq )
    {
        earliest_irq_ -= delta;
        if ( earliest_irq_ < 0 )
            earliest_irq_ = 0;
    }

    blip.end_frame(delta);

    pump_audio();
}

void APU::run(uint64_t cycles)
{
    require(cycles >= last_cycles);

    if (cycles == last_cycles)
        return;

    if (last_dmc_cycles < cycles)
    {
        uint64_t start = last_dmc_cycles;
        last_dmc_cycles = cycles;
        dmc.run( start, cycles );
    }

    while (true)
    {
        uint64_t time = last_cycles + frame_delay;
        if (time > cycles)
            time = cycles;
        frame_delay -= time - last_cycles;

        square1.run( last_cycles, time );
        square2.run( last_cycles, time );
        triangle.run( last_cycles, time );
        noise.run( last_cycles, time );
        last_cycles = time;

        if ( time == cycles )
            break;

        frame_delay = frame_period;
        switch ( ++frame_counter )
        {
            case 0:
                if ((frame_mode_5step & 0xC0) == 0)
                {
                    next_irq = time + frame_period * 4 + 2;
                    irq_flag = true;
                    update_irq_line();
                }
            case 2:
                square1.clock_length(0x20);
                square2.clock_length(0x20);
                noise.clock_length(0x20);
                triangle.clock_length(0x80);

                square1.clock_sweep(-1);
                square2.clock_sweep(0);

                if (dmc.pal_mode && frame_counter == 3)
                    frame_delay -= 2;
                break;
            case 1:
                if (!dmc.pal_mode)
                    frame_delay -= 2;
                break;

            case 3:
                frame_counter = 0;

                if (frame_mode_5step & 0x80)
                    frame_delay += frame_period - (dmc.pal_mode ? 2 : 6);
                break;
        }

        triangle.clock_linear_counter();
        square1.clock_envelope();
        square2.clock_envelope();
        noise.clock_envelope();
    }
}

void APU::treble_eq( const blip_eq_t& eq )
{
    square_synth.treble_eq(eq);
    triangle.synth.treble_eq(eq);
    noise.synth.treble_eq(eq);
    dmc.synth.treble_eq(eq);
}

void APU::update_irq_line()
{
    bool level = irq_flag || dmc.irq_flag;
    bus->set_apu_irq(level);
}

void APU::enable_nonlinear( double v )
{
    dmc.nonlinear = true;
    square_synth.volume( 1.3 * 0.25751258 / 0.742467605 * 0.25 / amp_range * v );

    const double tnd = 0.48 / 202 * nonlinear_tnd_gain();
    triangle.synth.volume( 3.0 * tnd );
    noise.synth.volume( 2.0 * tnd );
    dmc.synth.volume( tnd );

    square1.last_amp = 0;
    square2.last_amp = 0;
    triangle.last_amp = 0;
    noise.last_amp = 0;
    dmc.last_amp = 0;
}

void APU::write_registers(uint16_t addr, uint8_t data)
{
    require( addr > 0x20 );
    require( (unsigned) data <= 0xFF );

    uint64_t cycles = 0;
    //run(cycles);

    if ( addr < 0x4014 )
    {
        // Write to channel
        int osc_index = (addr - 0x4000) >> 2;
        Nes_Osc* osc = oscs [osc_index];

        int reg = addr & 3;
        osc->regs [reg] = data;
        osc->reg_written [reg] = true;

        if ( osc_index == 4 )
        {
            // handle DMC specially
            dmc.write_register( reg, data );
        }
        else if ( reg == 3 )
        {
            // load length counter
            if ((osc_enables >> osc_index) & 1)
                osc->length_counter = LENGTH_TABLE [(data >> 3) & 0x1F];

            // reset square phase
            if (osc_index < 2)
                ((Nes_Square*) osc)->phase = Nes_Square::phase_range - 1;
        }
    }
    else if (addr == 0x4015)
    {
        // Channel enables
        for ( int i = 5; i--; )
            if ( !((data >> i) & 1) )
                oscs [i]->length_counter = 0;

        bool recalc_irq = dmc.irq_flag;
        dmc.irq_flag = false;

        int old_enables = osc_enables;
        osc_enables = data;
        if ( !(data & 0x10) ) {
            dmc.next_irq = no_irq;
            recalc_irq = true;
        }
        else if ( !(old_enables & 0x10) ) {
            dmc.start(); // dmc just enabled
        }

        if ( recalc_irq )
            update_irq_line();
    }
    else if (addr == 0x4017)
    {
        frame_mode_5step = data;

        bool irq_enabled = !(data & 0x40);
        irq_flag &= irq_enabled;
        next_irq = no_irq;

        frame_delay = (frame_delay & 1);
        frame_counter = 0;

        if ( !(data & 0x80) )
        {
            // mode 0
            frame_counter = 1;
            frame_delay += frame_period;
            if ( irq_enabled )
                next_irq = cycles + frame_delay + frame_period * 3 + 1;
        }

        update_irq_line();;

    }
}

uint8_t APU::read_status()
{
    uint8_t result = (dmc.irq_flag << 7) | (irq_flag << 6);

    for ( int i = 0; i < 5; i++ )
        if ( oscs [i]->length_counter )
            result |= 1 << i;

    if ( irq_flag )
    {
        result |= 0x40;
        irq_flag = false;
        update_irq_line();
    }

    return result;
}

void APU::set_tempo(double t)
{
    tempo_ = t;
    frame_period = 7458;

    if ( t != 1.0 )
        frame_period = (int) (frame_period / t) & ~1;
}

void APU::output(Blip_Buffer *buffer)
{
    for ( int i = 0; i < 5; ++i )
        osc_output(i, buffer);
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

void APU::pump_audio()
{
    if (!audioDev)
        return;

    int avail = (int)blip.samples_avail();
    if (avail <= 0)
        return;

    int toRead = qMin(avail, temp.size());
    long got = blip.read_samples(reinterpret_cast<blip_sample_t*>(temp.data()), toRead, 0);

    if (got > 0)
        audioDev->write(reinterpret_cast<const char*>(temp.constData()), got * sizeof(qint16));
}

void APU::irq_changed()
{
    nes_time_t new_irq = dmc.next_irq;

    if (dmc.irq_flag || irq_flag)
        new_irq = 0;
    else if (next_irq < new_irq)
        new_irq = next_irq;

    if (new_irq != earliest_irq_)
    {
        earliest_irq_ = new_irq;
        update_irq_line();
    }
}

void APU::volume(double v)
{
    dmc.nonlinear = false;
    square_synth.volume(   0.1128  / amp_range * v );
    triangle.synth.volume( 0.12765 / amp_range * v );
    noise.synth.volume(    0.0741  / amp_range * v );
    dmc.synth.volume(      0.42545 / 127 * v );
}

void APU::reset(bool pal_mode, int initial_dmc_dac)
{
    dmc.pal_mode = pal_mode;
    set_tempo( tempo_ );

    square1.reset();
    square2.reset();
    triangle.reset();
    noise.reset();
    dmc.reset();

    last_cycles = 0;
    last_dmc_cycles = 0;
    osc_enables = 0;
    irq_flag = false;
    earliest_irq_ = no_irq;
    frame_delay = 1;
    write_registers(0x4017, 0x00 );
    write_registers(0x4015, 0x00 );

    for ( nes_addr_t addr = 0x4000; addr <= 0x4013; addr++ )
        write_registers(addr, (addr & 3) ? 0x00 : 0x10 );

    dmc.dac = initial_dmc_dac;
    if ( !dmc.nonlinear )
        triangle.last_amp = 15;
    if ( !dmc.nonlinear ) // TODO: remove?
        dmc.last_amp = initial_dmc_dac; // prevent output transition
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
        return false;
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
