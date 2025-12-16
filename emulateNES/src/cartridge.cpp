#include "cartridge.h"
#include <QFile>
#include <QMessageBox>



Cartridge::Cartridge(const QString& path, bool* status)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly))
    {
        file.read(reinterpret_cast<char*>(&header), 16);

        if(header.magic[0] == 'N' && header.magic[1] == 'E' && header.magic[2] == 'S')
        {
            prg_rom.resize(header.prg_rom * 0x4000);
            chr_rom.resize(header.chr_rom * 0x2000);

            if(header.prg_ram > 0)
                prg_ram.resize(header.prg_ram * 0x2000);
            else
                prg_ram.resize(0x2000);

            file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
            file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

            uint8_t map_lo =  header.flags6 >> 4;
            uint8_t map_hi =  header.flags7 & 0xF0;

            uint8_t mapper = (map_hi << 8) | map_lo;

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
    if (prg_rom.size() == 0x4000)
    {
        uint32_t offset = (addr - 0x8000) & 0x3FFF;
        return prg_rom[offset];
    }
    else
    {
        uint32_t offset = addr - 0x8000;
        return prg_rom[offset];
    }
}

uint8_t Cartridge::read_prg_ram(uint16_t addr)
{
    return prg_ram[addr - 0x6000];
}

void Cartridge::write_prg_ram(uint16_t addr, uint8_t data)
{
    prg_ram[addr - 0x6000] = data;
}

uint16_t Cartridge::get_NMI()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFA) | mapper_read_prg(0xFFFB) << 8;
}

uint16_t Cartridge::get_RESET()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFC) | mapper_read_prg(0xFFFD) << 8;
}

uint16_t Cartridge::get_IRQ()
{
    if(prg_rom.size() > 0)
        return mapper_read_prg(0xFFFE) | mapper_read_prg(0xFFFF) << 8;
}
