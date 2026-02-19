#ifndef BUS_H
#define BUS_H

#include <vector>
#include <memory>


class Cartridge;
class QString;
class PPU;
class CPU;
class APU;

class Bus
{
public:

    Bus();
    ~Bus();

    uint8_t read_cpu(uint16_t addr, bool onlyRead = false);
    uint8_t read_ppu(uint16_t addr);

    void write_cpu(uint16_t addr, uint8_t data);
    void write_ppu(uint16_t addr, uint8_t data);

    void init_new_cartridge(const QString& path, bool* status);
    void init_PPU(PPU* _ppu);
    void init_CPU(CPU* _cpu);
    void init_APU(APU* _cpu);

    void run_steps_ppu(int cycles);
    void run_steps_apu();

    void cpu_request_nmi();
    void reset_ppu();

    void run_watch_all_tiles();
    void run_watch_cpu_instr(uint16_t PC);

    uint16_t get_timer_purse1() { return p1_timer; }
    uint16_t get_timer_purse2() { return p2_timer; }
    uint16_t get_timer_triangle() { return triangle_timer; }
    uint16_t get_noise_period() { return noise_period; }
    bool get_envelope_loop_noise(){ return envelope_loop_noise; }

    uint8_t get_envelope_constant_period_purse1(){ return envelope_constant_period_purse1; }
    bool get_envelope_loop_purse1(){ return envelope_loop_purse1; }
    bool get_envelope_start_purse1(){ return envelope_start_purse1; }
    bool get_envelope_constant_volume_purse1(){ return envelope_constant_volume_purse1; }
    bool get_sweep_negate_purse1(){ return sweep_negate_purse1; }
    bool get_sweep_enabled_purse1(){ return sweep_enabled_purse1; }
    uint8_t get_sweep_period_purse1(){ return sweep_period_purse1; }
    uint8_t get_sweep_shift_purse1(){ return sweep_shift_purse1; }
    bool get_sweep_reload_purse1(){ return sweep_reload_purse1; }
    uint8_t get_length_counter_purse1(){ return length_counter_purse1; }

    uint16_t get_NMI();
    uint16_t get_RESET();
    uint16_t get_IRQ();

    uint16_t get_PC();
    uint8_t get_SP();
    uint8_t get_statusCPU();
    uint8_t get_A();
    uint8_t get_X();
    uint8_t get_Y();

private:
    std::vector<uint8_t> ram;               //ОЗУ
    std::vector<uint8_t> vram;              //видеопамять
    std::vector<uint8_t> palette;           //палитра
    std::unique_ptr<Cartridge> cartridge;   //Картридж
    PPU* ppu;                               //Видеокарта
    CPU* cpu;
    APU* apu;

    uint16_t cycle;
    uint16_t scanline;
    uint16_t frame;
    uint8_t controller[2];

    // APU парамерты
    uint8_t p1_timer_lo = 0;    // младший байт таймера для pulse1
    uint16_t p1_timer = 0;      // таймер для pulse1

    uint8_t p2_timer_lo = 0;    // младший байт таймера для pulse2
    uint16_t p2_timer = 0;      // таймер для pulse2

    uint8_t triangle_timer_lo = 0; // младший байт таймера для Triangle
    uint16_t triangle_timer = 0;   // таймер для triangle

    uint8_t noise_period = 0;

    uint8_t sweep_period_purse1 = 0;
    uint8_t sweep_shift_purse1 = 0;
    uint8_t length_counter_purse1 = 0;
    uint8_t envelope_constant_period_purse1 = 0;
    bool envelope_loop_purse1 = false;
    bool envelope_start_purse1 = false;
    bool envelope_constant_volume_purse1 = false;
    bool sweep_negate_purse1 = false;
    bool sweep_enabled_purse1 = false;
    bool sweep_reload_purse1 = false;

    bool envelope_loop_noise = false;

    bool constant_volume_purse2 = false;
    bool constant_volume_noise = false;

    uint8_t LENGTH_TABLE[32] =
    {
        10,254,20, 2,40, 4,80, 6,160, 8,60,10,14,12,26,14,
        12,16,24,18,48,20,96,22,192,24,72,26,16,28,32,30
    };

    //
};

#endif // BUS_H
