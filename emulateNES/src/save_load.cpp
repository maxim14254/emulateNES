#include "save_load.h"
#include <QFile>
#include <QCoreApplication>
#include <QMessageBox>
#include <QByteArray>
#include <QDir>
#include "cartridge.h"
#include "mapper_4.h"
#include "mainwindow.h"
#include "global.h"


SaveLoad::SaveLoad(CPU& _cpu, Bus& _bus, PPU& _ppu, APU* _apu, MainWindow& _w) : cpu(_cpu), bus(_bus), ppu(_ppu), apu(_apu), w(_w)
{
    SaveDir = QCoreApplication::applicationDirPath() + "/saves/";

    QDir dir;
    dir.mkpath(SaveDir);

    cpu.set_save_callback(std::bind(&SaveLoad::Save, this));
    cpu.set_load_callback(std::bind(&SaveLoad::Load, this));
    cpu.chande_slot_callback = [&]()->uint8_t& { return saveNumb; };
}

void SaveLoad::Save()
{
    QFile file(QString("%1save_%2.sav").arg(SaveDir).arg(saveNumb));

    if (file.open(QIODevice::WriteOnly))
    {
        QByteArray array;
        QDataStream out(&array, QIODevice::WriteOnly);

        if(!_update)
        {
            std::lock_guard<std::mutex> lg(update_frame_mutex);
            _update = true;

            cv.notify_one();
        }

        std::lock_guard<std::mutex> lock(cpu.mutex_stop);
        //CPU регистры
        out << cpu;

        //PPU регистры
        out << ppu;

        //Bus
        out << bus;

        //APU
        if(apu)
            out << *apu;

        file.write(array);
        file.close();

        w.show_text(QString("Сохранено в слот:%1").arg(saveNumb));
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

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray array = file.readAll();
        file.close();

        QDataStream in(&array, QIODevice::ReadOnly);

        QString p;
        in >> p;

        if(p != cpu.path)
        {
            w.show_text(QString("Неверный ROM"));
            return;
        }

        if(!_update)
        {
            std::lock_guard<std::mutex> lg(update_frame_mutex);
            _update = true;

            cv.notify_one();
        }

        std::lock_guard<std::mutex> lock(cpu.mutex_stop);

        //CPU регистры
        in >> cpu;

        //PPU регистры
        in >> ppu;

        //Bus
        in >> bus;

        //APU
        if(apu)
            in >> *apu;

        w.show_text(QString("Загружен слот:%1").arg(saveNumb));
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Information, "Ошибка", QString("Не удалось загрузить файл %1").arg(file.fileName()), QMessageBox::StandardButton::Ok);
        message.exec();
    }
}
