#include "bus.h"
#include "cartridge.h"
#include <QString>
#include <QDir>
#include <QDebug>
#include "ppu.h"
#include "cpu.h"
#include "global.h"
#include "apu.h"


bool linear_reload_flag = false;
uint8_t linear_counter_reload = 0;
uint8_t linear_counter = 0;
bool control_flag = false;


Bus::Bus()
{
    ram.resize(0x800);
    vram.resize(0x800);
    palette.resize(0x20);
}

Bus::~Bus()
{

}

uint8_t Bus::read_cpu(uint16_t addr, bool onlyRead)
{
    if (addr < 0x2000) // ОЗУ
    {
        return ram[addr & 0x07FF];
    }
    else if(addr >= 0x2000 && addr <= 0x2007) // регистры PPU
    {
        return ppu->get_register(addr, onlyRead);
    }
    else if(addr >= 0x2008 && addr <= 0x3FFF) // зеркало PPU
    {
        uint16_t reg = 0x2000 + (addr & 0x0007);
        return ppu->get_register(reg, onlyRead);
    }
    else if(addr >= 0x4000 && addr <= 0x4017) // APU и ввода/вывода
    {
        if(addr == 0x4016 || addr == 0x4017) // джойстики
        {
            bool rez = (controller[addr & 0x0001] & 0x80) > 0;
            controller[addr & 0x0001] <<= 1;

            return rez;
        }
        else
            return 0x00; // TO DO
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
        if(addr == 0x4000)
        {
            switch ((data & 0xC0) >> 6)
            {
            case 0x00:
                pulse1.duty = 0.125;
                pulse1.sequencer.sequence = 0b01000000;
                break;
            case 0x01:
                pulse1.duty = 0.25;
                pulse1.sequencer.sequence = 0b01100000;
                break;
            case 0x02:
                pulse1.duty = 0.5;
                pulse1.sequencer.sequence = 0b01111000;
                break;
            case 0x03:
                pulse1.duty = 0.75;
                pulse1.sequencer.sequence = 0b10011111;
                break;
            }

            pulse1.envelope.bLoop = (data & 0x20) > 0;
            pulse1.envelope.disable = (data & 0x10) > 0;
            pulse1.envelope.volume = (data & 0x0F);
        }
        else if(addr == 0x4001)
        {
            pulse1.sweep.enabled = (data & 0x80) > 0;
            pulse1.sweep.period = (data & 0x70) >> 4;
            pulse1.sweep.negate = (data & 0x08) > 0;
            pulse1.sweep.shift = data & 0x07;
            pulse1.sweep.reload = true;
        }
        else if(addr == 0x4002)
        {
            pulse1.sequencer.reload = (pulse1.sequencer.reload & 0xFF00) | data;
        }
        else if(addr == 0x4003)
        {
            pulse1.sequencer.reload = pulse1.sequencer.timer = (data & 0x07) << 8 | (pulse1.sequencer.reload & 0x00FF);;
            uint8_t length = (data & 0xF8) >> 3;

            if (pulse1_enable)
                pulse1.length_counter = LENGTH_TABLE[length];

            pulse1.envelope.start = true;
        }
        else if(addr == 0x4004)
        {
            switch ((data & 0xC0) >> 6)
            {
            case 0x00:
                pulse2.duty = 0.125;
                pulse2.sequencer.sequence = 0b01000000;
                break;
            case 0x01:
                pulse2.duty = 0.25;
                pulse2.sequencer.sequence = 0b01100000;
                break;
            case 0x02:
                pulse2.duty = 0.5;
                pulse2.sequencer.sequence = 0b01111000;
                break;
            case 0x03:
                pulse2.duty = 0.75;
                pulse2.sequencer.sequence = 0b10011111;
                break;
            }

            pulse2.envelope.bLoop = (data & 0x20) > 0;
            pulse2.envelope.disable = (data & 0x10) > 0;
            pulse2.envelope.volume = (data & 0x0F);
        }
        else if(addr == 0x4005)
        {
            pulse2.sweep.enabled = (data & 0x80) > 0;
            pulse2.sweep.period = (data & 0x70) >> 4;
            pulse2.sweep.negate = (data & 0x08) > 0;
            pulse2.sweep.shift = data & 0x07;
            pulse2.sweep.reload = true;

        }
        else if(addr == 0x4006)
        {
            pulse2.sequencer.reload = (pulse2.sequencer.reload & 0xFF00) | data;
        }
        else if(addr == 0x4007)
        {
            pulse2.sequencer.reload = pulse2.sequencer.timer = (data & 0x07) << 8 | (pulse2.sequencer.reload & 0x00FF);;
            uint8_t length = (data & 0xF8) >> 3;

            if (pulse2_enable)
                pulse2.length_counter = LENGTH_TABLE[length];

            pulse2.envelope.start = true;;
        }
        else if(addr == 0x4008)
        {
            control_flag = (data & 0x80) > 0;
            linear_counter_reload = data & 0x7F;
        }
        else if(addr == 0x400A)
        {
            triangle_timer_lo = data;
        }
        else if(addr == 0x400B)
        {
            linear_reload_flag = true;
            uint8_t length_counter = (data & 0xF8) >> 3;

            triangle_timer = (data & 0x07) << 8 | triangle_timer_lo;
        }
        else if(addr == 0x400C)
        {
            noise.envelope.bLoop =  (data & 0x20) > 0;
            noise.envelope.disable =  (data & 0x10) > 0;
            noise.envelope.volume = (data & 0x0F);
        }
        else if(addr == 0x400E)
        {
            noise.shortMode = (data & 0x80) > 0;
            noise.periodIndex = data & 0x0F;
        }
        else if(addr == 0x400F)
        {
            if (noise_enable)
                noise.length_counter = LENGTH_TABLE[(data & 0xF8) >> 3];

            noise.envelope.start = true;
        }
        else if(addr == 0x4014) // DMA
            ppu->set_oam(&ram[data << 8]);
        else if(addr == 0x4015)
        {
            pulse1_enable = data & 0x01;
            pulse2_enable = data & 0x02;
            noise_enable = data & 0x04;
            triangle_enable = data & 0x08;
        }
        else if(addr == 0x4016 || addr == 0x4017) // джойстики
        {
            controller[addr & 0x0001] = cpu->get_gamepad(addr & 0x0001);

            if(linear_reload_flag)
            {
                linear_counter = linear_counter_reload;
            }
            else if(linear_counter > 0)
            {
                --linear_counter;
            }

            if(!control_flag)
            {
                linear_reload_flag = false;
            }
        }

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
        uint8_t data = 0;

        if (cartridge->Orintation == Cartridge::MIRROR::VERTICAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
        }
        else if (cartridge->Orintation == Cartridge::MIRROR::HORIZONTAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
        }

        return data;
    }
    else if(addr >= 0x3000 && addr <= 0x3EFF) // зеркало VRAM
    {
        uint16_t nt_index = cartridge->map_nametable_addr((addr - 0x2000) & 0x0FFF);
        uint8_t data = 0;

        if (cartridge->Orintation == Cartridge::MIRROR::VERTICAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
        }
        else if (cartridge->Orintation == Cartridge::MIRROR::HORIZONTAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                data = vram[nt_index & 0x03FF];
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                data = vram[0x0400 + (nt_index & 0x03FF)];
        }

        return data;
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

        return palette[palette_addr] & (ppu->get_register(0x2001, true) & 0x01 ? 0x30 : 0x3F);
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

        if (cartridge->Orintation == Cartridge::MIRROR::VERTICAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
        }
        else if (cartridge->Orintation == Cartridge::MIRROR::HORIZONTAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
        }
    }
    else if(addr >= 0x3000 && addr <= 0x3EFF) // зеркало VRAM
    {
        uint16_t nt_index = cartridge->map_nametable_addr((addr - 0x2000) & 0x0FFF);

        if (cartridge->Orintation == Cartridge::MIRROR::VERTICAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
        }
        else if (cartridge->Orintation == Cartridge::MIRROR::HORIZONTAL)
        {
            if (nt_index >= 0x0000 && nt_index <= 0x03FF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0400 && nt_index <= 0x07FF)
                vram[nt_index & 0x03FF] = data;
            if (nt_index >= 0x0800 && nt_index <= 0x0BFF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
            if (nt_index >= 0x0C00 && nt_index <= 0x0FFF)
                vram[0x0400 + (nt_index & 0x03FF)] = data;
        }
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

void Bus::init_APU(APU *_apu)
{
    apu = _apu;
}

void Bus::run_steps_ppu(int cycles)
{
    ppu->run(cycles);
}

void Bus::run_steps_apu(int cycles)
{
    apu->run(cycles);
}

void Bus::cpu_request_nmi()
{
    cpu->request_nmi();
}

void Bus::reset_ppu()
{
    ppu->reset();
}

void Bus::run_watch_all_tiles()
{
    ppu->run_watch_all_tiles();
}

void Bus::run_watch_cpu_instr(uint16_t PC)
{
    ppu->run_watch_cpu_instr(PC);
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

uint16_t Bus::get_PC()
{
    return cpu->get_PC();
}

uint8_t Bus::get_SP()
{
    return cpu->get_SP();
}

uint8_t Bus::get_statusCPU()
{
    return cpu->get_statusCPU();
}

uint8_t Bus::get_A()
{
    return cpu->get_A();
}

uint8_t Bus::get_X()
{
    return cpu->get_X();
}

uint8_t Bus::get_Y()
{
    return cpu->get_Y();
}
