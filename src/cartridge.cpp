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

            file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());
            file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());

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

uint16_t Cartridge::get_NMI()
{
    if(prg_rom.size() > 0)
        return prg_rom[prg_rom.size() - 6] | (prg_rom[prg_rom.size() - 5] << 8);
}

uint16_t Cartridge::get_RESET()
{
    if(prg_rom.size() > 0)
        return prg_rom[prg_rom.size() - 4] | (prg_rom[prg_rom.size() - 3] << 8);
}

uint16_t Cartridge::get_IRQ()
{
    if(prg_rom.size() > 0)
        return prg_rom[prg_rom.size() - 2] | (prg_rom[prg_rom.size() - 1] << 8);
}
