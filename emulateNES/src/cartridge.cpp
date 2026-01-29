#include "cartridge.h"
#include <QFile>
#include <QMessageBox>



Cartridge::Cartridge(const QString& path, bool* status)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly))
    {
        file.read(reinterpret_cast<char*>(&header), 16);

        if(header.magic[0] == 'N' && header.magic[1] == 'E' && header.magic[2] == 'S' && header.magic[3] == 0x1A)
        {
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

            uint8_t map_lo =  header.flags6 >> 4;
            uint8_t map_hi =  header.flags7 & 0xF0;

            uint8_t mapper = (map_hi << 8) | map_lo;

            Orintation = map_lo & 0x01 ? VERTICAL : HORIZONTAL;

            if(mapper == 0)
            {
                // NROM
            }
            else if(mapper == 2)
            {
                // UNROM
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

//    if (addr >= 0x8000 && addr <= 0xFFFF)
//    {
//        auto mapped_addr =  addr & (header.prg_rom > 1 ? 0x7FFF : 0x3FFF);
//        return prg_rom[mapped_addr];

//    }

    return 0;
}

uint8_t Cartridge::mapper_read_chr(uint16_t addr)
{
    if(chr_rom.size() > 0)
        return chr_rom[addr];
    else
        return chr_ram[addr];
}

uint8_t Cartridge::read_prg_ram(uint16_t addr)
{
    return prg_ram[addr - 0x6000];
}

void Cartridge::write_prg_ram(uint16_t addr, uint8_t data)
{
    prg_ram[addr - 0x6000] = data;
}

void Cartridge::write_chr_ram(uint16_t addr, uint8_t data)
{
    if(chr_ram.size() > 0)
        chr_ram[addr] = data;
}

uint16_t Cartridge::map_nametable_addr(uint16_t addr)
{
    return addr; // TO DO
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
