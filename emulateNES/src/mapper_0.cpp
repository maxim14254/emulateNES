#include "mapper_0.h"



Mapper_0::Mapper_0(QFile& file, NESHeader _header)
{
    header = _header;

    prg_rom.resize(header.prg_rom * 0x4000);

    if(header.chr_rom > 0)
        chr_rom.resize(header.chr_rom * 0x2000);
    else
        chr_ram.resize(0x2000);

    if(header.prg_ram > 0)
        prg_ram.resize(header.prg_ram * 0x2000);
    else
        prg_ram.resize(0x2000);

    bool has_trainer = (header.flags6 & 0x04) != 0;
    if (has_trainer)
        file.seek(512);

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    Orintation = header.flags6 & 0x01 ? VERTICAL : HORIZONTAL;
}

Mapper_0::~Mapper_0()
{

}

uint8_t Mapper_0::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000)
        return 0;

    uint32_t prgSize = prg_rom.size();

    if (prgSize == 0)
        return 0;

    uint32_t offset = addr - 0x8000;

    if (prgSize == 0x4000)
        offset &= 0x3FFF;

    offset %= prgSize;

    uint8_t dd = prg_rom[offset];
    return dd;
}

uint8_t Mapper_0::mapper_read_chr(uint16_t addr)
{
    if(chr_rom.size() > 0)
        return chr_rom[addr];
    else
        return chr_ram[addr];
}

uint8_t Mapper_0::read_prg_ram(uint16_t addr)
{
    return prg_ram[addr - 0x6000];
}

void Mapper_0::write_prg_ram(uint16_t addr, uint8_t data)
{
    prg_ram[addr - 0x6000] = data;
}

void Mapper_0::write_chr_ram(uint16_t addr, uint8_t data)
{
    if(chr_ram.size() > 0)
        chr_ram[addr] = data;
}

uint16_t Mapper_0::map_nametable_addr(uint16_t addr)
{
    addr &= 0x0FFF;

    switch (Orintation)
    {
        case VERTICAL:
            return addr & 0x07FF;
        case HORIZONTAL:
            return ((addr & 0x0800) >> 1) | (addr & 0x03FF);
        case ONESCREEN_LO:
            return addr & 0x03FF;
        case ONESCREEN_HI:
            return 0x0400 | (addr & 0x03FF);
    }

    return addr & 0x07FF;
}

uint16_t Mapper_0::get_NMI()
{
    if(prg_rom.size() > 0)
    {
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
    }
    else
        return 0;
}

uint16_t Mapper_0::get_RESET()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
    else
        return 0;
}

uint16_t Mapper_0::get_IRQ()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
    else
        return 0;
}

