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

    PPUCTRL = PPUMASK = PPUSTATUS = OAMADDR = OAMDATA = PPUSCROLL = PPUDATA = PPUADDR = 0;
}

PPU::~PPU()
{
    if(thread_render_frame.joinable())
        thread_render_frame.join();
}

uint8_t PPU::get_register(uint16_t addr)
{
    uint8_t result;

    if(addr == 0x2000)
        openBus = result = PPUCTRL;
    else if(addr == 0x2001)
        openBus = result = PPUMASK;
    else if(addr == 0x2002)
    {
        uint8_t data = (PPUSTATUS & 0xE0) | (openBus & 0x1F);
        PPUSTATUS &= ~0x80;
        w = false;

        openBus = result = data;
    }
    else if(addr == 0x2003)
        openBus = result = OAMADDR;
    else if(addr == 0x2004)
        openBus = result = oam[OAMADDR];
    else if(addr == 0x2005)
        result = openBus;
    else if(addr == 0x2006)
        result = openBus;
    else if(addr == 0x2007)
    {
        uint8_t data = read_vram_buffered();
        increment_VRAM_address();

        openBus = result = data;
    }
    else
        openBus = result = 0;

    return result;
}

void PPU::set_register(uint16_t addr, uint8_t data)
{
    openBus = data;

    if(addr == 0x2000)
    {
        PPUCTRL = data;
        temp_VRAM = (temp_VRAM & 0xF3FF) | ((data & 0x03) << 10);
    }
    else if(addr == 0x2001)
        PPUMASK = data;
    else if(addr == 0x2003)
        OAMADDR = data;
    else if(addr == 0x2004)
        oam[OAMADDR++] = data;
    else if(addr == 0x2005)
    {
        if (!w)
        {
            numb_pixelX = data & 0x07;
            temp_VRAM = (temp_VRAM & 0xFFE0) | (data >> 3);
            w = true;
        }
        else
        {
            temp_VRAM = (temp_VRAM & 0x8FFF) | ((data & 0x07) << 12);
            temp_VRAM = (temp_VRAM & 0xFC1F) | ((data & 0xF8) << 2);
            w = false;
        }
    }
    else if(addr == 0x2006)
    {
        if (!w)
        {
            temp_VRAM = (temp_VRAM & 0x00FF) | (data << 8);
            w = true;
        }
        else
        {
            temp_VRAM = (temp_VRAM & 0xFF00) | data;
            temp_VRAM &= 0x3FFF;
            current_VRAM = temp_VRAM;
            w = false;
        }
    }
    else if(addr == 0x2007)
    {
        bus->write_ppu(current_VRAM, data);
        increment_VRAM_address();
    }
}

void PPU::set_oam(const uint8_t* _oam)
{
    for(int i = 0; i < 0x100; ++i)
        oam[i] = _oam[i];
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
                uint8_t x, y;
                uint8_t color_pixel = get_background(x, y);

                for(int j = 0; j < sprites_current_scanline.size(); ++j)
                {
                    uint8_t color_sprite = get_sprite(sprites_current_scanline[j]);

                    y = scanline;
                    x = cycle - 1;

                    if (color_sprite != 0xFF)
                    {
                        color_pixel = color_sprite;
                        break;
                    }
                }

                frame_buffer[y][x] = nesPalette[color_pixel];

                if(cycle == 256)
                    increment_y();
            }
            else if(cycle == 257)
            {
                render_VRAM = (render_VRAM & 0xFBFF) | (temp_VRAM & 0x400);
                render_VRAM = (render_VRAM & 0xFFE0) | (temp_VRAM & 0x1F);
            }
            else if(cycle > 257 && cycle < 320)
                get_current_sprites();
        }

        if (scanline == 261)
        {
            if(cycle == 1)
            {
                PPUSTATUS &= ~0x40;
                PPUSTATUS &= ~0x20;
                PPUSTATUS &= ~0x80;
            }
            else if(cycle >= 280 && cycle <= 304)
            {
                render_VRAM = (render_VRAM & 0xF7FF) | (temp_VRAM & 0x800);
                render_VRAM = (render_VRAM & 0xFC1F) | (temp_VRAM & 0x3E0);
                render_VRAM = (render_VRAM & 0x8FFF) | (temp_VRAM & 0x7000);
            }
            else if(cycle == 256)
                increment_y();
            else if(cycle == 257)
            {
                render_VRAM = (render_VRAM & 0xFBFF) | (temp_VRAM & 0x400);
                render_VRAM = (render_VRAM & 0xFFE0) | (temp_VRAM & 0x1F);
            }
        }

        if (scanline == 241 && cycle == 1)
        {
            PPUSTATUS |= 0x80;

            render_VRAM = current_VRAM;

            if (PPUCTRL & 0x80)
                bus->cpu_request_nmi();

            // thread_render_frame = std::thread(&MainWindow::render_frame, window, std::ref(frame_buffer));
        }
    }
}

void PPU::ppu_tick()
{
    // if(cycle == 0 && scanline == 0)
    //     get_current_sprites();

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

void PPU::get_current_sprites()
{
    uint8_t sprite_height = (PPUCTRL & 0x20) > 0 ? 16 : 8;

    sprites_current_scanline.clear();

    for(int i = 0; i < oam.size(); i+=4)
    {
        if(scanline >= oam[i] && scanline < oam[i] + sprite_height)
        {
            if(sprites_current_scanline.size() == 8)
            {
                PPUSTATUS |= 0x20;
                break;
            }

            sprites_current_scanline.push_back(Sprite{oam[i], oam[i + 1], oam[i + 2], oam[i + 3]});
        }
    }
}

uint8_t PPU::get_sprite(const Sprite& sprite)
{
    int spriteScreenY = sprite.y + 1;
    int spriteScreenX = sprite.x;
    uint8_t sprite_height = (PPUCTRL & 0x20) > 0 ? 16 : 8;

    if (scanline < spriteScreenY || scanline >= spriteScreenY + sprite_height)
        return 0xFF;
    if (cycle < spriteScreenX || cycle >= spriteScreenX + 8)
        return 0xFF;

    int localY = scanline - spriteScreenY;
    int localX = cycle - spriteScreenX;

    bool flipY = (sprite.attr & 0x80) != 0;
    bool flipX = (sprite.attr & 0x40) != 0;

    if (flipY)
        localY = 7 - localY;
    if (flipX)
        localX = 7 - localX;

    uint16_t patternBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
    uint16_t tileAddr = patternBase + sprite.tile * 16;

    uint8_t lowByte = bus->read_ppu(tileAddr + localY);
    uint8_t highByte = bus->read_ppu(tileAddr + localY + 8);

    uint8_t bitIndex = 7 - localX;

    uint8_t lo = (lowByte  >> bitIndex) & 1;
    uint8_t hi = (highByte >> bitIndex) & 1;

    uint8_t color = (hi << 1) | lo;

    uint8_t paletteIndex = sprite.attr & 0x03;

    uint8_t colorByte;
    if (color == 0)
        colorByte = 0xFF;
    else
    {
        uint16_t addr = 0x3F10 + paletteIndex * 4 + color;
        colorByte = bus->read_ppu(addr);
    }

    return colorByte;
}

uint8_t PPU::get_background(uint8_t& x, uint8_t& y)
{
    uint8_t numb_tileX = render_VRAM & 0x1F;
    uint8_t numb_tileY = (render_VRAM >> 5) & 0x1F;
    uint8_t numb_pixelY = (render_VRAM >> 12) & 0x07;

    x = numb_tileX * 8 + numb_pixelX;
    y = numb_tileY * 8 + numb_pixelY;


    uint16_t nameTableBase = 0x2000 | (render_VRAM & 0x0C00);
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


    ++numb_pixelX;
    if (numb_pixelX == 8)
    {
        numb_pixelX = 0;
        increment_x();
    }

    return colorByte & 0x3F;
}

void PPU::increment_x()
{
    if ((render_VRAM & 0x001F) == 31)
    {
        render_VRAM &= ~0x001F;
        render_VRAM ^=  0x0400;
    }
    else
        ++render_VRAM;
}

void PPU::increment_y()
{
    if ((render_VRAM & 0x7000) != 0x7000)
        render_VRAM += 0x1000;
    else
    {
        render_VRAM &= ~0x7000;
        uint16_t temp_numb_pixelY = (render_VRAM & 0x03E0);

        if (temp_numb_pixelY == 0x03A0)
        {
            temp_numb_pixelY = 0;
            render_VRAM ^= 0x0800;
        }
        else if (temp_numb_pixelY == 0x03E0)
            temp_numb_pixelY = 0;
        else
            temp_numb_pixelY += 0x20;

        render_VRAM = (render_VRAM & ~0x03E0) | temp_numb_pixelY;
    }
}

void PPU::increment_VRAM_address()
{
    if (PPUCTRL & 0x04)
        current_VRAM += 32;
    else
        ++current_VRAM;

    current_VRAM &= 0x3FFF;
}

uint8_t PPU::read_vram_buffered()
{
    uint8_t value = bus->read_ppu(current_VRAM);
    uint8_t ret;

    if (current_VRAM >= 0x3F00 && current_VRAM <= 0x3FFF)
    {
        ret = value;
        ppu_data_buffer = bus->read_ppu(current_VRAM - 0x1000);
    }
    else
    {
        ret = ppu_data_buffer;
        ppu_data_buffer = value;
    }

    return ret;
}
