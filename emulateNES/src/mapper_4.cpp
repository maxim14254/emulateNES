#include "mapper_4.h"

Mapper_4::Mapper_4(QFile& file, NESHeader _header)
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

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    Orintation = (header.flags6 & 0x01) ? VERTICAL : HORIZONTAL;

    prg_bank_count_8k = (prg_rom.size() / 0x2000);

    if (!chr_rom.empty())
        chr_bank_count_1k = (chr_rom.size() / 0x0400);
    else
        chr_bank_count_1k = 0;

    bank_registers[6] = 0;
    bank_registers[7] = 1;

    update_banks();
}

Mapper_4::~Mapper_4()
{

}

void Mapper_4::update_banks()
{
    if (prg_bank_count_8k < 2)
        return;

    uint16_t last = (prg_bank_count_8k - 1);
    uint16_t second_last = (prg_bank_count_8k - 2);

    uint16_t r6 = bank_registers[6] % prg_bank_count_8k;
    uint16_t r7 = bank_registers[7] % prg_bank_count_8k;

    if (!prg_mode)
    {
        prg_bank_map[0] = r6;
        prg_bank_map[1] = r7;
        prg_bank_map[2] = second_last;
        prg_bank_map[3] = last;
    }
    else
    {
        prg_bank_map[0] = second_last;
        prg_bank_map[1] = r7;
        prg_bank_map[2] = r6;
        prg_bank_map[3] = last;
    }

    if (chr_bank_count_1k == 0)
        return;

    uint16_t r0 = (bank_registers[0] & 0xFE) % chr_bank_count_1k;
    uint16_t r1 = (bank_registers[1] & 0xFE) % chr_bank_count_1k;
    uint16_t r2 = bank_registers[2] % chr_bank_count_1k;
    uint16_t r3 = bank_registers[3] % chr_bank_count_1k;
    uint16_t r4 = bank_registers[4] % chr_bank_count_1k;
    uint16_t r5 = bank_registers[5] % chr_bank_count_1k;

    if (!chr_mode)
    {
        chr_bank_map[0] = r0;
        chr_bank_map[1] = (r0 + 1) % chr_bank_count_1k;
        chr_bank_map[2] = r1;
        chr_bank_map[3] = (r1 + 1) % chr_bank_count_1k;
        chr_bank_map[4] = r2;
        chr_bank_map[5] = r3;
        chr_bank_map[6] = r4;
        chr_bank_map[7] = r5;
    }
    else
    {
        chr_bank_map[0] = r2;
        chr_bank_map[1] = r3;
        chr_bank_map[2] = r4;
        chr_bank_map[3] = r5;
        chr_bank_map[4] = r0;
        chr_bank_map[5] = (r0 + 1) % chr_bank_count_1k;
        chr_bank_map[6] = r1;
        chr_bank_map[7] = (r1 + 1) % chr_bank_count_1k;
    }
}

uint8_t Mapper_4::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000 || prg_rom.empty())
        return 0;

    uint16_t slot = (addr - 0x8000) / 0x2000;
    uint16_t offset = (addr - 0x8000) % 0x2000;

    uint32_t bank = prg_bank_map[slot] % prg_bank_count_8k;
    uint32_t index = bank * 0x2000 + offset;

    if (index < prg_rom.size())
        return prg_rom[index];

    return 0;
}

void Mapper_4::clock_irq_on_a12(uint16_t addr)
{
    bool a12 = (addr & 0x1000) != 0;

    if (!a12)
    {
        a12_low_cycles++;
    }
    else
    {
        if (!last_a12 && a12_low_cycles >= 8)
        {
            if (irq_counter == 0 || irq_reload)
            {
                irq_counter = irq_latch;
                irq_reload = false;
            }
            else
            {
                irq_counter--;
            }

            if (irq_counter == 0 && irq_enabled)
                irq_pending = true;
        }

        a12_low_cycles = 0;
    }

    last_a12 = a12;
}

uint8_t Mapper_4::mapper_read_chr(uint16_t addr)
{
    if (addr >= 0x2000)
        return 0;

    clock_irq_on_a12(addr);

    uint16_t slot = addr / 0x0400;
    uint16_t offset = addr % 0x0400;

    uint32_t bank = chr_bank_map[slot] % chr_bank_count_1k;
    uint32_t index = bank * 0x0400 + offset;

    if (!chr_rom.empty())
    {
        if (index < chr_rom.size())
            return chr_rom[index];
    }
    else
    {
        if (index < chr_ram.size())
            return chr_ram[index];
    }

    return 0;
}

uint8_t Mapper_4::read_prg_ram(uint16_t addr)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return 0;

    if (!prg_ram_enable)
        return 0;

    return prg_ram[(addr - 0x6000) % prg_ram.size()];
}

void Mapper_4::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return;

    if (!prg_ram_enable || prg_ram_write_protect)
        return;

    prg_ram[(addr - 0x6000) % prg_ram.size()] = data;
}

void Mapper_4::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (addr >= 0x2000 || !chr_rom.empty() || chr_ram.empty())
        return;

    uint16_t slot = addr / 0x0400;
    uint16_t offset = addr % 0x0400;

    uint32_t bank = chr_bank_map[slot] % chr_bank_count_1k;
    uint32_t index = bank * 0x0400 + offset;

    if (index < chr_ram.size())
        chr_ram[index] = data;
}

void Mapper_4::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr < 0x8000)
        return;

    switch (addr & 0xE001)
    {
        case 0x8000:
        {
            bank_select = data & 0x07;
            prg_mode = (data & 0x40) != 0;
            chr_mode = (data & 0x80) != 0;
            update_banks();
            break;
        }
        case 0x8001:
        {
            bank_registers[bank_select] = data;
            update_banks();
            break;
        }
        case 0xA000:
        {
            if ((header.flags6 & 0x08) == 0)
            {
                Orintation = (data & 0x01) ? HORIZONTAL : VERTICAL;
            }
            break;
        }
        case 0xA001:
        {
            prg_ram_enable = (data & 0x80) != 0;
            prg_ram_write_protect = (data & 0x40) != 0;
            break;
        }
        case 0xC000:
        {
            irq_latch = data;
            break;
        }
        case 0xC001:
        {
            irq_reload = true;
            break;
        }
        case 0xE000:
        {
            irq_enabled = false;
            irq_pending = false;
            break;
        }
        case 0xE001:
        {
            irq_enabled = true;
            break;
        }
        default:
            break;
    }
}

uint16_t Mapper_4::map_nametable_addr(uint16_t addr)
{
    uint16_t table = addr / 0x0400;
    uint16_t offset = addr % 0x0400;

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

uint16_t Mapper_4::get_NMI()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
    else
        return 0;
}

uint16_t Mapper_4::get_RESET()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
    else
        return 0;
}

uint16_t Mapper_4::get_IRQ()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
    else
        return 0;
}
