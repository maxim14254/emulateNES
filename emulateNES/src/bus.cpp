#include "bus.h"
#include "cartridge.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>


Bus::Bus()
{
    ram.resize(0x800);
}

Bus::~Bus()
{

}

uint8_t Bus::read(uint16_t addr)
{
    if (addr < 0x2000) // ОЗУ
    {
        return ram[addr & 0x07FF];
    }
    else if(addr < 0x4000) //PPU
    {

    }
    else if(addr < 0x4020) //APU и ввода/вывода
    {
        return 0xFF;
    }
    else if(addr < 0x8000) // ОЗУ картриджа
    {
        return cartridge->read_prg_ram(addr);
    }
    else  //область картриджа
    {
        return cartridge->mapper_read_prg(addr);
    }
}

void Bus::write(uint16_t addr, uint8_t data)
{

    if (addr < 0x2000) // ОЗУ
    {
        ram[addr & 0x07FF] = data;
    }
    else if(addr < 0x4000) //PPU
    {

    }
    else if(addr < 0x4020) //APU и ввода/вывода
    {

    }
    else if(addr < 0x8000) // ОЗУ картриджа
    {
        cartridge->write_prg_ram(addr, data);
    }
    else //область картриджа
    {

    }

}

void Bus::init_new_cartridge(const QString& path, bool* status)
{
    cartridge.reset(new Cartridge(path, status));

    if(!status)
        return;

    std::fill(ram.begin(), ram.end(), 0);
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
