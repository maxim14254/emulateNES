#include "mapper_1.h"


Mapper_1::Mapper_1(QFile& file, NESHeader _header)
{
    header = _header;

    prg_rom.resize(header.prg_rom * 0x4000);

    if(header.chr_rom > 0)
        chr_rom.resize(header.chr_rom * 0x2000);
    else
        chr_ram.resize(0x2000);

    bool has_trainer = (header.flags6 & 0x04) != 0;
    if (has_trainer)
        file.seek(512);

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    size_t prgRamBanks = (header.prg_ram == 0) ? 1 : header.prg_ram;
    prg_ram.resize(prgRamBanks * 8 * 1024, 0);

    if (chr_rom.size() == 0 || header.chr_rom == 0)
        chr_ram.resize(0x2000);

    shift_reg = 0;
    shift_cnt = 0;

    reg_control = 0x0C;
    reg_chr0 = 0x00;
    reg_chr1 = 0x00;
    reg_prg = 0x00;

    up_orintation();
}

Mapper_1::~Mapper_1()
{

}

uint8_t Mapper_1::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000)
        return 0;

    uint32_t prgSize = (uint32_t)prg_rom.size();
    if (prgSize == 0)
        return 0;

    uint32_t mapped = map_prg_addr(addr);
    mapped %= prgSize;
    return prg_rom[mapped];
}

uint16_t Mapper_1::map_nametable_addr(uint16_t addr)
{
    addr &= 0x0FFF;
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

void Mapper_1::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (chr_ram.empty())
        return;

    addr &= 0x1FFF;
    uint32_t mapped = map_chr_addr(addr);
    mapped %= (uint32_t)chr_ram.size();
    chr_ram[mapped] = data;
}

void Mapper_1::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr < 0x8000)
        return;

    if (data & 0x80)
    {
        shift_reg = 0x10;
        reg_control |= 0x0C;
        up_orintation();
        return;
    }

    bool complete = (shift_reg & 0x01) != 0;
    shift_reg = (shift_reg >> 1) | ((data & 1) << 4);

    if (!complete)
        return;

    uint8_t value = shift_reg & 0x1F;
    shift_reg = 0x10;

    if(addr >= 0x8000 && addr <= 0x9FFF)
    {
        reg_control = value;
        up_orintation();
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF)
        reg_chr0 = value;
    else if(addr >= 0xC000 && addr <= 0xDFFF)
        reg_chr1 = value;
    else
        reg_prg = value;

}

uint8_t Mapper_1::mapper_read_chr(uint16_t addr)
{
    addr &= 0x1FFF;

    uint32_t mapped = map_chr_addr(addr);

    if (!chr_rom.empty())
    {
        mapped %= (uint32_t)chr_rom.size();
        return chr_rom[mapped];
    }

    if (!chr_ram.empty())
    {
        mapped %= (uint32_t)chr_ram.size();
        return chr_ram[mapped];
    }

    return 0;
}

uint8_t Mapper_1::read_prg_ram(uint16_t addr)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return 0;

    if (reg_prg & 0x10)
        return 0xFF;

    return prg_ram[(addr - 0x6000) % prg_ram.size()];
}

void Mapper_1::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return;

    if (reg_prg & 0x10)
        return;

    prg_ram[(addr - 0x6000) % prg_ram.size()] = data;
}

void Mapper_1::up_orintation()
{
    switch (reg_control & 0x03)
    {
    case 0:
        Orintation = ONESCREEN_LO;
        break;
    case 1:
        Orintation = ONESCREEN_HI;
        break;
    case 2:
        Orintation = VERTICAL;
        break;
    case 3:
        Orintation = HORIZONTAL;
        break;
    }
}

uint32_t Mapper_1::map_prg_addr(uint16_t addr)
{
    uint8_t prgMode = (reg_control >> 2) & 0x03;

    uint32_t prgBank16Count = (uint32_t)prg_rom.size() / (16 * 1024);
    uint32_t last16 = (prgBank16Count > 0) ? (prgBank16Count - 1) : 0;

    uint16_t off = addr & 0x3FFF;

    switch (prgMode)
    {
    case 1:
    {
        uint32_t bank32 = ((uint32_t)(reg_prg & 0x0F)) >> 1;
        uint32_t base = bank32 * (32 * 1024);
        return base + (addr & 0x7FFF);
    }
    case 2:
    {
        if (addr < 0xC000)
            return off;
        else
        {
            uint32_t bank16 = (uint32_t)(reg_prg & 0x0F);
            bank16 %= std::max(1u, prgBank16Count);
            return bank16 * (16 * 1024) + off;
        }
    }
    case 3:
    default:
    {
        if (addr < 0xC000)
        {
            uint32_t bank16 = (uint32_t)(reg_prg & 0x0F);
            bank16 %= std::max(1u, prgBank16Count);
            return bank16 * (16 * 1024) + off;
        }
        else
            return last16 * (16 * 1024) + off;
    }
    }
}

uint32_t Mapper_1::map_chr_addr(uint16_t addr)
{
    uint8_t chrMode = (reg_control >> 4) & 0x01;

    if (chrMode == 0)
    {
        uint32_t bank8 = ((uint32_t)(reg_chr0 & 0x1E)) >> 1; // 8KB bank index
        return bank8 * (8 * 1024) + (addr & 0x1FFF);
    }
    else
    {
        if (addr < 0x1000)
        {
            uint32_t bank4 = (uint32_t)(reg_chr0 & 0x1F);
            return bank4 * (4 * 1024) + (addr & 0x0FFF);
        }
        else
        {
            uint32_t bank4 = (uint32_t)(reg_chr1 & 0x1F);
            return bank4 * (4 * 1024) + (addr & 0x0FFF);
        }
    }
}

uint16_t Mapper_1::read_vector(uint16_t addr)
{
    uint16_t loAddr = addr;
    uint16_t hiAddr = (uint16_t)(addr + 1);


    auto rd = [&](uint16_t a) -> uint8_t
    {
        if (a < 0x8000 || prg_rom.empty())
            return 0;

        uint32_t m = map_prg_addr(a) % (uint32_t)prg_rom.size();
        return prg_rom[m];
    };

    uint8_t lo = rd(loAddr);
    uint8_t hi = rd(hiAddr);
    return (uint16_t)(lo | (hi << 8));
}

uint16_t Mapper_1::get_NMI()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
    else
        return 0;
}

uint16_t Mapper_1::get_RESET()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
    else
        return 0;
}

uint16_t Mapper_1::get_IRQ()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
    else
        return 0;
}
