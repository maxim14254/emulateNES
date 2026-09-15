// Private oscillators used by Nes_Apu

// Nes_Snd_Emu 0.1.8
#ifndef NES_OSCS_H
#define NES_OSCS_H


#include "blargg_common.h"
#include "Blip_Buffer.h"



typedef int32_t nes_time_t; // CPU clock cycle count
typedef unsigned nes_addr_t;

class APU;

struct Nes_Osc
{
	unsigned char regs [4];
	bool reg_written [4];
	Blip_Buffer* output;
	int length_counter;// length counter (0 if unused by oscillator)
	int delay;      // delay until next (potential) transition
	int last_amp;   // last amplitude oscillator was outputting

	void clock_length( int halt_mask );
	int period() const {
		return (regs [3] & 7) * 0x100 + (regs [2] & 0xFF);
	}
	void reset() {
		delay = 0;
		last_amp = 0;
	}
	int update_amp( int amp ) {
		int delta = amp - last_amp;
		last_amp = amp;
		return delta;
	}

};

struct Nes_Envelope : Nes_Osc
{
	int envelope;
	int env_delay;

	void clock_envelope();
	int volume() const;
	void reset() {
		envelope = 0;
		env_delay = 0;
		Nes_Osc::reset();
	}

};

// Nes_Square
struct Nes_Square : Nes_Envelope
{
	enum { negate_flag = 0x08 };
	enum { shift_mask = 0x07 };
	enum { phase_range = 8 };
	int phase;
	int sweep_delay;

    typedef Blip_Synth<blip_good_quality,1> Synth;
	Synth const& synth; // shared between squares

	Nes_Square( Synth const* s ) : synth( *s ) { }

	void clock_sweep( int adjust );
	void run( nes_time_t, nes_time_t );
	void reset() {
		sweep_delay = 0;
		Nes_Envelope::reset();
	}
	nes_time_t maintain_phase( nes_time_t time, nes_time_t end_time,
			nes_time_t timer_period );

    friend QDataStream &operator<<(QDataStream &out, const Nes_Square &apu);
    friend QDataStream &operator>>(QDataStream &in, Nes_Square &apu);
};
inline QDataStream &operator<<(QDataStream &out, const Nes_Square &apu)
{
    for(int i = 0; i < 4; ++i)
        out << apu.regs[i];
    for(int i = 0; i < 4; ++i)
        out << apu.reg_written[i];
    out << apu.length_counter;
    out << apu.delay;
    out << apu.last_amp;

    out << apu.envelope;
    out << apu.env_delay;

    out << apu.phase;
    out << apu.sweep_delay;

    return out;
}
inline QDataStream &operator>>(QDataStream &in, Nes_Square &apu)
{
    for(int i = 0; i < 4; ++i)
        in >> apu.regs[i];
    for(int i = 0; i < 4; ++i)
        in >> apu.reg_written[i];
    in >> apu.length_counter;
    in >> apu.delay;
    in >> apu.last_amp;

    in >> apu.envelope;
    in >> apu.env_delay;

    in >> apu.phase;
    in >> apu.sweep_delay;


    return in;
}

// Nes_Triangle
struct Nes_Triangle : Nes_Osc
{
	enum { phase_range = 16 };
	int phase;
	int linear_counter;
    Blip_Synth<blip_med_quality,1> synth;

	int calc_amp() const;
	void run( nes_time_t, nes_time_t );
	void clock_linear_counter();
	void reset() {
		linear_counter = 0;
		phase = 1;
		Nes_Osc::reset();
    }
	nes_time_t maintain_phase( nes_time_t time, nes_time_t end_time,
			nes_time_t timer_period );

    friend QDataStream &operator<<(QDataStream &out, const Nes_Triangle &apu);
    friend QDataStream &operator>>(QDataStream &in, Nes_Triangle &apu);
};
inline QDataStream &operator<<(QDataStream &out, const Nes_Triangle &apu)
{
    for(int i = 0; i < 4; ++i)
        out << apu.regs[i];
    for(int i = 0; i < 4; ++i)
        out << apu.reg_written[i];
    out << apu.length_counter;
    out << apu.delay;
    out << apu.last_amp;

    out << apu.phase;
    out << apu.linear_counter;


    return out;
}
inline QDataStream &operator>>(QDataStream &in, Nes_Triangle &apu)
{
    for(int i = 0; i < 4; ++i)
        in >> apu.regs[i];
    for(int i = 0; i < 4; ++i)
        in >> apu.reg_written[i];
    in >> apu.length_counter;
    in >> apu.delay;
    in >> apu.last_amp;

    in >> apu.phase;
    in >> apu.linear_counter;


    return in;
}

// Nes_Noise
struct Nes_Noise : Nes_Envelope
{
	int noise;
    Blip_Synth<blip_med_quality,1> synth;

	void run( nes_time_t, nes_time_t );
	void reset() {
		noise = 1 << 14;
		Nes_Envelope::reset();
    }

    friend QDataStream &operator<<(QDataStream &out, const Nes_Noise &apu);
    friend QDataStream &operator>>(QDataStream &in, Nes_Noise &apu);
};
inline QDataStream &operator<<(QDataStream &out, const Nes_Noise &apu)
{
    for(int i = 0; i < 4; ++i)
        out << apu.regs[i];
    for(int i = 0; i < 4; ++i)
        out << apu.reg_written[i];
    out << apu.length_counter;
    out << apu.delay;
    out << apu.last_amp;

    out << apu.envelope;
    out << apu.env_delay;

    out << apu.noise;

    return out;
}
inline QDataStream &operator>>(QDataStream &in, Nes_Noise &apu)
{
    for(int i = 0; i < 4; ++i)
        in >> apu.regs[i];
    for(int i = 0; i < 4; ++i)
        in >> apu.reg_written[i];
    in >> apu.length_counter;
    in >> apu.delay;
    in >> apu.last_amp;

    in >> apu.envelope;
    in >> apu.env_delay;

    in >> apu.noise;


    return in;
}

// Nes_Dmc
struct Nes_Dmc : Nes_Osc
{
	int address;    // address of next byte to read
	int period;
	//int length_counter; // bytes remaining to play (already defined in Nes_Osc)
	int buf;
	int bits_remain;
	int bits;
	bool buf_full;
	bool silence;

	enum { loop_flag = 0x40 };

	int dac;

	nes_time_t next_irq;
	bool irq_enabled;
	bool irq_flag;
	bool pal_mode;
	bool nonlinear;

	int (*prg_reader)( void*, nes_addr_t ); // needs to be initialized to prg read function
	void* prg_reader_data;

    APU* apu;

    Blip_Synth<blip_med_quality,1> synth;

	void start();
	void write_register( int, int );
	void run( nes_time_t, nes_time_t );
	void recalc_irq();
	void fill_buffer();
	void reload_sample();
	void reset();
	int count_reads( nes_time_t, nes_time_t* ) const;
    nes_time_t next_read_time() const;

    friend QDataStream &operator<<(QDataStream &out, const Nes_Dmc &apu);
    friend QDataStream &operator>>(QDataStream &in, Nes_Dmc &apu);
};
inline QDataStream &operator<<(QDataStream &out, const Nes_Dmc &apu)
{
    for(int i = 0; i < 4; ++i)
        out << apu.regs[i];
    for(int i = 0; i < 4; ++i)
        out << apu.reg_written[i];
    out << apu.length_counter;
    out << apu.delay;
    out << apu.last_amp;

    out << apu.address;
    out << apu.period;
    out << apu.buf;
    out << apu.bits_remain;
    out << apu.bits;
    out << apu.buf_full;
    out << apu.silence;
    out << apu.dac;
    out << apu.next_irq;
    out << apu.irq_enabled;
    out << apu.irq_flag;
    out << apu.pal_mode;


    return out;
}
inline QDataStream &operator>>(QDataStream &in, Nes_Dmc &apu)
{
    for(int i = 0; i < 4; ++i)
        in >> apu.regs[i];
    for(int i = 0; i < 4; ++i)
        in >> apu.reg_written[i];
    in >> apu.length_counter;
    in >> apu.delay;
    in >> apu.last_amp;

    in >> apu.address;
    in >> apu.period;
    in >> apu.buf;
    in >> apu.bits_remain;
    in >> apu.bits;
    in >> apu.buf_full;
    in >> apu.silence;
    in >> apu.dac;
    in >> apu.next_irq;
    in >> apu.irq_enabled;
    in >> apu.irq_flag;
    in >> apu.pal_mode;


    return in;
}

#endif
