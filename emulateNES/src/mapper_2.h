#ifndef MAPPER_2_H
#define MAPPER_2_H

#include "mapper.h"
#include <QFile>

class Mapper_2 : public Mapper
{
public:
    Mapper_2(QFile& file, NESHeader _header);
    ~Mapper_2();

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
    uint8_t reg_prg = 0;
    uint32_t total_prg_banks = 1;

    uint32_t map_prg_addr(uint16_t addr);

    uint8_t reg_chr = 0;
};



#endif // MAPPER_2_H
