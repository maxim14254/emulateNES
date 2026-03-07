#ifndef MAPPER_H
#define MAPPER_H

#include <vector>
#include <QString>

typedef struct
{
    uint8_t magic[4];          // "NESx1A"
    uint8_t prg_rom;           // байты PRG ROM
    uint8_t chr_rom;           // байты CHR ROM
    uint8_t flags6;
    uint8_t flags7;
    uint8_t prg_ram;           // 0 = 1*8КБ по умолчанию
    uint8_t tv_system1;        // устаревшее
    uint8_t tv_system2;        // устаревшее/доп. флаги
    uint8_t unused[5];         // 0 в iNES, расширено в NES 2.0
} NESHeader;

class Mapper
{
public:
    Mapper();
    virtual ~Mapper();

    virtual uint8_t mapper_read_prg(uint16_t addr) = 0;
    virtual uint8_t mapper_read_chr(uint16_t addr) = 0;

    virtual uint8_t read_prg_ram(uint16_t addr) = 0;
    virtual void write_prg_ram(uint16_t addr, uint8_t data) = 0;

    virtual void write_chr_ram(uint16_t addr, uint8_t data) = 0;

    virtual uint16_t map_nametable_addr(uint16_t addr) = 0;

    virtual uint16_t get_NMI();
    virtual uint16_t get_RESET();
    virtual uint16_t get_IRQ();

    enum MIRROR
    {
        HORIZONTAL,
        VERTICAL,
        ONESCREEN_LO,
        ONESCREEN_HI,
    } Orintation = HORIZONTAL;

protected:
    std::vector<uint8_t> chr_ram;   // ОЗУ картриджа
    std::vector<uint8_t> prg_ram;   // ОЗУ картриджа

    std::vector<uint8_t> prg_rom;   // код
    std::vector<uint8_t> chr_rom;   // (графика), мапперы

    NESHeader header;
};

#endif // MAPPER_H
