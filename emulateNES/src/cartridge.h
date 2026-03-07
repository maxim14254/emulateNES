#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <QString>
#include <cstdint>
#include <memory>
#include "mapper.h"



class Cartridge
{
public:
    Cartridge(const QString& path, bool* status);
    ~Cartridge();

    uint8_t mapper_read_prg(uint16_t addr);
    uint8_t mapper_read_chr(uint16_t addr);

    uint8_t read_prg_ram(uint16_t addr);
    void write_prg_ram(uint16_t addr, uint8_t data);

    void write_chr_ram(uint16_t addr, uint8_t data);

    uint16_t map_nametable_addr(uint16_t addr);

    uint16_t get_NMI();
    uint16_t get_RESET();
    uint16_t get_IRQ();
    int get_orintation();


private:
    std::unique_ptr<Mapper> mapper;

};

#endif // CARTRIDGE_H
