#include "mapper_34.h"

Mapper_34::Mapper_34(QFile& file, NESHeader _header)
{
    header = _header;

    prg_rom.resize(header.prg_rom * 0x4000);

    if(header.chr_rom > 0)
        chr_rom.resize(header.chr_rom * 0x2000);
    else
        chr_ram.resize(0x2000);

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    prg_bank_count_32k = static_cast<uint32_t>(prg_rom.size() / 0x8000);

    if (prg_bank_count_32k == 0)
        prg_bank_count_32k = 1;

    uint32_t prg_ram_size = (header.prg_ram == 0 ? 1 : header.prg_ram) * 0x2000;
    prg_ram.resize(prg_ram_size, 0);


    init_mirroring();
}

Mapper_34::~Mapper_34()
{

}

void Mapper_34::init_mirroring()
{
    Orintation = (header.flags6 & 0x01) ? VERTICAL : HORIZONTAL;
}

uint8_t Mapper_34::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000)
        return 0;

    uint32_t bank = prg_bank % prg_bank_count_32k;
    uint32_t offset = static_cast<uint32_t>(addr - 0x8000);
    uint32_t index = bank * 0x8000 + offset;

    if (index < prg_rom.size())
        return prg_rom[index];

    return 0;
}

uint8_t Mapper_34::mapper_read_chr(uint16_t addr)
{
    if (addr >= 0x2000)
        return 0;

    if (!chr_rom.empty())
    {
        if (addr < chr_rom.size())
            return chr_rom[addr];
        return 0;
    }

    if (addr < chr_ram.size())
        return chr_ram[addr];

    return 0;
}

uint8_t Mapper_34::read_prg_ram(uint16_t addr)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return 0;

    uint32_t offset = addr - 0x6000;
    if (offset < prg_ram.size())
        return prg_ram[offset];

    return 0;
}

void Mapper_34::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return;

    uint32_t offset = addr - 0x6000;
    if (offset < prg_ram.size())
        prg_ram[offset] = data;
}

void Mapper_34::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (addr >= 0x2000)
        return;

    if (!chr_rom.empty())
        return;

    if (addr < chr_ram.size())
        chr_ram[addr] = data;
}

void Mapper_34::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
    {
        if (prg_bank_count_32k != 0)
            prg_bank = data % prg_bank_count_32k;
        else
            prg_bank = 0;
    }
}

uint16_t Mapper_34::map_nametable_addr(uint16_t addr)
{
    uint16_t table = addr / 0x0400;
    uint16_t offset = addr & 0x03FF;

    switch (Orintation)
    {
    case VERTICAL:
        return (table % 2) * 0x0400 + offset;
    case HORIZONTAL:
        return ((table / 2) * 0x0400) + offset;
    case ONESCREEN_LO:
        return offset;
    case ONESCREEN_HI:
        return 0x0400 + offset;
    }

    return offset;
}

uint16_t Mapper_34::get_NMI()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
    else
        return 0;
}

uint16_t Mapper_34::get_RESET()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
    else
        return 0;
}

uint16_t Mapper_34::get_IRQ()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
    else
        return 0;
}
