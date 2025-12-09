#ifndef LOG_H
#define LOG_H


#include "QFile"


class LOG
{
public:
    LOG();

    static void Init();
    static void Write(uint16_t PC, const int16_t operands[3], const QString& instruction, uint8_t A,uint8_t X, uint8_t Y, uint8_t P, uint8_t SP, uint32_t cycles);
};

#endif // LOG_H
