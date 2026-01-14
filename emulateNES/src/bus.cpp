#include "bus.h"
#include "cartridge.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "ppu.h"
#include "cpu.h"


Bus::Bus()
{
    ram.resize(0x800);
    vram.resize(0x800);
    palette.resize(0x20);
}

Bus::~Bus()
{

}

uint8_t Bus::read_cpu(uint16_t addr)
{
    if (addr < 0x2000) // ОЗУ
    {
        return ram[addr & 0x07FF];
    }
    else if(addr >= 0x2000 && addr <= 0x2007) // регистры PPU
    {
        return ppu->get_register(addr);
    }
    else if(addr >= 0x2008 && addr <= 0x3FFF) // зеркало PPU
    {
        uint16_t reg = 0x2000 + (addr & 0x0007);
        return ppu->get_register(reg);
    }
    else if(addr >= 0x4000 && addr <= 0x4017) // APU и ввода/вывода
    {
        return 0xFF; // TO DO
    }
    else if(addr >= 0x5000 && addr <= 0x5FFF) // расширение ПЗУ\ОЗУ
    {
        return cartridge->mapper_read_prg(addr);
    }
    else if(addr >= 0x6000 && addr <= 0x7FFF) // ОЗУ картриджа
    {
        return cartridge->read_prg_ram(addr);
    }
    else  // область картриджа
    {
        return cartridge->mapper_read_prg(addr);
    }
}

void Bus::write_cpu(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000) // ОЗУ
    {
        ram[addr & 0x07FF] = data;
    }
    else if(addr >= 0x2000 && addr <= 0x2007) // запись регистры PPU
    {
        ppu->set_register(addr, data);
    }
    else if(addr >= 0x2008 && addr <= 0x3FFF) // зеркало PPU
    {
        uint16_t reg = 0x2000 + (addr & 0x0007);
        ppu->set_register(reg, data);
    }
    else if(addr >= 0x4000 && addr <= 0x4017) // APU и ввода/вывода DMA
    {
        if(addr == 0x4014)
            ppu->set_oam(&ram[data << 8]);
    }
    else if(addr >= 0x5000 && addr <= 0x5FFF) // расширение ПЗУ\ОЗУ
    {
        // cartridge->mapper_write_prg(addr, data); TO DO
    }
    else if(addr >= 0x6000 && addr <= 0x7FFF) // ОЗУ картриджа
    {
        cartridge->write_prg_ram(addr, data);
    }
    else // область картриджа
    {
        // cartridge->mapper_write_prg(addr, data); TO DO
    }
}

uint8_t Bus::read_ppu(uint16_t addr)
{
    addr &= 0x3FFF;

    if (addr < 0x2000) // область картриджа
    {
        return cartridge->mapper_read_chr(addr);
    }
    else if(addr >= 0x2000 && addr <= 0x2FFF) // VRAM
    {
        uint16_t nt_index = cartridge->map_nametable_addr((addr - 0x2000) & 0x0FFF);
        return vram[nt_index];
    }
    else if(addr >= 0x3000 && addr <= 0x3EFF) // зеркало VRAM
    {
        uint16_t nt_index = cartridge->map_nametable_addr((addr - 0x2000) & 0x0FFF);
        return vram[nt_index];
    }
    else if(addr >= 0x3F00 && addr <= 0x3FFF) // палитра
    {
        uint8_t palette_addr = addr & 0x1F;

        if (palette_addr == 0x10)
            palette_addr = 0x00;
        else if (palette_addr == 0x14)
            palette_addr = 0x04;
        else if (palette_addr == 0x18)
            palette_addr = 0x08;
        else if (palette_addr == 0x1C)
            palette_addr = 0x0C;

        return palette[palette_addr];
    }
    else
        return 0;
}

void Bus::write_ppu(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;

    if (addr < 0x2000) // область картриджа
    {
        cartridge->write_chr_ram(addr, data);
    }
    else if(addr >= 0x2000 && addr <= 0x2FFF) // VRAM
    {
        uint16_t nt_index = cartridge->map_nametable_addr((addr - 0x2000) & 0x0FFF);
        vram[nt_index] = data;
    }
    else if(addr >= 0x3000 && addr <= 0x3EFF) // зеркало VRAM
    {
        uint16_t nt_index = cartridge->map_nametable_addr((addr - 0x2000) & 0x0FFF);
        vram[nt_index] = data;
    }
    else if(addr >= 0x3F00 && addr <= 0x3FFF) // палитра
    {
        uint8_t palette_addr = addr & 0x1F;

        if (palette_addr == 0x10)
            palette_addr = 0x00;
        else if (palette_addr == 0x14)
            palette_addr = 0x04;
        else if (palette_addr == 0x18)
            palette_addr = 0x08;
        else if (palette_addr == 0x1C)
            palette_addr = 0x0C;

        palette[palette_addr] = data;
    }
}

void Bus::init_new_cartridge(const QString& path, bool* status)
{
    cartridge.reset(new Cartridge(path, status));

    if(!status)
        return;

    std::fill(ram.begin(), ram.end(), 0);
}

void Bus::init_PPU(PPU *_ppu)
{
    ppu = _ppu;
}

void Bus::init_CPU(CPU *_cpu)
{
    cpu = _cpu;
}

void Bus::run_steps_ppu(int cycles)
{
    ppu->run(cycles);
}

void Bus::cpu_request_nmi()
{
    cpu->request_nmi();
}

void Bus::reset_ppu()
{
    ppu->reset();
}

uint16_t Bus::get_NMI()
{
    return cartridge->get_NMI();
}

uint16_t Bus::get_RESET()
{
    return cartridge->get_RESET();
}

uint16_t Bus::get_IRQ()
{
    return cartridge->get_IRQ();
}
