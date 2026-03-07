#include "mapper.h"


Mapper::Mapper()
{

}

Mapper::~Mapper()
{

}

uint16_t Mapper::get_NMI()
{
    if(prg_rom.size() > 0)
    {
        return prg_rom[0xFFFA] | prg_rom[0xFFFB] << 8;
    }
    else
        return 0;
}

uint16_t Mapper::get_RESET()
{
    if(prg_rom.size() > 0)
        return prg_rom[0xFFFC] | prg_rom[0xFFFD] << 8;
    else
        return 0;
}

uint16_t Mapper::get_IRQ()
{
    if(prg_rom.size() > 0)
        return prg_rom[0xFFFE] | prg_rom[0xFFFF] << 8;
    else
        return 0;
}
