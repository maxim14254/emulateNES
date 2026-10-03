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
#include <QEventLoop>
#include <QTimer>



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
    std::thread([&]()
    {
        QFile file(QString("%1save_%2.sav").arg(SaveDir).arg(saveNumb));

        if (file.open(QIODevice::WriteOnly))
        {
            QByteArray array;
            QDataStream out(&array, QIODevice::WriteOnly);


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

            QMetaObject::invokeMethod(&w, [&]()
            {
                w.show_text(QString("Сохранено в слот:%1").arg(saveNumb));
            });
        }
        else
        {
            QMessageBox message(QMessageBox::Icon::Information, "Ошибка", QString("Не удалось сохранить в файл %1").arg(file.fileName()), QMessageBox::StandardButton::Ok);
            message.exec();
        }

    }).detach();
}

void SaveLoad::Load()
{
    QFile file(QString("%1save_%2.sav").arg(SaveDir).arg(saveNumb));

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox message(QMessageBox::Icon::Information, "Ошибка", QString("Не удалось загрузить файл %1").arg(file.fileName()), QMessageBox::StandardButton::Ok);
        message.exec();
        return;
    }

    QByteArray array = file.readAll();
    file.close();

    std::thread([&](QByteArray&& array)
    {
        QDataStream in(&array, QIODevice::ReadOnly);

        QString p;
        in >> p;

        if (p != cpu.path)
        {
            QMetaObject::invokeMethod(&w, [&]()
            {
                w.show_text("Неверный ROM");
            });

            return;
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

        if (netPlay.isConnnection())
        {

            Data d;
            d.header = "load";
            d.frame = 0;
            d.controller = 0;
            d.data = array;

            netPlay.clearBuffers();

            QEventLoop loop;
            QTimer timeout;
            timeout.setSingleShot(true);
            QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);

            netPlay.set_load_sucsess_callback([&loop]()
            {
                loop.quit();
            });

            netPlay.writeDatagram(d);

            timeout.start(5000);
            loop.exec();

            netPlay.set_load_sucsess_callback(nullptr);
        }

        QMetaObject::invokeMethod(&w, [&]()
        {
            w.show_text(QString("Загружен слот:%1").arg(saveNumb));
        });

    }, std::move(array)).detach();

}

void SaveLoad::LoadSaveFromNet(QByteArray& array)
{
    std::thread([&](QByteArray&& array)
    {
        QDataStream in(&array, QIODevice::ReadOnly);

        QString p;
        in >> p;

        if (p != cpu.path)
        {
            QMetaObject::invokeMethod(&w, [&]()
            {
                w.show_text("Неверный ROM");
            });

            Data d;
            d.header = "load_fail";
            d.frame = 0;
            d.controller = 0;
            d.data = "";

            netPlay.writeDatagram(d);
            return;
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

        Data d;
        d.header = "load_sucsess";
        d.frame = 0;
        d.controller = 0;
        d.data = "";

        netPlay.writeDatagram(d);

        QMetaObject::invokeMethod(&w, [&]()
        {
            w.show_text(QString("Загружен слот:%1").arg(saveNumb));
        });


        break_wait = false;


    }, std::move(array)).detach();
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

    Data d;
    d.header = "new_game";
    d.frame = ppu.getFrame() = 0;
    d.controller = 0;
    d.data = array;

    netPlay.writeDatagram(d);

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
        w.show_text("Не удалось загрузить ROM");
        return;
    }

    //APU
    if(apu)
        in >> *apu;

}
