#include "mapper_3.h"

Mapper_3::Mapper_3(QFile& file, NESHeader _header)
{
    header = _header;

    prg_rom.resize(header.prg_rom * 0x4000);

    if(header.chr_rom > 0)
        chr_rom.resize(header.chr_rom * 0x2000);
    else
        chr_ram.resize(0x2000);

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    Orintation = (header.flags6 & 0x01) ? VERTICAL : HORIZONTAL;

    size_t chr_banks = chr_rom.empty() ? 1 : (chr_rom.size() / 0x2000);

    if (chr_banks == 0)
        chr_banks = 1;

    chr_bank_mask = static_cast<uint8_t>(chr_banks - 1);
    chr_bank = 0;
}

Mapper_3::~Mapper_3()
{

}

uint8_t Mapper_3::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000)
        return 0;

    if (prg_rom.size() == 0x4000)
    {
        return prg_rom[(addr - 0x8000) & 0x3FFF];
    }
    else if (prg_rom.size() >= 0x8000)
    {
        return prg_rom[(addr - 0x8000) & 0x7FFF];
    }

    return prg_rom.empty() ? 0 : prg_rom[(addr - 0x8000) % prg_rom.size()];
}

uint8_t Mapper_3::mapper_read_chr(uint16_t addr)
{
    if (addr >= 0x2000)
        return 0;

    if (!chr_rom.empty())
    {
        size_t bank_offset = static_cast<size_t>(chr_bank) * 0x2000;
        size_t index = bank_offset + addr;

        if (index < chr_rom.size())
            return chr_rom[index];

        return 0;
    }

    if (!chr_ram.empty())
        return chr_ram[addr & 0x1FFF];

    return 0;
}

uint8_t Mapper_3::read_prg_ram(uint16_t addr)
{
    if (addr >= 0x6000 && addr <= 0x7FFF && !prg_ram.empty())
    {
        size_t index = (addr - 0x6000) % prg_ram.size();
        return prg_ram[index];
    }

    return 0;
}

void Mapper_3::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF && !prg_ram.empty())
    {
        size_t index = (addr - 0x6000) % prg_ram.size();
        prg_ram[index] = data;
    }
}

void Mapper_3::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000 && !chr_ram.empty())
    {
        chr_ram[addr & 0x1FFF]= data;
    }
}

void Mapper_3::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
    {
        chr_bank = data & chr_bank_mask;
    }
}

uint16_t Mapper_3::map_nametable_addr(uint16_t addr)
{
    uint16_t vram_addr = (addr - 0x2000) & 0x0FFF;
    uint16_t table = vram_addr / 0x0400;
    uint16_t offset = vram_addr % 0x0400;

    switch (Orintation)
    {
    case VERTICAL:
        switch (table)
        {
        case 0: return 0x0000 + offset;
        case 1: return 0x0400 + offset;
        case 2: return 0x0000 + offset;
        case 3: return 0x0400 + offset;
        }
        break;

    case HORIZONTAL:
        switch (table)
        {
        case 0: return 0x0000 + offset;
        case 1: return 0x0000 + offset;
        case 2: return 0x0400 + offset;
        case 3: return 0x0400 + offset;
        }
        break;

    case ONESCREEN_LO:
        return 0x0000 + offset;

    case ONESCREEN_HI:
        return 0x0400 + offset;
    }

    return offset;
}

uint16_t Mapper_3::get_NMI()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
    else
        return 0;
}

uint16_t Mapper_3::get_RESET()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
    else
        return 0;
}

uint16_t Mapper_3::get_IRQ()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
    else
        return 0;
}
