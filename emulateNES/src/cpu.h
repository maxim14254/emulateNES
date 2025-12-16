#ifndef CPU_H
#define CPU_H


#include "QObject"
#include <vector>
#include <functional>
#include <mutex>
#include <QString>
#include <memory>
#include <thread>


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
    void STX_zpY();                 // 4 цикла
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
    void SBC_imm_E9();              // 2 цикла
    void SBC_imm_EB();              // 2 цикла
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

    void NOP_imm_1A();              // 2 цикла
    void NOP_imm_3A();              // 2 цикла
    void NOP_imm_5A();              // 2 цикла
    void NOP_imm_7A();              // 2 цикла
    void NOP_imm_80();              // 2 цикла
    void NOP_impl_DA();             // 2 цикла
    void NOP_impl_EA();             // 2 цикла
    void NOP_impl_FA();             // 2 цикла
    void NOP_zp_04();               // 3 цикла
    void NOP_zp_44();               // 3 цикла
    void NOP_zp_64();               // 3 цикла
    void NOP_zpX_14();              // 4 цикла
    void NOP_zpX_34();              // 4 цикла
    void NOP_zpX_54();              // 4 цикла
    void NOP_zpX_74();              // 4 цикла
    void NOP_zpX_D4();              // 4 цикла
    void NOP_zpX_F4();              // 4 цикла
    void NOP_abs();                 // 4 цикла
    void NOP_absX_1C();             // 4-5 цикла
    void NOP_absX_3C();             // 4-5 цикла
    void NOP_absX_5C();             // 4-5 цикла
    void NOP_absX_7C();             // 4-5 цикла
    void NOP_absX_DC();             // 4-5 цикла
    void NOP_absX_FC();             // 4-5 цикла

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
