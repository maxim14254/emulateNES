#ifndef MAPPER_1_H
#define MAPPER_1_H

#include "mapper.h"
#include <QFile>


class Mapper_1 : public Mapper
{
public:
    Mapper_1(QFile& file, NESHeader _header);
    ~Mapper_1();

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
    uint8_t reg_control = 0x0C;
    uint8_t reg_chr0 = 0;
    uint8_t reg_chr1 = 0;
    uint8_t reg_prg = 0;

    uint8_t shift_reg = 0;
    uint8_t shift_cnt = 0;

    void up_orintation();
    uint32_t map_prg_addr(uint16_t addr);
    uint32_t map_chr_addr(uint16_t addr);
    uint16_t read_vector(uint16_t addr);
};

#endif // MAPPER_1_H
