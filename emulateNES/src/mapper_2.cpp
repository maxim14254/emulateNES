#include "mapper_2.h"
#include <QDebug>



Mapper_2::Mapper_2(QFile& file, NESHeader _header)
{
    header = _header;

    prg_rom.resize(header.prg_rom * 0x4000);

    if (header.chr_rom > 0)
        chr_rom.resize(header.chr_rom * 0x2000);
    else
        chr_ram.resize(0x2000, 0);

    bool has_trainer = (header.flags6 & 0x04) != 0;
    if (has_trainer)
        file.seek(16 + 512);

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    if (header.chr_rom > 0)
        file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    size_t prgRamBanks = (header.prg_ram == 0) ? 1 : header.prg_ram;
    prg_ram.resize(prgRamBanks * 8 * 1024, 0);

    Orintation = (header.flags6 & 0x01) ? VERTICAL : HORIZONTAL;

    total_prg_banks = (uint32_t)(prg_rom.size() / 0x4000);
    if (total_prg_banks == 0)
        total_prg_banks = 1;

    reg_prg = 0;
    reg_chr = 0;
}

Mapper_2::~Mapper_2()
{
}

uint32_t Mapper_2::map_prg_addr(uint16_t addr)
{
    uint32_t off = addr & 0x3FFF;

    if (addr < 0xC000)
    {
        uint32_t bank = (reg_prg & 0x0F) % total_prg_banks;
        return bank * 0x4000 + off;
    }
    else
    {
        uint32_t last = total_prg_banks - 1;
        return last * 0x4000 + off;
    }
}

uint8_t Mapper_2::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000 || prg_rom.empty())
        return 0;

    uint32_t mapped = map_prg_addr(addr) % prg_rom.size();
    return prg_rom[mapped];
}

uint8_t Mapper_2::mapper_read_chr(uint16_t addr)
{
    addr &= 0x1FFF;

    if (!chr_rom.empty())
        return chr_rom[addr % chr_rom.size()];

    if (!chr_ram.empty())
        return chr_ram[addr % chr_ram.size()];

    return 0;
}

void Mapper_2::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (chr_ram.empty())
        return;

    addr &= 0x1FFF;
    chr_ram[addr % chr_ram.size()] = data;
}

void Mapper_2::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr < 0x8000)
        return;

    reg_prg = data & 0x0F;
}

uint16_t Mapper_2::map_nametable_addr(uint16_t addr)
{
    uint16_t table = (addr >> 10) & 0x3;
    uint16_t off   = addr & 0x03FF;

    uint16_t physTable = 0;

    switch (Orintation)
    {
    case HORIZONTAL:
        physTable = (table < 2) ? 0 : 1;
        break;
    case VERTICAL:
        physTable = (table & 1);
        break;
    case ONESCREEN_LO:
        physTable = 0;
        break;
    case ONESCREEN_HI:
        physTable = 1;
        break;
    }

    return (uint16_t)(physTable * 0x0400 + off);
}

uint8_t Mapper_2::read_prg_ram(uint16_t addr)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return 0;

    return prg_ram[(addr - 0x6000) % prg_ram.size()];
}

void Mapper_2::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return;

    prg_ram[(addr - 0x6000) % prg_ram.size()] = data;
}

uint16_t Mapper_2::get_NMI()
{
    if (prg_rom.empty())
        return 0;
    return mapper_read_prg(0xFFFA) | (mapper_read_prg(0xFFFB) << 8);
}

uint16_t Mapper_2::get_RESET()
{
    if (prg_rom.empty())
        return 0;
    return mapper_read_prg(0xFFFC) | (mapper_read_prg(0xFFFD) << 8);
}

uint16_t Mapper_2::get_IRQ()
{
    if (prg_rom.empty())
        return 0;
    return mapper_read_prg(0xFFFE) | (mapper_read_prg(0xFFFF) << 8);
}
