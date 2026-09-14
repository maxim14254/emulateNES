#include "mapper_7.h"
#include "bus.h"
#include "global.h"

Mapper_7::Mapper_7(QFile& file, NESHeader _header, Bus* _bus) : bus(_bus)
{
    header = _header;

    prg_rom.resize(header.prg_rom * 0x4000);

    if (header.chr_rom > 0)
        chr_rom.resize(header.chr_rom * 0x2000);
    else
        chr_ram.resize(0x2000);

    if (header.prg_ram > 0)
        prg_ram.resize(header.prg_ram * 0x2000);
    else
        prg_ram.resize(0x2000);

    file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
    file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

    // Mapper 7 всегда одноэкранный, по умолчанию нижний
    Orintation = ONESCREEN_LO;

    prg_bank_count_8k = prg_rom.size() / 0x2000;
    prg_bank_count_32k = prg_rom.size() / 0x8000;
    if (prg_bank_count_32k == 0)
        prg_bank_count_32k = 1; // защита от деления на ноль

    prg_bank_32k = 0;
    update_banks();
}

Mapper_7::~Mapper_7()
{
}

void Mapper_7::update_banks()
{
    // 32 КБ банк занимает 4 слота по 8 КБ
    for (int i = 0; i < 4; ++i)
        prg_bank_map[i] = prg_bank_32k * 4 + i;
}

uint8_t Mapper_7::mapper_read_prg(uint16_t addr)
{
    if (addr < 0x8000 || prg_rom.empty())
        return 0;

    uint16_t slot = (addr - 0x8000) / 0x2000;
    uint16_t offset = (addr - 0x8000) & 0x1FFF;

    uint32_t bank = prg_bank_map[slot];
    uint32_t index = bank * 0x2000 + offset;

    if (index < prg_rom.size())
        return prg_rom[index];

    return 0;
}

uint8_t Mapper_7::mapper_read_chr(uint16_t addr)
{
    addr &= 0x1FFF;

    if (!chr_rom.empty())
    {
        if (addr < chr_rom.size())
            return chr_rom[addr];
    }
    else
    {
        if (addr < chr_ram.size())
            return chr_ram[addr];
    }

    return 0;
}

uint8_t Mapper_7::read_prg_ram(uint16_t addr)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return 0;

    return prg_ram[addr - 0x6000];
}

void Mapper_7::write_prg_ram(uint16_t addr, uint8_t data)
{
    if (addr < 0x6000 || addr > 0x7FFF || prg_ram.empty())
        return;

    prg_ram[addr - 0x6000] = data;
}

void Mapper_7::write_chr_ram(uint16_t addr, uint8_t data)
{
    if (addr >= 0x2000 || !chr_rom.empty() || chr_ram.empty())
        return;

    chr_ram[addr & 0x1FFF] = data;
}

void Mapper_7::mapper_write(uint16_t addr, uint8_t data)
{
    if (addr < 0x8000)
        return;

    // AxROM: биты 0-2 — номер 32КБ PRG банка, бит 4 — выбор одноэкранного зеркалирования
    uint8_t bank = data & 0x07;
    if (prg_bank_count_32k > 0)
        bank %= prg_bank_count_32k;

    prg_bank_32k = bank;

    if (data & 0x10)
        Orintation = ONESCREEN_HI;
    else
        Orintation = ONESCREEN_LO;

    update_banks();
}

uint16_t Mapper_7::map_nametable_addr(uint16_t addr)
{
    uint16_t offset = addr & 0x03FF;

    switch (Orintation)
    {
        case ONESCREEN_LO:
            return offset;
        case ONESCREEN_HI:
            return 0x0400 + offset;
        default:
            return offset;
    }
}

uint16_t Mapper_7::get_NMI()
{
    if (prg_rom.size() > 0)
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
    else
        return 0;
}

uint16_t Mapper_7::get_RESET()
{
    if (prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
    else
        return 0;
}

uint16_t Mapper_7::get_IRQ()
{
    if (prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
    else
        return 0;
}
