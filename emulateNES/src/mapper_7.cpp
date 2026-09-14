#include "mapper_7.h"
#include <QFile>

Mapper_7::Mapper_7(QFile& file, NESHeader _header)
{
    header = _header;

    int prg_size = (header.prg_rom) * 0x4000;
    prg_rom.resize(prg_size);

    if (prg_size > 0)
        file.read(reinterpret_cast<char*>(prg_rom.data()), static_cast<qint64>(prg_size));

    int chr_size = header.chr_rom * 0x2000;

    if (chr_size > 0)
    {
        chr_rom.resize(chr_size);
        file.read(reinterpret_cast<char*>(chr_rom.data()), static_cast<qint64>(chr_size));
    }
    else
    {
        chr_ram.resize(0x2000);
    }

    prg_ram.resize(0x2000);

    prg_bank = 0;
    Orintation = ONESCREEN_LO;
}

Mapper_7::~Mapper_7()
{
}

uint8_t Mapper_7::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000)
        return 0;

    if (prg_rom.empty())
        return 0;

    int offset = prg_bank * 0x8000 + (addr - 0x8000);
    offset %= prg_rom.size();

    return prg_rom[offset];
}

uint8_t Mapper_7::mapper_read_chr(uint16_t addr)
{
    addr &= 0x1FFF;

    if (!chr_rom.empty())
        return chr_rom[addr % chr_rom.size()];

    if (!chr_ram.empty())
        return chr_ram[addr];

    return 0;
}

uint8_t Mapper_7::read_prg_ram(uint16_t addr)
{
    if (addr >= 0x6000 && addr <= 0x7FFF && !prg_ram.empty())
        return prg_ram[(addr - 0x6000) % prg_ram.size()];

    return 0;
}

void Mapper_7::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF && !prg_ram.empty())
        prg_ram[(addr - 0x6000) % prg_ram.size()] = data;
}

void Mapper_7::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (chr_rom.empty() && !chr_ram.empty())
        chr_ram[addr & 0x1FFF] = data;
}

void Mapper_7::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr < 0x8000)
        return;

    prg_bank = data & 0x0F;

    if (data & 0x10)
        Orintation = ONESCREEN_HI;
    else
        Orintation = ONESCREEN_LO;
}

uint16_t Mapper_7::map_nametable_addr(uint16_t addr)
{
    addr &= 0x0FFF;

    if (Orintation == ONESCREEN_HI)
        return (0x400 | (addr & 0x03FF));

    return (addr & 0x03FF);
}

