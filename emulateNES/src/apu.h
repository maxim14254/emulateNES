#ifndef APU_H
#define APU_H

#include <QIODevice>
#include <deque>
#include "Nes_Oscs.h"
#include <QtMultimedia/QAudioOutput>


class RingBufferSPSC
{

public:
    RingBufferSPSC(size_t capacity);

    bool write(qint16 val, size_t n);
    bool read(qint16* val, size_t n);
    size_t bytesAvailable() const;
    qint16 next();

private:
    std::vector<qint16> buf;
    size_t cap = 0;
    std::atomic<size_t> head {0}; // write index
    std::atomic<size_t> tail {0}; // read index

    size_t availableToRead() const;
    size_t availableToWrite() const;

};


static int const amp_range = 15;

class Bus;

class APU
{

public:
    explicit APU(double sampleRate, Bus* _bus, QAudioOutput* sink);


    void run(uint64_t cycles);
    void write_registers(uint16_t addr, uint8_t data);
    uint8_t read_status();
    void set_tempo( double t );
    void output( Blip_Buffer* buffer );
    void irq_changed();
    void enable_nonlinear( double v );
    void run_(uint64_t cycles);
    nes_time_t next_dmc_read_cycles() const;
    void end_frame(uint64_t cycles, uint64_t old_cycles);


    static const unsigned int no_irq = INT_MAX / 2 + 1;
    int last_dmc_cycles = 0;
    int osc_enables = 0;

private:

    double m_sampleRate = 48000.0;
    double sampleAccum = 0.0;
    qint16 last = 0;
    uint8_t status;

    int last_cycles = 0;
    nes_time_t next_irq = 0;
    nes_time_t earliest_irq_;
    uint32_t frame_delay = 1;
    int frame_period = 0;

    double tempo_ = 1.0;

    Nes_Osc* oscs [5];
    Nes_Square square1;
    Nes_Square square2;
    Nes_Noise noise;
    Nes_Triangle triangle;
    Nes_Dmc dmc;
    Nes_Square::Synth square_synth;
    Blip_Buffer blip;

    Bus* bus;
    QAudioOutput* sink;
    QIODevice* audioDev = nullptr;
    int frame_counter = 0;
    RingBufferSPSC ring_buffer{13500};

    double pulse1_output = 0;
    double pulse2_output = 0;
    uint8_t noise_output = 0;
    uint8_t triangle_output = 0;


    bool pulse1_enable = false;
    bool pulse2_enable = false;
    bool noise_enable = false;
    bool triangle_enable = false;
    uint8_t frame_mode_5step = false;
    bool irq_flag = false;
    bool frame_irq_flag = false;

    QVector<qint16> temp;

    double mix_nes(double p1, double p2, double t, double n);
    void pump_audio();
    void volume(double v);
    void reset(bool pal_mode = false, int initial_dmc_dac = 0);
    void treble_eq( const blip_eq_t& eq );
    void update_irq_line();
    static double nonlinear_tnd_gain() { return 0.75; }

    uint8_t LENGTH_TABLE[32] =
    {
        10,254,20, 2,40, 4,80, 6,160, 8,60,10,14,12,26,14,
        12,16,24,18,48,20,96,22,192,24,72,26,16,28,32,30
    };

    inline void osc_output(int osc, Blip_Buffer* buf)
    {
        assert((unsigned) osc < 5);
        oscs[osc]->output = buf;
    }

};

inline nes_time_t Nes_Dmc::next_read_time() const
{
    if ( length_counter == 0 )
        return APU::no_irq; // not reading

    return apu->last_dmc_cycles + delay + long (bits_remain - 1) * period;
}

inline nes_time_t APU::next_dmc_read_cycles() const { return dmc.next_read_time(); }

#endif // APU_H
