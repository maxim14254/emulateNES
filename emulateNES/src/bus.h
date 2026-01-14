#ifndef BUS_H
#define BUS_H

#include <vector>
#include <memory>


class Cartridge;
class QString;
class PPU;
class CPU;

class Bus
{
public:

    Bus();
    ~Bus();

    uint8_t read_cpu(uint16_t addr);
    void write_cpu(uint16_t addr, uint8_t data);
    uint8_t read_ppu(uint16_t addr);
    void write_ppu(uint16_t addr, uint8_t data);
    void init_new_cartridge(const QString& path, bool* status);
    void init_PPU(PPU* _ppu);
    void init_CPU(CPU* _cpu);
    void run_steps_ppu(int cycles);
    void cpu_request_nmi();
    void reset_ppu();

    uint16_t get_NMI();
    uint16_t get_RESET();
    uint16_t get_IRQ();

private:
    std::vector<uint8_t> ram;               //ОЗУ
    std::vector<uint8_t> vram;              //видеопамять
    std::vector<uint8_t> palette;           //палитра
    std::unique_ptr<Cartridge> cartridge;   //Картридж
    PPU* ppu;                               //Видеокарта
    CPU* cpu;

    uint16_t cycle;
    uint16_t scanline;
    uint16_t frame;
};

#endif // BUS_H
