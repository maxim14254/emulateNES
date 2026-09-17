#ifndef MAPPER_7_H
#define MAPPER_7_H

#include <QFile>
#include <vector>
#include "mapper.h"


class Mapper_7 : public Mapper
{
public:
    Mapper_7(QFile& file, NESHeader _header);
    virtual ~Mapper_7();

    uint8_t mapper_read_prg(uint16_t addr) override;
    uint8_t mapper_read_chr(uint16_t addr) override;

    uint8_t read_prg_ram(uint16_t addr) override;

    void write_prg_ram(uint16_t addr, uint8_t data) override;
    void write_chr_ram(uint16_t addr, uint8_t data) override;

    void mapper_write(uint16_t addr, uint8_t data) override;
    uint16_t map_nametable_addr(uint16_t addr) override;

    friend QDataStream &operator<<(QDataStream &stream, const Mapper_7 &mapper);
    friend QDataStream &operator>>(QDataStream &in, Mapper_7 &mapper);

private:
    uint8_t prg_bank = 0;
};

#endif // MAPPER_7_H
