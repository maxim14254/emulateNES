#include "cartridge.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include "mapper_0.h"
#include "mapper_1.h"
#include "mapper_3.h"


Cartridge::Cartridge(const QString& path, bool* status)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly))
    {
        NESHeader header;
        file.read(reinterpret_cast<char*>(&header), 16);

        if(header.magic[0] == 'N' && header.magic[1] == 'E' && header.magic[2] == 'S' && header.magic[3] == 0x1A)
        {
            uint8_t map_lo = (header.flags6 >> 4) & 0x0F;
            uint8_t map_hi = (header.flags7 >> 4) & 0x0F;;

            uint8_t map = (map_hi << 4) | map_lo;

            if(map == 0)
            {
                mapper.reset(new Mapper_0(file, header));
            }
            else if(map == 1)
            {
                mapper.reset(new Mapper_1(file, header));
            }
            else if(map == 2)
            {
                // UNROM
            }
            else if(map == 3)
            {
                mapper.reset(new Mapper_3(file, header));
            }

            *status = true;
        }
        else
        {
            QMessageBox message(QMessageBox::Icon::Critical, "Ошибка", "Неверный формат", QMessageBox::StandardButton::Ok);
            message.exec();
            *status = false;
        }
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Ошибка", "Не удается открыть файл", QMessageBox::StandardButton::Ok);
        message.exec();
        *status = false;
    }

    file.close();
}

Cartridge::~Cartridge()
{

}

uint8_t Cartridge::mapper_read_prg(uint16_t addr)
{
    if(mapper)
        return mapper->mapper_read_prg(addr);
    else
        return 0;
}

uint8_t Cartridge::mapper_read_chr(uint16_t addr)
{
    if(mapper)
        return mapper->mapper_read_chr(addr);
    else
        return 0;
}

uint8_t Cartridge::read_prg_ram(uint16_t addr)
{
    if(mapper)
        return mapper->read_prg_ram(addr);
    else
        return 0;
}

void Cartridge::write_prg_ram(uint16_t addr, uint8_t data)
{
    if(mapper)
        mapper->write_prg_ram(addr, data);
}

void Cartridge::write_chr_ram(uint16_t addr, uint8_t data)
{
    if(mapper)
        mapper->write_chr_ram(addr, data);
}

void Cartridge::mapper_write(uint16_t addr, uint8_t data)
{
    if(mapper)
        mapper->mapper_write(addr, data);
}

uint16_t Cartridge::map_nametable_addr(uint16_t addr)
{
    if(mapper)
        return mapper->map_nametable_addr(addr);
    else
        return 0;
}

uint16_t Cartridge::get_NMI()
{
    if(mapper)
        return mapper->get_NMI();
    else
        return 0;
}

uint16_t Cartridge::get_RESET()
{
    if(mapper)
        return mapper->get_RESET();
    else
        return 0;
}

uint16_t Cartridge::get_IRQ()
{
    if(mapper)
        return mapper->get_IRQ();
    else
        return 0;
}

int Cartridge::get_orintation()
{
    if(mapper)
        return mapper->Orintation;
    else
        return 0;
}
