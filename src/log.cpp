#include "log.h"
#include <QString>
#include <QDir>
#include "QFile"


QFile log_file;


void LOG::Init()
{
    QString path = QString("%1/log.txt").arg(QDir::currentPath());
    log_file.setFileName(path);

    if (log_file.open(QIODevice::WriteOnly))
        log_file.write("");

    log_file.close();
}

void LOG::Write(uint16_t PC, const int16_t operands[3], const QString &instruction, uint8_t A, uint8_t X, uint8_t Y, uint8_t P, uint8_t SP, uint32_t cycles)
{
    if (log_file.open(QIODevice::Append | QIODevice::Text))
    {
        std::string rez(100, ' ');

        int n = sprintf(&rez[0], "%04X", PC);
        rez[n] = ' ';

        if(operands[0] >= 0)
        {
            n = sprintf(&rez[6], "%02X", operands[0]);
            rez[n + 6] = ' ';
        }
        if(operands[1] >= 0)
        {
            n = sprintf(&rez[9], "%02X", operands[1]);
            rez[n + 9] = ' ';
        }
        if(operands[2] >= 0)
        {
            n = sprintf(&rez[12], "%02X", operands[2]);
            rez[n + 12] = ' ';
        }

        n = sprintf(&rez[16], "%s", instruction.toStdString().c_str());
        rez[n + 16] = ' ';
        n = sprintf(&rez[48], "A:%02X", A);
        rez[n + 48] = ' ';
        n = sprintf(&rez[53], "X:%02X", X);
        rez[n + 53] = ' ';
        n = sprintf(&rez[58], "Y:%02X", Y);
        rez[n + 58] = ' ';
        n = sprintf(&rez[63], "P:%02X", P);
        rez[n + 63] = ' ';
        n = sprintf(&rez[68], "SP:%02X", SP);
        rez[n + 68] = ' ';
        n = sprintf(&rez[74], "CYC:%d", cycles);
        rez[n + 74] = '\n';
        rez[n + 75] = '\0';

        log_file.write(rez.data());
    }

    log_file.close();
}
