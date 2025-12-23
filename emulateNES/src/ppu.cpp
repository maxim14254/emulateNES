#include "ppu.h"
#include <QMainWindow>
#include "bus.h"
#include <QDebug>



PPU::PPU(MainWindow* _window, Bus* _bus) : window(_window), bus(_bus)
{
    oam.resize(0x100);

    frame_buffer.resize(240);
    for (auto &row : frame_buffer)
        row.resize(256, Color{0, 0, 0});

    PPUCTRL = PPUMASK = PPUSTATUS = OAMADDR = OAMDATA = PPUSCROLL = PPUDATA = PPUADDR = 0;
}

PPU::~PPU()
{

}

uint8_t PPU::get_register(uint16_t addr)
{
    if(addr == 0x2000)
        return PPUCTRL;
    else if(addr == 0x2001)
        return PPUMASK;
    else if(addr == 0x2002)
    {
        uint8_t data = PPUSTATUS;
        PPUSTATUS &= ~0x80;
        w = false;

        return data;
    }
    else if(addr == 0x2003)
        return OAMADDR;
    else if(addr == 0x2004)
        return oam[OAMDATA];
    else if(addr == 0x2005)
        return PPUSCROLL;
    else if(addr == 0x2006)
        return PPUADDR;
    else if(addr == 0x2007)
    {
        uint8_t data = bus->read_ppu(PPUADDR);
        incrementVRAMAddress();

        return data;
    }
    else
        return 0;
}

void PPU::set_register(uint16_t addr, uint8_t data)
{
    if(addr == 0x2000)
    {
        PPUCTRL = data;
        t = (t & 0xF3FF) | ((data & 0x03) << 10);
    }
    else if(addr == 0x2001)
    {
        PPUMASK = data;
    }
    else if(addr == 0x2003)
    {
        OAMADDR = data;
    }
    else if(addr == 0x2004)
    {
        oam[OAMADDR++] = data;
    }
    else if(addr == 0x2005)
    {
        if (!w)
        {
            x = data & 0x07;
            t = (t & 0xFFE0) | (data >> 3);
            w = true;
        }
        else
        {
            t = (t & 0x8FFF) | ((data & 0x07) << 12);
            t = (t & 0xFC1F) | ((data & 0xF8) << 2);
            w = false;
        }
    }
    else if(addr == 0x2006)
    {
        if (!w)
        {
            t = (t & 0x00FF) | ((data & 0x3F) << 8);
            w = true;
        }
        else
        {
            t = (t & 0xFF00) | data;
            v = t;
            w = false;
        }
    }
    else if(addr == 0x2007)
    {
        bus->write_ppu(PPUADDR, data);
        incrementVRAMAddress();
    }
}

void PPU::run(int cycles)
{
    int count = cycles * 3;

    for(int i = 0; i < count; ++i)
    {
        ppu_tick();

        if (scanline < 240)
        {
            if (cycle >= 1 && cycle <= 256)
            {
                uint8_t color_pixel = get_pixel(cycle - 1, scanline);
                frame_buffer.at(scanline).at(cycle - 1) = nesPalette[color_pixel];
            }
        }

        if (scanline == 261 && cycle == 1)
        {
            // pre-render: надо сбросить VBlank флаг в PPUSTATUS
        }

        if (scanline == 241 && cycle == 1)
        {
            // начало VBlank — ставим флаг и, если разрешено, вызываем NMI
        }
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

uint8_t PPU::get_pixel(uint16_t x, uint16_t y)
{
    uint16_t numb_tileX = x / 8;
    uint16_t numb_tileY = y / 8;
    uint16_t numb_pixelX = x % 8;
    uint16_t numb_pixelY = y % 8;

    uint16_t nameTableBase = 0x2000;
    uint16_t tileIndex = numb_tileY * 32 + numb_tileX;
    //uint8_t tileByte = bus->read_ppu(nameTableBase + tileIndex);

    uint16_t attrTableBase = nameTableBase + 0x3C0;
    uint16_t numb_blockX = numb_tileX / 2;
    uint16_t numb_blockY = numb_tileY / 2;
    uint16_t blockIndex = numb_blockY * 16 + numb_blockX;

    uint8_t attrIndex = blockIndex / 4;
    uint8_t attrByte = bus->read_ppu(attrTableBase + attrIndex);

    uint8_t numbBit = (tileIndex + 1) % 4;
    uint8_t paletteNumb = (attrByte >> (2 * numbBit)) & 0x03;


    uint16_t patternBase = (PPUCTRL & 0x10) ? 0x1000 : 0x0000;
    uint16_t tileAddr = patternBase + tileIndex * 16 + numb_pixelY;

    uint8_t lowByte  = bus->read_ppu(tileAddr);
    uint8_t highByte = bus->read_ppu(tileAddr + 8);

    uint8_t step_bit = 7 - numb_pixelX;
    bool highBit = (highByte >> step_bit) & 1;
    bool lowBit = (lowByte >> step_bit) & 1;
    uint8_t color = (highBit << 1) | lowBit;


    uint8_t colorByte;
    if (color == 0)
        colorByte = bus->read_ppu(0x3F00);
    else
    {
        uint16_t addr = 0x3F00 + paletteNumb * 4 + color;
        colorByte = bus->read_ppu(addr);
    }

    return colorByte & 0x3F;
}

void PPU::incrementVRAMAddress()
{
    if (PPUCTRL & 0x04)
        PPUADDR += 32;
    else
        PPUADDR += 1;
}

