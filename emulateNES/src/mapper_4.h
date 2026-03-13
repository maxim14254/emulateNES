#ifndef MAPPER_4_H
#define MAPPER_4_H

#include "mapper.h"
#include <QFile>

class Bus;

class Mapper_4 : public Mapper
{
public:
    Mapper_4(QFile& file, NESHeader _header, Bus* _bus);
    ~Mapper_4();

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
    void update_banks();
    void clock_irq_on_a12(uint16_t addr);

private:
    uint8_t bank_select = 0;
    uint8_t bank_registers[8] = {0};

    bool prg_mode = false;
    bool chr_mode = false;

    bool prg_ram_enable = true;
    bool prg_ram_write_protect = false;

    uint8_t prg_bank_map[4] = {0};
    uint16_t chr_bank_map[8] = {0};

    uint32_t prg_bank_count_8k = 0;
    uint32_t chr_bank_count_1k = 0;

    uint8_t irq_latch = 0;
    uint8_t irq_counter = 0;
    bool irq_reload = false;
    bool irq_enabled = false;
    bool irq_pending = false;

    // A12 edge detect
    bool last_a12 = false;

    int a12_low_cycles = 0;

    Bus* bus;
};

#endif // MAPPER_4_H
