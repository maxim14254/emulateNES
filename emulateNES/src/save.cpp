#include "save.h"
#include <QFile>
#include <QCoreApplication>
#include <QMessageBox>
#include <QByteArray>
#include <QDir>
#include "cartridge.h"
#include "mapper_4.h"


SaveLoad::SaveLoad(CPU& _cpu, Bus& _bus, PPU& _ppu) : cpu(_cpu), bus(_bus), ppu(_ppu)
{
    SaveDir = QCoreApplication::applicationDirPath() + "/saves/";

    QDir dir;
    dir.mkpath(SaveDir);

    cpu.set_save_callback(std::bind(&SaveLoad::Save, this));
    cpu.set_load_callback(std::bind(&SaveLoad::Load, this));
}

void SaveLoad::Save()
{
    QFile file(QString("%1save_%2.sav").arg(SaveDir).arg(saveNumb));

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QByteArray array;
        QDataStream out(&array, QIODevice::WriteOnly);

        std::lock_guard<std::mutex> lock(cpu.mutex_stop);
        //CPU регистры
        out << cpu;

        //PPU регистры
        out << ppu;

        // Маппер
        if(bus.cartridge->map == 4)
        {
            Mapper_4* mapper = dynamic_cast<Mapper_4*>(bus.cartridge->mapper.get());
            out << *mapper;
        }

        //APU

        file.write(array);
        file.close();
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Information, "Ошибка", QString("Не удалось сохранить в файл %1").arg(file.fileName()), QMessageBox::StandardButton::Ok);
        message.exec();
    }
}

void SaveLoad::Load()
{
    QFile file(QString("%1save_%2.sav").arg(SaveDir).arg(saveNumb));

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray array = file.readAll();
        file.close();

        QDataStream in(&array, QIODevice::ReadOnly);

        std::lock_guard<std::mutex> lock(cpu.mutex_stop);
        //CPU регистры
        in >> cpu;

        //PPU регистры
        in >> ppu;

        // Маппер
        if(bus.cartridge->map == 4)
        {
            Mapper_4* mapper = dynamic_cast<Mapper_4*>(bus.cartridge->mapper.get());
            in >> *mapper;
        }

        //APU
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Information, "Ошибка", QString("Не удалось загрузить файл %1").arg(file.fileName()), QMessageBox::StandardButton::Ok);
        message.exec();
    }
}
