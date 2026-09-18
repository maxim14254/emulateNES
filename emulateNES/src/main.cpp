#include "mainwindow.h"
#include <QApplication>
#include "cpu.h"
#include <QSurfaceFormat>
#include <QMetaObject>
#include "bus.h"
#include "ppu.h"
#include "apu.h"
#include "save_load.h"
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QMessageBox>


#ifdef LOG_ON
#include "log.h"
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);

    MainWindow w;
    w.show();

    Bus bus;

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setCodec("audio/pcm");
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo dev = QAudioDeviceInfo::defaultOutputDevice();

    APU* apu = nullptr;
    if (dev.isFormatSupported(format))
    {
        QAudioOutput* sink = new QAudioOutput(dev, format);
        sink->setBufferSize(16384);
        sink->setNotifyInterval(5);

        apu = new APU(format.sampleRate(), &bus, sink);
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Error", "Аудио устройство не поддерживает аудио формат:\nSampleRate(48000)\nCodec(audio/pcm)\nSampleSize(32)\nSampleType(SignedInt)\nByteOrder(LittleEndian)",
                            QMessageBox::StandardButton::Ok);
        message.exec();
    }

    bus.init_APU(apu);

    PPU ppu(&w, &bus);
    bus.init_PPU(&ppu);

#ifdef LOG_ON
    LOG::Init(&ppu);
#endif

    CPU cpu(&w, &bus);
    bus.init_CPU(&cpu);
   // bool rez = cpu.slot_init_new_cartridge(":/games/Addams Family, The (USA).nes");

    //:/games/Bomberman (USA).nes
    //:/games/Tom & Jerry - The Ultimate Game of Cat and Mouse! (USA).nes
    //:/games/Jackal (USA).nes
    //:/games/Batman - Return of the Joker (USA).nes
    //:/games/Captain America and the Avengers (USA).nes
    //:/games/Chessmaster, The (USA).nes
    //:/games/Galaxian (Japan).nes
    //:/games/Jackal (USA).nes
    //:/games/Jungle Book, The (USA).nes
    //:/games/Kekkyoku Nankyoku Daibouken (Japan).nes
    //:/games/Power Blade 2 (USA).nes
    //:/games/Tetris (USA) (Tengen) (Unl).nes
    //:/games/Side Pocket (USA).nes
    SaveLoad save_load(cpu, bus, ppu, apu, w);

    int exec = a.exec();

    return exec;
}
