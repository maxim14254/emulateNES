#ifndef BUS_H
#define BUS_H

#include <vector>
#include <memory>


class Cartridge;
class QString;
class PPU;

class Bus
{
public:

    Bus();
    ~Bus();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void init_new_cartridge(const QString& path, bool* status);
    void init_PPU(PPU* _ppu);
    void run_steps_ppu(int cycles);

    uint16_t get_NMI();
    uint16_t get_RESET();
    uint16_t get_IRQ();

private:
    std::vector<uint8_t> ram;               //ОЗУ
    std::unique_ptr<Cartridge> cartridge;   //Картридж
    PPU* ppu;                               //Видеокарта

    uint16_t cycle;
    uint16_t scanline;
    uint16_t frame;
};

#endif // BUS_H
