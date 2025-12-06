#ifndef BUS_H
#define BUS_H

#include <vector>
#include <memory>


class Cartridge;
class QString;

class Bus
{
public:

    Bus();
    ~Bus();

    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void init_new_cartridge(const QString& path, bool* status);

    uint16_t get_NMI();
    uint16_t get_RESET();
    uint16_t get_IRQ();

private:
    std::vector<uint8_t> ram;               //ОЗУ
    std::shared_ptr<Cartridge> cartridge;   //Картридж
};

#endif // BUS_H
