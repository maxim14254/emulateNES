#ifndef MAPPER_7_H
#define MAPPER_7_H

#include <QFile>
#include <vector>
#include "mapper.h"

class Bus;

class Mapper_7 : public Mapper
{
public:
    Mapper_7(QFile& file, NESHeader _header, Bus* _bus);
    ~Mapper_7();

    uint8_t mapper_read_prg(uint16_t addr);
    uint8_t mapper_read_chr(uint16_t addr);

    uint8_t read_prg_ram(uint16_t addr);

    void write_prg_ram(uint16_t addr, uint8_t data);
    void write_chr_ram(uint16_t addr, uint8_t data);

    void mapper_write(uint16_t addr, uint8_t data);
    uint16_t map_nametable_addr(uint16_t addr);

    uint16_t get_NMI();
    uint16_t get_RESET();
    uint16_t get_IRQ();

private:
    void update_banks();

    NESHeader header;
    Bus* bus;

    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    std::vector<uint8_t> chr_ram;
    std::vector<uint8_t> prg_ram;

    uint8_t prg_bank_count_8k;
    uint8_t prg_bank_count_32k;
    uint8_t prg_bank_32k;
    uint8_t prg_bank_map[4];
};

#endif // MAPPER_7_H
