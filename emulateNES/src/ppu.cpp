#include "ppu.h"
#include <QMainWindow>
#include "bus.h"



PPU::PPU(MainWindow* _window, Bus* _bus) : window(_window), bus(_bus)
{
    vram.resize(0x800);
}

uint8_t PPU::get_register(uint16_t addr)
{
    if(addr == 0x2000)
        return _2000;
    else if(addr == 0x2001)
        return _2001;
    else if(addr == 0x2002)
        return _2002;
    else if(addr == 0x2003)
        return _2003;
    else if(addr == 0x2004)
        return _2004;
    else if(addr == 0x2005)
        return _2005;
    else if(addr == 0x2006)
        return _2006;
    else if(addr == 0x2007)
        return _2007;
    else
        return 0;
}

void PPU::run(int cycles)
{
    int count = cycles * 3;
    for(int i = 0; i < count; ++i)
    {
        ppu_tick();
    }
}

void PPU::ppu_tick()
{
    ++cycle;

    if(cycle > 340)
    {
        cycle = 0;
        ++scanline;

        if(scanline > 261)
        {
            scanline = 0;
            ++frame;
        }
    }
}

