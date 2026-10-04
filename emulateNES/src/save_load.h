#ifndef SAVE_H
#define SAVE_H

#include "cpu.h"
#include "bus.h"
#include "ppu.h"
#include "apu.h"
#include "net_play.h"


class SaveLoad
{
public:
    SaveLoad(CPU& cpu, Bus& bus, PPU& ppu, APU* apu, NetPlay& _netPlay, MainWindow& w);

    void Save();
    void Load();
    void LoadSaveFromNet(QByteArray& array);
    void StartGameForNet();
    void LoadGameForNet(QByteArray& array);
    void StartFullSynchronization();
    void LoadFullSynchronization(QByteArray& array);

private:
    CPU& cpu;
    Bus& bus;
    PPU& ppu;
    APU* apu;
    NetPlay& netPlay;
    MainWindow& w;

    QString SaveDir;

    uint8_t saveNumb = 0;
};

#endif // SAVE_H
