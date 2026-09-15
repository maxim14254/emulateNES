#ifndef SAVE_H
#define SAVE_H

#include "cpu.h"
#include "bus.h"
#include "ppu.h"
#include "apu.h"


class SaveLoad
{
public:
    SaveLoad(CPU& cpu, Bus& bus, PPU& ppu, APU* apu);

    void Save();
    void Load();

private:
    CPU& cpu;
    Bus& bus;
    PPU& ppu;
    APU* apu;

    QString SaveDir;

    uint8_t saveNumb = 0;
};

#endif // SAVE_H
