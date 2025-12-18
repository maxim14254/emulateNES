#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <vector>


class Bus;
class MainWindow;


class PPU
{
public:
    PPU(MainWindow* _window, Bus* _bus);

    uint8_t get_register(uint16_t addr);
    void run(int cycles);

private:
    uint8_t  _2000, _2001, _2002, _2003, _2004, _2007;    // регистры
    uint16_t  _2005, _2006;                               // регистры
    std::vector<uint8_t> vram;                            // видеопамять
    uint16_t scanline;
    uint16_t cycle;
    uint64_t frame;


    MainWindow* window;
    Bus* bus;


    void ppu_tick();
};

#endif // PPU_H
