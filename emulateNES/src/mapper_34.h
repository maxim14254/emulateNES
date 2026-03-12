#ifndef MAPPER_34_H
#define MAPPER_34_H

#include "mapper.h"
#include <QFile>


class Mapper_34 : public Mapper
{
public:
    Mapper_34(QFile& file, NESHeader _header);
    ~Mapper_34();

    uint8_t mapper_read_prg(uint16_t addr) override;
    uint8_t mapper_read_chr(uint16_t addr) override;

    uint8_t read_prg_ram(uint16_t addr) override;

    void write_prg_ram(uint16_t addr, uint8_t data) override;
    void write_chr_ram(uint16_t addr, uint8_t data) override;
    void mapper_write(uint16_t addr, uint8_t data) override;

    uint16_t map_nametable_addr(uint16_t addr) override;

    uint16_t get_NMI() override;
    uint16_t get_RESET() override;
    uint16_t get_IRQ() override;

private:
    uint8_t prg_bank = 0;
    uint32_t prg_bank_count_32k = 0;

    void init_mirroring();
};

#endif // MAPPER_34_H
