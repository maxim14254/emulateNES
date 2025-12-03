#ifndef CPU_H
#define CPU_H


#include <vector>
#include <functional>
#include <mutex>
#include <QString>
#include <QSharedPointer>
#include <thread>


//0x00  BRK   impl+
//0x01  ORA   (ind,X)+
//0x05  ORA   zp+
//0x06  ASL   zp+
//0x08  PHP   impl+
//0x09  ORA   #imm+
//0x0A  ASL   A+
//0x0D  ORA   abs+
//0x0E  ASL   abs+

//0x10  BPL   rel+
//0x11  ORA   (ind),Y+
//0x15  ORA   zp,X+
//0x16  ASL   zp,X+
//0x18  CLC   impl+
//0x19  ORA   abs,Y+
//0x1D  ORA   abs,X+
//0x1E  ASL   abs,X+

//0x20  JSR   abs+
//0x21  AND   (ind,X)+
//0x24  BIT   zp+
//0x25  AND   zp+
//0x26  ROL   zp+
//0x28  PLP   impl+
//0x29  AND   #imm+
//0x2A  ROL   A+
//0x2C  BIT   abs+
//0x2D  AND   abs+
//0x2E  ROL   abs+

//0x30  BMI   rel+
//0x31  AND   (ind),Y+
//0x35  AND   zp,X+
//0x36  ROL   zp,X+
//0x38  SEC   impl+
//0x39  AND   abs,Y+
//0x3D  AND   abs,X+
//0x3E  ROL   abs,X+

//0x40  RTI   impl+
//0x41  EOR   (ind,X)+
//0x45  EOR   zp+
//0x46  LSR   zp+
//0x48  PHA   impl+
//0x49  EOR   #imm+
//0x4A  LSR   A+
//0x4C  JMP   abs+
//0x4D  EOR   abs+
//0x4E  LSR   abs+

//0x50  BVC   rel+
//0x51  EOR   (ind),Y+
//0x55  EOR   zp,X+
//0x56  LSR   zp,X+
//0x58  CLI   impl+
//0x59  EOR   abs,Y+
//0x5D  EOR   abs,X+
//0x5E  LSR   abs,X+

//0x60  RTS   impl+
//0x61  ADC   (ind,X)+
//0x65  ADC   zp+
//0x66  ROR   zp+
//0x68  PLA   impl+
//0x69  ADC   #imm+
//0x6A  ROR   A+
//0x6C  JMP   (ind)+
//0x6D  ADC   abs+
//0x6E  ROR   abs+

//0x70  BVS   rel+
//0x71  ADC   (ind),Y+
//0x75  ADC   zp,X+
//0x76  ROR   zp,X+
//0x78  SEI   impl+
//0x79  ADC   abs,Y+
//0x7D  ADC   abs,X+
//0x7E  ROR   abs,X+

//0x81  STA   (ind,X)+
//0x84  STY   zp+
//0x85  STA   zp+
//0x86  STX   zp+
//0x88  DEY   impl+
//0x8A  TXA   impl+
//0x8C  STY   abs+
//0x8D  STA   abs+
//0x8E  STX   abs+

//0x90  BCC   rel+
//0x91  STA   (ind),Y+
//0x94  STY   zp,X+
//0x95  STA   zp,X+
//0x96  STX   zp,Y+
//0x98  TYA   impl+
//0x99  STA   abs,Y+
//0x9A  TXS   impl+
//0x9D  STA   abs,X+

//0xA0  LDY   #imm+
//0xA1  LDA   (ind,X)+
//0xA2  LDX   #imm+
//0xA4  LDY   zp+
//0xA5  LDA   zp+
//0xA6  LDX   zp+
//0xA8  TAY   impl+
//0xA9  LDA   #imm+
//0xAA  TAX   impl+
//0xAC  LDY   abs+
//0xAD  LDA   abs+
//0xAE  LDX   abs+

//0xB0  BCS   rel+
//0xB1  LDA   (ind),Y+
//0xB4  LDY   zp,X+
//0xB5  LDA   zp,X+
//0xB6  LDX   zp,Y+
//0xB8  CLV   impl+
//0xB9  LDA   abs,Y+
//0xBA  TSX   impl+
//0xBC  LDY   abs,X+
//0xBD  LDA   abs,X+
//0xBE  LDX   abs,Y+

//0xC0  CPY   #imm+
//0xC1  CMP   (ind,X)+
//0xC4  CPY   zp+
//0xC5  CMP   zp+
//0xC6  DEC   zp+
//0xC8  INY   impl+
//0xC9  CMP   #imm+
//0xCA  DEX   impl+
//0xCC  CPY   abs+
//0xCD  CMP   abs+
//0xCE  DEC   abs+

//0xD0  BNE   rel+
//0xD1  CMP   (ind),Y+
//0xD5  CMP   zp,X+
//0xD6  DEC   zp,X+
//0xD8  CLD   impl+
//0xD9  CMP   abs,Y+
//0xDD  CMP   abs,X+
//0xDE  DEC   abs,X+

//0xE0  CPX   #imm+
//0xE1  SBC   (ind,X)+
//0xE4  CPX   zp+
//0xE5  SBC   zp+
//0xE6  INC   zp+
//0xE8  INX   impl+
//0xE9  SBC   #imm+
//0xEA  NOP   impl+
//0xEC  CPX   abs+
//0xED  SBC   abs+
//0xEE  INC   abs+

//0xF0  BEQ   rel+
//0xF1  SBC   (ind),Y+
//0xF5  SBC   zp,X+
//0xF6  INC   zp,X+
//0xF8  SED   impl+
//0xF9  SBC   abs,Y+
//0xFD  SBC   abs,X+
//0xFE  INC   abs,X+




class Bus;

enum StatusFlags
{
    C = 0x1,
    Z = 0x2,
    I = 0x4,
    D = 0x8,
    B = 0x10,
    U = 0x20,
    V = 0x40,
    N = 0x80
};

class CPU
{
public:
    CPU();
    ~CPU();

    void set_flag(StatusFlags f, bool value);
    bool get_flag(StatusFlags f);

    bool init_new_cartridge(const QString& path);

    QSharedPointer<Bus> get_bus();

private:
    uint8_t  A, X, Y;           // регистры
    uint8_t  SP;                // стек
    uint16_t PC;                // счетчик команд
    uint8_t  status;            // флаги
    QSharedPointer<Bus> bus;    // шина
    uint8_t  cycles;            // счетчик циклов

    uint16_t NMI;
    uint16_t RESET;
    uint16_t IRQ;

    std::mutex mutex_stop;
    std::once_flag start_once_flag;
    std::thread run_t;
    bool start;

    std::vector<std::function<void()>> table_instructions;  // таблица инструкции

    void run();
    void reset();


    //адресации
    uint8_t immediate(uint16_t* addr = nullptr);            // 1 цикл
    uint8_t zero_page(uint16_t* addr = nullptr);            // 2 цикла
    uint8_t zero_pageX(uint16_t* addr = nullptr);           // 3 цикла
    uint8_t zero_pageY(uint16_t* addr = nullptr);           // 3 цикла
    uint8_t accumulator();                                  // 0 циклов
    uint8_t absolute(uint16_t* addr = nullptr);             // 3 цикла
    uint8_t absoluteX(uint16_t* addr = nullptr);            // 3-4 цикла
    uint8_t absoluteY(uint16_t* addr = nullptr);            // 3-4 цикла
    uint16_t indirect();                                    // 4 цикла
    uint8_t indexed_inderectX(uint16_t* addr = nullptr);    // 5 циклов
    uint8_t indexed_inderectY(uint16_t* addr = nullptr);    // 4-5 циклов
    int8_t relative(uint16_t* addr = nullptr);              // 1 цикл


    //инструкции
    void BRK_impl();                // 7 циклов

    void ORA_base(uint8_t val);     // 1 цикл
    void ORA_zpX();                 // 4 цикла
    void ORA_zp();                  // 3 цикла
    void ORA_imm();                 // 2 цикла
    void ORA_abs();                 // 4 цикла
    void ORA_absX();                // 4-5 циклов
    void ORA_absY();                // 4-5 циклов
    void ORA_indX();                // 6 циклов
    void ORA_indY();                // 5-6 циклов

    uint8_t ASL_base(uint8_t val);  // 2 цикла
    void ASL_zp();                  // 5 циклов
    void ASL_zpX();                 // 6 циклов
    void ASL_A();                   // 2 цикла
    void ASL_abs();                 // 6 циклов
    void ASL_absX();                // 7 циклов

    void PHP_impl();                // 3 цикла

    void BPL_rel();                 // 2-4 цикла

    void CLC_impl();                // 2 цикла

    void JSR_abs();                 // 6 циклов

    void AND_base(uint8_t val);     // 1 цикл
    void AND_indX();                // 6 циклов
    void AND_indY();                // 5-6 циклов
    void AND_zp();                  // 3 цикла
    void AND_zpX();                 // 4 цикла
    void AND_imm();                 // 2 цикла
    void AND_abs();                 // 4 цикла
    void AND_absX();                // 4-5 циклов
    void AND_absY();                // 4-5 циклов

    void BIT_base(uint8_t val);     // 1 цикл
    void BIT_zp();                  // 3 цикла
    void BIT_abs();                 // 4 цикла

    uint8_t ROL_base(uint8_t val);  // 2 цикла
    void ROL_zp();                  // 5 циклов
    void ROL_zpX();                 // 6 циклов
    void ROL_A();                   // 2 цикла
    void ROL_abs();                 // 6 циклов
    void ROL_absX();                // 7 циклов

    void PLP_impl();                // 4 цикла

    void BMI_rel();                 // 2-4 цикла

    void SEC_impl();                // 2 цикла

    void RTI_impl();                // 6 циклов

    void EOR_base(uint8_t val);     // 1 цикл
    void EOR_indX();                // 6 циклов
    void EOR_indY();                // 5-6 циклов
    void EOR_zp();                  // 3 цикла
    void EOR_zpX();                 // 4 цикла
    void EOR_imm();                 // 2 цикла
    void EOR_abs();                 // 4 цикла
    void EOR_absX();                // 4-5 циклов
    void EOR_absY();                // 4-5 циклов

    uint8_t LSR_base(uint8_t val);  // 2 цикла
    void LSR_zp();                  // 5 циклов
    void LSR_zpX();                 // 6 циклов
    void LSR_A();                   // 2 цикла
    void LSR_abs();                 // 6 циклов
    void LSR_absX();                // 7 циклов

    void PHA_impl();                // 3 цикла

    void JMP_abs();                 // 3 циклов
    void JMP_ind();                 // 5 циклов

    void BVC_rel();                 // 2-4 цикла

    void CLI_impl();                // 2 цикла

    void RTS_impl();                // 6 циклов

    void ADC_base(uint8_t val);     // 1 цикла
    void ADC_indX();                // 6 циклов
    void ADC_indY();                // 5-6 циклов
    void ADC_zp();                  // 3 цикла
    void ADC_zpX();                 // 4 цикла
    void ADC_imm();                 // 2 цикла
    void ADC_abs();                 // 4 цикла
    void ADC_absX();                // 4-5 циклов
    void ADC_absY();                // 4-5 циклов

    uint8_t ROR_base(uint8_t val);  // 1 цикл
    void ROR_zp();                  // 5 циклов
    void ROR_zpX();                 // 6 циклов
    void ROR_A();                   // 2 цикла
    void ROR_abs();                 // 6 циклов
    void ROR_absX();                // 7 циклов

    void PLA_impl();                // 4 цикла

    void BVS_rel();                 // 2-4 цикла

    void SEI_impl();                // 2 цикла

    void STA_indX();                // 6 циклов
    void STA_indY();                // 5-6 циклов
    void STA_zp();                  // 3 цикла
    void STA_zpX();                 // 4 цикла
    void STA_imm();                 // 2 цикла
    void STA_abs();                 // 4 цикла
    void STA_absX();                // 4-5 циклов
    void STA_absY();                // 4-5 циклов

    void STY_zp();                  // 3 цикла
    void STY_zpX();                 // 4 цикла
    void STY_abs();                 // 4 цикла

    void STX_zp();                  // 3 цикла
    void STX_zpX();                 // 4 цикла
    void STX_abs();                 // 4 цикла

    void DEY_impl();                // 2 цикла

    void TXA_impl();                // 2 цикла

    void BCC_rel();                 // 2-4 цикла

    void TYA_impl();                // 2 цикла

    void TXS_impl();                // 2 цикла

    void LDY_base(uint8_t val);     // 1 цикл
    void LDY_zp();                  // 3 цикла
    void LDY_zpX();                 // 4 цикла
    void LDY_imm();                 // 2 цикла
    void LDY_abs();                 // 4 цикла
    void LDY_absX();                // 4-5 циклов

    void LDA_base(uint8_t val);     // 1 цикл
    void LDA_indX();                // 6 циклов
    void LDA_indY();                // 5-6 циклов
    void LDA_zp();                  // 3 цикла
    void LDA_zpX();                 // 4 цикла
    void LDA_imm();                 // 2 цикла
    void LDA_abs();                 // 4 цикла
    void LDA_absX();                // 4-5 циклов
    void LDA_absY();                // 4-5 циклов

    void LDX_base(uint8_t val);     // 1 цикл
    void LDX_zp();                  // 3 цикла
    void LDX_zpX();                 // 4 цикла
    void LDX_imm();                 // 2 цикла
    void LDX_abs();                 // 4 цикла
    void LDX_absY();                // 4-5 циклов

    void TAY_impl();                // 2 цикла

    void BCS_rel();                 // 2-4 цикла

    void CLV_impl();                // 2 цикла

    void TSX_impl();                // 2 цикла

    void CPY_base(uint8_t val);     // 1 цикл
    void CPY_imm();                 // 2 цикла
    void CPY_zp();                  // 3 цикла
    void CPY_abs();                 // 4 цикла

    void CMP_base(uint8_t val);     // 1 цикл
    void CMP_indX();                // 6 циклов
    void CMP_indY();                // 5-6 циклов
    void CMP_zp();                  // 3 цикла
    void CMP_zpX();                 // 4 цикла
    void CMP_imm();                 // 2 цикла
    void CMP_abs();                 // 4 цикла
    void CMP_absX();                // 4-5 циклов
    void CMP_absY();                // 4-5 циклов

    uint8_t DEC_base(uint8_t val);  // 1 цикл
    void DEC_zp();                  // 5 циклов
    void DEC_zpX();                 // 6 циклов
    void DEC_abs();                 // 6 циклов
    void DEC_absX();                // 7 циклов

    void INY_impl();                // 2 цикла

    void DEX_impl();                // 2 цикла

    void BNE_rel();                 // 2-4 цикла

    void CLD_impl();                // 2 цикла

    void CPX_base(uint8_t val);     // 1 цикл
    void CPX_imm();                 // 2 цикла
    void CPX_zp();                  // 3 цикла
    void CPX_abs();                 // 4 цикла

    void SBC_base(uint8_t val);     // 1 цикл
    void SBC_indX();                // 6 циклов
    void SBC_indY();                // 5-6 циклов
    void SBC_zp();                  // 3 цикла
    void SBC_zpX();                 // 4 цикла
    void SBC_imm();                 // 2 цикла
    void SBC_abs();                 // 4 цикла
    void SBC_absX();                // 4-5 циклов
    void SBC_absY();                // 4-5 циклов

    uint8_t INC_base(uint8_t val);  // 1 цикл
    void INC_zp();                  // 5 циклов
    void INC_zpX();                 // 6 циклов
    void INC_abs();                 // 6 циклов
    void INC_absX();                // 7 циклов

    void INX_impl();                // 2 цикла

    void BEQ_rel();                 // 2-4 цикла

    void SED_impl();                // 2 цикла

    void TAX_impl();                // 2 цикла

    void NOP_impl();                // 2 цикла
};

#endif // CPU_H
