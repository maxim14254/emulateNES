#include "ppu.h"
#include "mainwindow.h"
#include "bus.h"
#include <QDebug>



PPU::PPU(MainWindow* _window, Bus* _bus) : window(_window), bus(_bus)
{
    oam.resize(0x100);

    frame_buffer.resize(240);
    for (auto &row : frame_buffer)
        row.resize(256, Color{0, 0, 0});

    PPUCTRL = PPUMASK = PPUSTATUS = OAMADDR = OAMDATA = PPUSCROLL = PPUDATA = PPUADDR = v = 0;
}

PPU::~PPU()
{
    if(thread_render_frame.joinable())
        thread_render_frame.join();
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
        return oam[OAMADDR];
    else if(addr == 0x2005)
        return PPUSCROLL;
    else if(addr == 0x2006)
        return 0;
    else if(addr == 0x2007)
    {
        uint8_t data = read_vram_buffered();
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
        bus->write_ppu(v, data);
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

                frame_buffer[scanline][cycle - 1] = nesPalette[color_pixel];
            }
        }

        if (scanline == 261 && cycle == 1)
        {
            PPUSTATUS &= ~0x40;
            PPUSTATUS &= ~0x20;
            PPUSTATUS &= ~0x80;
        }

        if (scanline == 241 && cycle == 1)
        {
            PPUSTATUS |= 0x80;

            if (PPUCTRL & 0x80)
                bus->cpu_request_nmi();

            thread_render_frame = std::thread(&MainWindow::render_frame, window, std::ref(frame_buffer));
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
            if(thread_render_frame.joinable())
                thread_render_frame.join();

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
    uint8_t tileByte = bus->read_ppu(nameTableBase + tileIndex);

    uint16_t attrTableBase = nameTableBase + 0x3C0;
    uint16_t attrIndex = (numb_tileY / 4) * 8 + (numb_tileX / 4);
    uint8_t attrByte = bus->read_ppu(attrTableBase + attrIndex);

    uint8_t quadrantX = (numb_tileX / 2) % 2;
    uint8_t quadrantY = (numb_tileY / 2) % 2;
    uint8_t shift = (quadrantY * 2 + quadrantX) * 2;

    uint8_t paletteNumb = (attrByte >> shift) & 0x03;

    uint16_t patternBase = (PPUCTRL & 0x10) ? 0x1000 : 0x0000;
    uint16_t tileAddr = patternBase + tileByte * 16 + numb_pixelY;

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
        v += 32;
    else
        v += 1;
}

uint8_t PPU::read_vram_buffered()
{
    uint8_t value = bus->read_ppu(v);
    uint8_t ret;

    if (v >= 0x3F00 && v <= 0x3FFF)
    {
        ret = value;
        ppu_data_buffer = bus->read_ppu(v - 0x1000);
    } else
    {
        ret = ppu_data_buffer;
        ppu_data_buffer = value;
    }

    return ret;
}

