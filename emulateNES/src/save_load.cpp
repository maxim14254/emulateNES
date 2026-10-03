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



SaveLoad::SaveLoad(CPU& _cpu, Bus& _bus, PPU& _ppu, APU* _apu, NetPlay& _netPlay, MainWindow& _w) : cpu(_cpu), bus(_bus), ppu(_ppu), apu(_apu), netPlay(_netPlay), w(_w)
{
    SaveDir = QCoreApplication::applicationDirPath() + "/saves/";

    QDir dir;
    dir.mkpath(SaveDir);

    cpu.set_save_callback(std::bind(&SaveLoad::Save, this));
    cpu.set_load_callback(std::bind(&SaveLoad::Load, this));
    cpu.set_start_newgame_for_net_callback(std::bind(&SaveLoad::StartGameForNet, this));
    cpu.set_load_newgame_for_net_callback(std::bind(&SaveLoad::LoadGameForNet, this, std::placeholders::_1));

    cpu.chande_slot_callback = [&]()->uint8_t& { return saveNumb; };

    netPlay.set_load_callback(std::bind(&SaveLoad::LoadSaveFromNet, this, std::placeholders::_1));
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

        if(netPlay.isConnnection())
        {
            QByteArray array2;

            array2.push_back("load");
            array2.push_back(array);

            Data d;
            d.frame = 0;
            d.controller = 0;
            d.startGame = array2;

            netPlay.clearBuffers();
            netPlay.writeDatagram(d);
        }

        w.show_text(QString("Загружен слот:%1").arg(saveNumb));
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Information, "Ошибка", QString("Не удалось загрузить файл %1").arg(file.fileName()), QMessageBox::StandardButton::Ok);
        message.exec();
    }
}

void SaveLoad::LoadSaveFromNet(QByteArray &&array)
{
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

    break_wait = true;
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

    break_wait = false;

}

void SaveLoad::StartGameForNet()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);

    //CPU регистры
    out << cpu;

    //PPU регистры
    out << ppu;

    //Bus
    out << bus;

    //APU
    if(apu)
        out << *apu;

    Data data;
    data.frame = ppu.getFrame() = 0;
    data.controller = 0;
    data.startGame = array;


    netPlay.writeDatagram(data);
}

void SaveLoad::LoadGameForNet(QByteArray &array)
{
    QDataStream in(&array, QIODevice::ReadOnly);

    if(!_update)
    {
        std::lock_guard<std::mutex> lg(update_frame_mutex);
        _update = true;

        cv.notify_one();
    }

    if(start)
        start = false;

    std::lock_guard<std::mutex> lock(cpu.mutex_stop);

    netPlay.clearBuffers();

    in >> cpu.path;

    //CPU регистры
    in >> cpu;

    //PPU регистры
    in >> ppu;

    //Bus
    bool status;
    bus.init_new_cartridge(cpu.path, &status);

    if(status)
        in >> bus;
    else
    {
        w.show_text(QString("Не удалось загрузить ROM"));
        return;
    }

    //APU
    if(apu)
        in >> *apu;

}
