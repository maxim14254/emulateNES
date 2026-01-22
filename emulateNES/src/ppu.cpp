#include "ppu.h"
#include "mainwindow.h"
#include "bus.h"
#include <QDebug>
#include <QObject>
#include "global.h"
#include <map>


PPU::PPU(MainWindow* _window, Bus* _bus) : window(_window), bus(_bus)
{
    oam.resize(0x100);
    //pColData.resize(128 * 128);

    frame_buffer.resize(240);
    for (auto &row : frame_buffer)
        row.resize(256, Color{0, 0, 0});

    PPUCTRL = PPUMASK = PPUSTATUS = OAMADDR = OAMDATA = PPUSCROLL = PPUDATA = PPUADDR = 0;
}

PPU::~PPU()
{

}

uint8_t PPU::get_register(uint16_t addr, bool onlyRead)
{
    uint8_t result = 0;

    if(onlyRead)
    {
        if(addr == 0x2000)
            result = PPUCTRL;
        else if(addr == 0x2001)
            result = PPUMASK;
        else if(addr == 0x2002)
            result = PPUSTATUS;
//        else if(addr == 0x2003)
//            result = OAMADDR;
//        else if(addr == 0x2004)
//            result = oam[OAMADDR];
//        else if(addr == 0x2005)
//            result = openBus;
//        else if(addr == 0x2006)
//            result = openBus;
//        else if(addr == 0x2007)
//            result = read_vram_buffered();
        else
            result = 0;
    }
    else
    {
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
    }

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
                uint8_t x = cycle - 1;
                uint8_t y = scanline;

                uint8_t color_pixel = get_background();
                uint8_t color_sprite = 0;
                uint8_t sprite_color_final = 0;
                bool sprite_from_sprite0 = false;

                if((PPUMASK & 0x10) || ((PPUMASK & 0x04) && x >= 8))
                {
                    for(size_t j = 0; j < sprites_current_scanline.size(); ++j)
                    {
                        color_sprite = get_sprite(sprites_current_scanline[j]);

                        if (color_sprite != 0)
                        {
                            bool front = (sprites_current_scanline[j].attr & 0x20) == 0;

                            if(sprites_current_scanline[j].index == 0)
                                sprite_from_sprite0 = true;

                            if(front || color_pixel == 0)
                                sprite_color_final = color_sprite;

                            break;
                        }
                    }
                }

                if(!(PPUSTATUS & 0x40) && sprite_from_sprite0 && color_pixel != 0 && sprite_color_final != 0)
                    PPUSTATUS |= 0x40;

                if(sprite_color_final != 0 && color_pixel == 0)
                    frame_buffer[y][x] = nesPalette[sprite_color_final];
                else
                    frame_buffer[y][x] = nesPalette[color_pixel];

                if(cycle == 256)
                    increment_y();
            }
            else if(cycle == 257)
            {
                render_VRAM = (render_VRAM & 0xFBFF) | (temp_VRAM & 0x400);
                render_VRAM = (render_VRAM & 0xFFE0) | (temp_VRAM & 0x1F);
            }
            //else if(cycle > 257 && cycle < 320)
                //get_current_sprites();
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
            QMetaObject::invokeMethod(window, [&]()
            {
                window->render_frame(frame_buffer, mutex_lock_frame_buffer);
            },
            Qt::QueuedConnection);

            PPUSTATUS |= 0x80;

            render_VRAM = current_VRAM;

            if (PPUCTRL & 0x80)
                bus->cpu_request_nmi();
        }
    }
}

void PPU::run_watch_all_tiles()
{
    int numb_table = 0;
    int palette = 0;

    static std::vector<uint32_t> pColData1(128 * 128);
    static std::vector<uint32_t> pColData2(128 * 128);

    for (uint16_t nTileY = 0; nTileY < 16; ++nTileY)
    {
        for (uint16_t nTileX = 0; nTileX < 16; ++nTileX)
        {
            uint16_t nOffset = nTileY * 256 + nTileX * 16;

            for (uint16_t row = 0; row < 8; ++row)
            {
                uint8_t tile_lsb = bus->read_ppu(numb_table * 0x1000 + nOffset + row + 0x0000);
                uint8_t tile_msb = bus->read_ppu(numb_table * 0x1000 + nOffset + row + 0x0008);

                for (uint16_t col = 0; col < 8; ++col)
                {
                    uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);

                    tile_lsb >>= 1;
                    tile_msb >>= 1;

                    int x = nTileX * 8 + (7 - col);
                    int y = nTileY * 8 + row;
                    Color color = nesPalette[bus->read_ppu(0x3F00 + (palette * 4) + pixel) & 0x3F];

                    uint32_t pixel2 = (static_cast<uint32_t>(color.r) << 16) | (static_cast<uint32_t>(color.g) << 8) | (static_cast<uint32_t>(color.b) << 0);
                    pColData1[y * 128 + x] = pixel2;
                }
            }
        }
    }

    numb_table = 1;
    for (uint16_t nTileY = 0; nTileY < 16; ++nTileY)
    {
        for (uint16_t nTileX = 0; nTileX < 16; ++nTileX)
        {
            uint16_t nOffset = nTileY * 256 + nTileX * 16;

            for (uint16_t row = 0; row < 8; ++row)
            {
                uint8_t tile_lsb = bus->read_ppu(numb_table * 0x1000 + nOffset + row + 0x0000);
                uint8_t tile_msb = bus->read_ppu(numb_table * 0x1000 + nOffset + row + 0x0008);

                for (uint16_t col = 0; col < 8; ++col)
                {
                    uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);

                    tile_lsb >>= 1;
                    tile_msb >>= 1;

                    int x = nTileX * 8 + (7 - col);
                    int y = nTileY * 8 + row;
                    Color color = nesPalette[bus->read_ppu(0x3F00 + (palette << 2) + pixel) & 0x3F];

                    uint32_t pixel2 = (static_cast<uint32_t>(color.r) << 16) | (static_cast<uint32_t>(color.g) << 8) | (static_cast<uint32_t>(color.b) << 0);
                    pColData2[y * 128 + x] = pixel2;
                }
            }
        }
    }

    QMetaObject::invokeMethod(window, [&]()
    {
        window->render_debug_tiles(pColData1.data(), pColData2.data());
    },
    Qt::QueuedConnection);

}

void PPU::run_watch_cpu_instr(uint16_t PC)
{
    static bool download_asm = true;
    static std::map<uint16_t, std::string> assembler_buf;

    if(download_asm)
    {
        download_asm = false;
        download_asm_buffer(assembler_buf);
    }

    auto it_a = assembler_buf.find(PC);

    QString value = "";

    if(it_a != assembler_buf.end())
    {
        for(int i = 0; i < 10; ++i)
            --it_a;

        for(int i = 0; i < 9; ++i)
        {
            if (++it_a != assembler_buf.end())
            {
                value += QString("    %1\n").arg(it_a->second.c_str());
            }
        }
    }

    if(++it_a != assembler_buf.end())
    {
        value += QString("=>%1\n").arg(it_a->second.c_str());

        for(int i = 0; i < 10; ++i)
        {
            if (++it_a != assembler_buf.end())
            {
                value += QString("    %1\n").arg(it_a->second.c_str());
            }
        }
    }

    QMetaObject::invokeMethod(window, [&, value]()
    {
        window->render_cpu_debug(value, PPUCTRL, PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUDATA, PPUADDR,
                                 bus->get_PC(), bus->get_SP(), bus->get_statusCPU(), bus->get_A(), bus->get_X(), bus->get_Y());
    },
    Qt::QueuedConnection);
}

void PPU::reset()
{
    scanline = 0;
    cycle = 0;
    frame = 0;
    render_VRAM = temp_VRAM = current_VRAM = 0;
    w = false;
    ppu_data_buffer = 0;
    PPUCTRL = PPUMASK = PPUSTATUS = OAMADDR = OAMDATA = PPUSCROLL = PPUDATA = PPUADDR = 0;
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
            std::lock_guard lock(mutex_lock_frame_buffer);

            scanline = 0;
            ++frame;

#if DEBUG_ON
            run_watch_all_tiles();
            run_watch_cpu_instr(bus->get_PC());
#endif
        }
    }
    else if(scanline == 261 && cycle == 339 && (PPUMASK & 0x18) && (frame & 1))
    {
        std::lock_guard lock(mutex_lock_frame_buffer);

        scanline = cycle = 0;

#if DEBUG_ON
        run_watch_all_tiles();
        run_watch_cpu_instr(bus->get_PC());
#endif
    }
}

void PPU::get_current_sprites()
{
    uint8_t sprite_height = (PPUCTRL & 0x20) > 0 ? 16 : 8;

    sprites_current_scanline.clear();

    for(size_t i = 0; i < oam.size(); i+=4)
    {
        if(scanline >= oam[i] && scanline < oam[i] + sprite_height)
        {
            if(sprites_current_scanline.size() == 8)
            {
                PPUSTATUS |= 0x20;
                break;
            }

            sprites_current_scanline.push_back(Sprite{oam[i], oam[i + 1], oam[i + 2], oam[i + 3], i / 4});
        }
    }
}

void PPU::download_asm_buffer(std::map<uint16_t, std::string> &assembler_buf)
{
    uint32_t addr = 0x0000;
    uint16_t line_addr = 0x0000;

    auto hex = [](uint32_t n, uint8_t d)
    {
        std::string s(d, '0');

        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];

        return s;
    };

    while (addr <= 0xFFFF)
    {
        line_addr = addr;

        std::string inst = "$" + hex(addr, 4) + ":  ";

        uint8_t opcode = bus->read_cpu(addr++);
        inst += table_instructions[opcode].name + "  ";

        if (table_instructions[opcode].addrmode == "IMP")
        {
            inst += "  {IMP}";
        }
        else if (table_instructions[opcode].addrmode == "IMM")
        {
            uint8_t value = bus->read_cpu(addr++);
            inst += "#$" + hex(value, 2) + "  {IMM}";
        }
        else if (table_instructions[opcode].addrmode == "ZP0")
        {
            uint8_t lo = bus->read_cpu(addr++);
            inst += "$" + hex(lo, 2) + "  {ZP0}";
        }
        else if (table_instructions[opcode].addrmode == "ZPX")
        {
            uint8_t lo = bus->read_cpu(addr++);
            inst += "$" + hex(lo, 2) + ",  X  {ZPX}";
        }
        else if (table_instructions[opcode].addrmode == "ZPY")
        {
            uint8_t lo = bus->read_cpu(addr++);
            inst += "$" + hex(lo, 2) + ",  Y  {ZPY}";
        }
        else if (table_instructions[opcode].addrmode == "IZX")
        {
            uint8_t lo = bus->read_cpu(addr++);
            inst += "($" + hex(lo, 2) + ",  X)  {IZX}";
        }
        else if (table_instructions[opcode].addrmode == "IZY")
        {
            uint8_t lo = bus->read_cpu(addr++);
            inst += "($" + hex(lo, 2) + "),  Y  {IZY}";
        }
        else if (table_instructions[opcode].addrmode == "ABS")
        {
            uint8_t lo = bus->read_cpu(addr++);
            uint8_t hi = bus->read_cpu(addr++);
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + "  {ABS}";
        }
        else if (table_instructions[opcode].addrmode == "ABX")
        {
            uint8_t lo = bus->read_cpu(addr++);
            uint8_t hi = bus->read_cpu(addr++);
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ",  X  {ABX}";
        }
        else if (table_instructions[opcode].addrmode == "ABY")
        {
            uint8_t lo = bus->read_cpu(addr++);
            uint8_t hi = bus->read_cpu(addr++);
            inst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ",  Y  {ABY}";
        }
        else if (table_instructions[opcode].addrmode == "IND")
        {
            uint8_t lo = bus->read_cpu(addr++);
            uint8_t hi = bus->read_cpu(addr++);
            inst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ")  {IND}";
        }
        else if (table_instructions[opcode].addrmode == "REL")
        {
            uint8_t value = bus->read_cpu(addr++);
            inst += "$" + hex(value, 2) + "  [$" + hex(addr + (int8_t)value, 4) + "]  {REL}";
        }

        assembler_buf[line_addr] = inst;
    }
}

uint8_t PPU::get_sprite(const Sprite& sprite)
{
    int spriteScreenY = sprite.y + 1;
    int spriteScreenX = sprite.x;
    uint8_t sprite_height = (PPUCTRL & 0x20) > 0 ? 16 : 8;

    if (scanline < spriteScreenY || scanline >= spriteScreenY + sprite_height)
        return 0;
    if (cycle < spriteScreenX || cycle >= spriteScreenX + 8)
        return 0;

    int localY = scanline - spriteScreenY;
    int localX = cycle - spriteScreenX;

    bool flipY = (sprite.attr & 0x80) != 0;
    bool flipX = (sprite.attr & 0x40) != 0;

    if (flipY)
        localY = sprite_height - 1 - localY;
    if (flipX)
        localX = 7 - localX;

    uint16_t patternBase;
    uint16_t tileAddr;

    if(sprite_height == 8)
    {
        patternBase = (PPUCTRL & 0x08) ? 0x1000 : 0x0000;
        tileAddr = patternBase + sprite.tile * 16;
    }
    else
    {
        patternBase = (sprite.tile & 1) ? 0x1000 : 0x0000;

        uint8_t tileIndex = (sprite.tile & 0xFE);
        if (localY >= 8)
        {
            tileIndex += 1;
            localY -= 8;
        }

        tileAddr = patternBase + tileIndex * 16;
    }

    uint8_t lowByte = bus->read_ppu(tileAddr + localY);
    uint8_t highByte = bus->read_ppu(tileAddr + localY + 8);

    uint8_t bitIndex = 7 - localX;

    uint8_t lo = (lowByte  >> bitIndex) & 1;
    uint8_t hi = (highByte >> bitIndex) & 1;

    uint8_t color = (hi << 1) | lo;

    uint8_t paletteIndex = sprite.attr & 0x03;

    uint8_t colorByte;
    if (color == 0)
        colorByte = color;
    else
    {
        uint16_t addr = 0x3F10 + paletteIndex * 4 + color;
        colorByte = bus->read_ppu(addr);
    }

    return colorByte;
}

uint8_t PPU::  get_background()
{   
    uint8_t x = cycle - 1;
    uint8_t y = scanline;

    uint8_t numb_tileX = x / 8;
    uint8_t numb_tileY = y / 8;

//    x = numb_tileX * 8 + numb_pixelX;
//    y = numb_tileY * 8 + numb_pixelY;

    if(!(PPUMASK & 0x08) || (!(PPUMASK & 0x02) && cycle - 1 < 8))
        return 0;

    uint16_t nameTableBase = 0x2000 | (render_VRAM & 0x0C00);
    uint16_t tileIndex = y * 256 + x;
    uint8_t tileByte = bus->read_ppu(nameTableBase + tileIndex);

    uint16_t attrTableBase = nameTableBase + 0x3C0;
    uint16_t attrIndex = (numb_tileY / 4) * 8 + (numb_tileX / 4);
    uint8_t attrByte = bus->read_ppu(attrTableBase + attrIndex);

    uint8_t quadrantX = (x / 2) % 2;
    uint8_t quadrantY = (y / 2) % 2;
    uint8_t shift = (quadrantY * 2 + quadrantX) * 2;

    uint8_t paletteNumb = (attrByte >> shift) & 0x03;

    uint16_t patternBase = (PPUCTRL & 0x10) ? 0x1000 : 0x0000;
    uint16_t tileAddr = patternBase + tileByte * 16 + y;

    uint8_t lowByte  = bus->read_ppu(tileAddr);
    uint8_t highByte = bus->read_ppu(tileAddr + 8);

    uint8_t step_bit = 7 - x;
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
