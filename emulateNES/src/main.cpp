#include "mainwindow.h"
#include <QApplication>
#include "cpu.h"
#include <QSurfaceFormat>
#include <QMetaObject>
#include "bus.h"
#include "ppu.h"
#include "apu.h"
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

    if (dev.isFormatSupported(format))
    {
        QAudioOutput* sink = new QAudioOutput(dev, format);
        APU* apu = new APU(440.0, format.sampleRate(), &bus);

        apu->start();
        sink->start(apu);

        bus.init_APU(apu);
    }
    else
    {
        QMessageBox message(QMessageBox::Icon::Critical, "Error", "Аудио устройство не поддерживает аудио формат:\nSampleRate(48000)\nCodec(audio/pcm)\nSampleSize(32)\nSampleType(SignedInt)\nByteOrder(LittleEndian)",
                            QMessageBox::StandardButton::Ok);
        message.exec();
    }

    PPU ppu(&w, &bus);
    bus.init_PPU(&ppu);

#ifdef LOG_ON
    LOG::Init(&ppu);
#endif

    CPU cpu(&w, &bus);
    bus.init_CPU(&cpu);
    bool rez = cpu.slot_init_new_cartridge(":/games/Donkey Kong (Japan).nes");

    int exec = a.exec();

    return exec;
}
