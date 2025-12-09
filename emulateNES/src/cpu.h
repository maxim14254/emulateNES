#ifndef CPU_H
#define CPU_H


#include "QObject"
#include <vector>
#include <functional>
#include <mutex>
#include <QString>
#include <memory>
#include <thread>


//0x00  BRK   impl++
//0x01  ORA   (ind,X)++
//0x05  ORA   zp++
//0x06  ASL   zp++
//0x08  PHP   impl++
//0x09  ORA   #imm++
//0x0A  ASL   A++
//0x0D  ORA   abs++
//0x0E  ASL   abs++

//0x10  BPL   rel++
//0x11  ORA   (ind),Y++
//0x15  ORA   zp,X++
//0x16  ASL   zp,X++
//0x18  CLC   impl++
//0x19  ORA   abs,Y++
//0x1D  ORA   abs,X++
//0x1E  ASL   abs,X++

//0x20  JSR   abs++
//0x21  AND   (ind,X)++
//0x24  BIT   zp++
//0x25  AND   zp++
//0x26  ROL   zp++
//0x28  PLP   impl++
//0x29  AND   #imm++
//0x2A  ROL   A++
//0x2C  BIT   abs++
//0x2D  AND   abs++
//0x2E  ROL   abs++

//0x30  BMI   rel++
//0x31  AND   (ind),Y++
//0x35  AND   zp,X++
//0x36  ROL   zp,X++
//0x38  SEC   impl++
//0x39  AND   abs,Y++
//0x3D  AND   abs,X++
//0x3E  ROL   abs,X++

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
//0xEB  SBC   #imm+
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





//0x03  SLO   ind,x++
//0x07  SLO   zp++
//0x0F  SLO   abs++
//0x13  SLO   ind,y++
//0x17  SLO   zp,x++
//0x1B  SLO   abs,y++
//0x1F  SLO   abs,x++

//0x23  RLA   ind,x++
//0x27  RLA   zp++
//0x2F  RLA   abs++
//0x33  RLA   ind,y++
//0x37  RLA   zp,x++
//0x3B  RLA   abs,y++
//0x3F  RLA   abs,x++

//0x43  SRE   ind,x+
//0x47  SRE   zp+
//0x4F  SRE   abs+
//0x53  SRE   ind,y+
//0x57  SRE   zp,x+
//0x5B  SRE   abs,y+
//0x5F  SRE   abs,x+

//0x63  RRA   ind,x+
//0x67  RRA   zp+
//0x6F  RRA   abs+
//0x73  RRA   ind,y+
//0x77  RRA   zp,x+
//0x7B  RRA   abs,y+
//0x7F  RRA   abs,x+

//0xC3  DCP   ind,x+
//0xC7  DCP   zp+
//0xCF  DCP   abs+
//0xD3  DCP   ind,y+
//0xD7  DCP   zp,x+
//0xDB  DCP   abs,y+
//0xDF  DCP   abs,x+

//0xE3  ISC   ind,x+
//0xE7  ISC   zp+
//0xEF  ISC   abs+
//0xF3  ISC   ind,y+
//0xF7  ISC   zp,x+
//0xFB  ISC   abs,y+
//0xFF  ISC   abs,x+

//0xA3  LAX   ind,x+
//0xA7  LAX   zp+
//0xAB  LAX   imm
//0xAF  LAX   abs+
//0xB3  LAX   ind,y+
//0xB7  LAX   zp,y+
//0xBF  LAX   abs,y+

//0x83  SAX   ind,x+
//0x87  SAX   zp+
//0x8F  SAX   abs+
//0x97  SAX   zp,y+
//0xCB  SAX   imm+


//0x1A  NOP   imp++
//0x3A  NOP   imp++
//0x5A  NOP   imp++
//0x7A  NOP   imp+
//0xDA  NOP   imp+
//0xFA  NOP   imp+

//0x04  NOP   zp++
//0x44  NOP   zp++
//0x64  NOP   zp++

//0x14  NOP   zp,x++
//0x34  NOP   zp,x++
//0x54  NOP   zp,x++
//0x74  NOP   zp,x+
//0xD4  NOP   zp,x+
//0xF4  NOP   zp,x+

//0x0C  NOP   abs++

//0x1C  NOP   abs,x++
//0x3C  NOP   abs,x+
//0x5C  NOP   abs,x++
//0x7C  NOP   abs,x+
//0xDC  NOP   abs,x+
//0xFC  NOP   abs,x+

//0x80  NOP   #imm+

//0x0B  ANC   #imm++
//0x2B  ANC   #imm+

//0x4B  ALR   #imm+
//0x6B  ARR   #imm++
//0x8B  XAA   #imm+

//0x93  AHX   ind,y+
//0x9F  AHX   abs,y+

//0x9C  SHY   abs,x+
//0x9E  SHX   abs,y+
//0x9B  TAS   abs,y+

//0xBB  LAS   abs,y+

//0x02  KIL   imp++
//0x12  KIL   imp++
//0x22  KIL   imp+
//0x32  KIL   imp++
//0x42  KIL   imp++
//0x52  KIL   imp+
//0x62  KIL   imp++
//0x72  KIL   imp+
//0x92  KIL   imp+
//0xB2  KIL   imp+
//0xD2  KIL   imp+
//0xF2  KIL   imp+

//0x82  NOP   #imm+
//0x89  BIT   #imm+
//0xC2  NOP   #imm+
//0xE2  NOP   #imm+



class Bus;
class MainWindow;

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

class CPU : public QObject
{
    Q_OBJECT

public:
    CPU(MainWindow* _window);
    ~CPU();

    void set_flag(StatusFlags f, bool value);
    bool get_flag(StatusFlags f);

    bool init_new_cartridge(const QString& path);

    std::shared_ptr<Bus> get_bus();

signals:
    void signal_error_show();

private:
    uint8_t  A, X, Y;           // регистры
    uint8_t  SP;                // стек
    uint16_t PC;                // счетчик команд
    uint8_t  status;            // флаги
    std::shared_ptr<Bus> bus;   // шина
    uint32_t  cycles;           // счетчик циклов

    uint16_t NMI;
    uint16_t RESET;
    uint16_t IRQ;

    std::mutex mutex_stop;
    std::once_flag start_once_flag;
    std::thread run_t;
    bool start;
    MainWindow* window;

    std::vector<std::function<void(CPU&)>> table_instructions;  // таблица инструкции

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
    uint16_t indirect(uint16_t* addr = nullptr);            // 4 цикла
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
    void BIT_imm();                 // 2 цикла

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
    void LDX_zpY();                 // 4 цикла
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

    void NOP_impl_DA();             // 2 цикла
    void NOP_impl_EA();             // 2 цикла
    void NOP_impl_FA();             // 2 цикла
    void NOP_zp();                  // 3 цикла
    void NOP_zpX();                 // 4 цикла
    void NOP_abs();                 // 4 цикла
    void NOP_absX();                // 4-5 цикла
    void NOP_imm();                 // 2 цикла

    void SLO_base(uint8_t val, uint16_t addr);    // 3 цикла
    void SLO_indX();                // 8 циклов
    void SLO_zp();                  // 5 циклов
    void SLO_abs();                 // 6 циклов
    void SLO_indY();                // 8 циклов
    void SLO_zpX();                 // 6 циклов
    void SLO_absY();                // 7 циклов
    void SLO_absX();                // 7 циклов

    void RLA_base(uint8_t val, uint16_t addr);    // 3 цикла
    void RLA_indX();                // 8 циклов
    void RLA_zp();                  // 5 циклов
    void RLA_abs();                 // 6 циклов
    void RLA_indY();                // 8 циклов
    void RLA_zpX();                 // 6 циклов
    void RLA_absY();                // 7 циклов
    void RLA_absX();                // 7 циклов

    void SRE_base(uint8_t val, uint16_t addr);    // 3 цикла
    void SRE_indX();                // 8 циклов
    void SRE_zp();                  // 5 циклов
    void SRE_abs();                 // 6 циклов
    void SRE_indY();                // 8 циклов
    void SRE_zpX();                 // 6 циклов
    void SRE_absY();                // 7 циклов
    void SRE_absX();                // 7 циклов

    void RRA_base(uint8_t val, uint16_t addr);   // 3 цикла
    void RRA_indX();                // 8 циклов
    void RRA_zp();                  // 5 циклов
    void RRA_abs();                 // 6 циклов
    void RRA_indY();                // 8 циклов
    void RRA_zpX();                 // 6 циклов
    void RRA_absY();                // 7 циклов
    void RRA_absX();                // 7 циклов

    void DCP_base(uint8_t val, uint16_t addr);   // 3 цикла
    void DCP_indX();                // 8 циклов
    void DCP_zp();                  // 5 циклов
    void DCP_abs();                 // 6 циклов
    void DCP_indY();                // 8 циклов
    void DCP_zpX();                 // 6 циклов
    void DCP_absY();                // 7 циклов
    void DCP_absX();                // 7 циклов

    void ISC_base(uint8_t val, uint16_t addr);   // 3 цикла
    void ISC_indX();                // 8 циклов
    void ISC_zp();                  // 5 циклов
    void ISC_abs();                 // 6 циклов
    void ISC_indY();                // 8 циклов
    void ISC_zpX();                 // 6 циклов
    void ISC_absY();                // 7 циклов
    void ISC_absX();                // 7 циклов

    void LAX_base(uint8_t val);     // 1 цикла
    void LAX_indX();                // 6 циклов
    void LAX_zp();                  // 3 циклов
    void LAX_abs();                 // 4 циклов
    void LAX_indY();                // 5 циклов
    void LAX_absY();                // 4-5 циклов
    void LAX_zpY();                 // 4 циклов
    void LAX_imm();                 // 2 цикла

    void SAX_base(uint16_t addr);   // 1 цикл
    void SAX_indX();                // 6 циклов
    void SAX_zp();                  // 3 цикла
    void SAX_abs();                 // 4 цикла
    void SAX_zpY();                 // 4 цикла
    void SAX_imm();                 // 2 цикла

    void ANC_imm();                 // 2 цикла

    void ALR_imm();                 // 2 цикла

    void ARR_imm();                 // 2 цикла

    void XAA_imm();                 // 2 цикла

    void AHX_base(uint16_t addr);   // 2 цикла
    void AHX_indY();                // 6 циклов
    void AHX_absY();                // 5 циклов

    void SHY_absX();                // 5 циклов

    void TAS_absY();                // 5 циклов

    void SHX_absY();                // 5 циклов

    void LAS_absY();                // 4-5 циклов

    void KIL_imp();

};

#endif // CPU_H
