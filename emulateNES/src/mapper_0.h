#ifndef MAPPER___H
#define MAPPER___H

#include "mapper.h"
#include <QFile>

class Mapper_0 : public Mapper
{
public:
    Mapper_0(QFile& file, NESHeader header);
    ~Mapper_0();

    uint8_t mapper_read_prg(uint16_t addr) override;
    uint8_t mapper_read_chr(uint16_t addr) override;

    uint8_t read_prg_ram(uint16_t addr) override;
    void write_prg_ram(uint16_t addr, uint8_t data) override;

    void write_chr_ram(uint16_t addr, uint8_t data) override;

    uint16_t map_nametable_addr(uint16_t addr) override;

    uint16_t get_NMI() override;
    uint16_t get_RESET() override;
    uint16_t get_IRQ() override;

};

#endif // MAPPER___H
