#include "cpu.h"
#include "bus.h"
#include <QMessageBox>
#include "log.h"
#include <QMetaObject>
#include "mainwindow.h"



CPU::CPU(MainWindow* _window) : window(_window)
{
    bus.reset(new Bus());

    connect(this, &CPU::signal_error_show, window, &MainWindow::slot_show_error_message, Qt::QueuedConnection);

    table_instructions.resize(256);


    table_instructions[0x00] = &CPU::BRK_impl;
    table_instructions[0x01] = &CPU::ORA_indX;
    table_instructions[0x02] = &CPU::KIL_imp;
    table_instructions[0x03] = &CPU::SLO_indX;
    table_instructions[0x04] = &CPU::NOP_zp;
    table_instructions[0x05] = &CPU::ORA_zp;
    table_instructions[0x06] = &CPU::ASL_zp;
    table_instructions[0x07] = &CPU::SLO_zp;
    table_instructions[0x08] = &CPU::PHP_impl;
    table_instructions[0x09] = &CPU::ORA_imm;
    table_instructions[0x0A] = &CPU::ASL_A;
    table_instructions[0x0B] = &CPU::ANC_imm;
    table_instructions[0x0C] = &CPU::NOP_abs;
    table_instructions[0x0D] = &CPU::ORA_abs;
    table_instructions[0x0E] = &CPU::ASL_abs;
    table_instructions[0x0F] = &CPU::SLO_abs;

    table_instructions[0x10] = &CPU::BPL_rel;
    table_instructions[0x11] = &CPU::ORA_indY;
    table_instructions[0x12] = &CPU::KIL_imp;
    table_instructions[0x13] = &CPU::SLO_indY;
    table_instructions[0x14] = &CPU::NOP_zpX;
    table_instructions[0x15] = &CPU::ORA_zpX;
    table_instructions[0x16] = &CPU::ASL_zpX;
    table_instructions[0x17] = &CPU::SLO_zpX;
    table_instructions[0x18] = &CPU::CLC_impl;
    table_instructions[0x19] = &CPU::ORA_absY;
    table_instructions[0x1A] = &CPU::NOP_imm;
    table_instructions[0x1B] = &CPU::SLO_absY;
    table_instructions[0x1C] = &CPU::NOP_absX;
    table_instructions[0x1D] = &CPU::ORA_absX;
    table_instructions[0x1E] = &CPU::ASL_absX;
    table_instructions[0x1F] = &CPU::SLO_absX;

    table_instructions[0x20] = &CPU::JSR_abs;
    table_instructions[0x21] = &CPU::AND_indX;
    table_instructions[0x22] = &CPU::KIL_imp;
    table_instructions[0x23] = &CPU::RLA_indX;
    table_instructions[0x24] = &CPU::BIT_zp;
    table_instructions[0x25] = &CPU::AND_zp;
    table_instructions[0x26] = &CPU::ROL_zp;
    table_instructions[0x27] = &CPU::RLA_zp;
    table_instructions[0x28] = &CPU::PLP_impl;
    table_instructions[0x29] = &CPU::AND_imm;
    table_instructions[0x2A] = &CPU::ROL_A;
    table_instructions[0x2B] = &CPU::ANC_imm;
    table_instructions[0x2C] = &CPU::BIT_abs;
    table_instructions[0x2D] = &CPU::AND_abs;
    table_instructions[0x2E] = &CPU::ROL_abs;
    table_instructions[0x2F] = &CPU::RLA_abs;

    table_instructions[0x30] = &CPU::BMI_rel;
    table_instructions[0x31] = &CPU::AND_indY;
    table_instructions[0x32] = &CPU::KIL_imp;
    table_instructions[0x33] = &CPU::RLA_indY;
    table_instructions[0x34] = &CPU::NOP_zpX;
    table_instructions[0x35] = &CPU::AND_zpX;
    table_instructions[0x36] = &CPU::ROL_zpX;
    table_instructions[0x37] = &CPU::RLA_zpX;
    table_instructions[0x38] = &CPU::SEC_impl;
    table_instructions[0x39] = &CPU::AND_absY;
    table_instructions[0x3A] = &CPU::NOP_imm;
    table_instructions[0x3B] = &CPU::RLA_absY;
    table_instructions[0x3C] = &CPU::NOP_absX;
    table_instructions[0x3D] = &CPU::AND_absX;
    table_instructions[0x3E] = &CPU::ROL_absX;
    table_instructions[0x3F] = &CPU::RLA_absX;

    table_instructions[0x40] = &CPU::RTI_impl;
    table_instructions[0x41] = &CPU::EOR_indX;
    table_instructions[0x42] = &CPU::KIL_imp;
    table_instructions[0x43] = &CPU::SRE_indX;
    table_instructions[0x44] = &CPU::NOP_zp;
    table_instructions[0x45] = &CPU::EOR_zp;
    table_instructions[0x46] = &CPU::LSR_zp;
    table_instructions[0x47] = &CPU::SRE_zp;
    table_instructions[0x48] = &CPU::PHA_impl;
    table_instructions[0x49] = &CPU::EOR_imm;
    table_instructions[0x4A] = &CPU::LSR_A;
    table_instructions[0x4B] = &CPU::ALR_imm;
    table_instructions[0x4C] = &CPU::JMP_abs;
    table_instructions[0x4D] = &CPU::EOR_abs;
    table_instructions[0x4E] = &CPU::LSR_abs;
    table_instructions[0x4F] = &CPU::SRE_abs;

    table_instructions[0x50] = &CPU::BVC_rel;
    table_instructions[0x51] = &CPU::EOR_indY;
    table_instructions[0x52] = &CPU::KIL_imp;
    table_instructions[0x53] = &CPU::SRE_indY;
    table_instructions[0x54] = &CPU::NOP_zpX;
    table_instructions[0x55] = &CPU::EOR_zpX;
    table_instructions[0x56] = &CPU::LSR_zpX;
    table_instructions[0x57] = &CPU::SRE_zpX;
    table_instructions[0x58] = &CPU::CLI_impl;
    table_instructions[0x59] = &CPU::EOR_absY;
    table_instructions[0x5A] = &CPU::NOP_imm;
    table_instructions[0x5B] = &CPU::SRE_absY;
    table_instructions[0x5C] = &CPU::NOP_absX;
    table_instructions[0x5D] = &CPU::EOR_absX;
    table_instructions[0x5E] = &CPU::LSR_absX;
    table_instructions[0x5F] = &CPU::SRE_absX;

    table_instructions[0x60] = &CPU::RTS_impl;
    table_instructions[0x61] = &CPU::ADC_indX;
    table_instructions[0x62] = &CPU::KIL_imp;
    table_instructions[0x63] = &CPU::RRA_indX;
    table_instructions[0x64] = &CPU::NOP_zp;
    table_instructions[0x65] = &CPU::ADC_zp;
    table_instructions[0x66] = &CPU::ROR_zp;
    table_instructions[0x67] = &CPU::RRA_zp;
    table_instructions[0x68] = &CPU::PLA_impl;
    table_instructions[0x69] = &CPU::ADC_imm;
    table_instructions[0x6A] = &CPU::ROR_A;
    table_instructions[0x6B] = &CPU::ARR_imm;
    table_instructions[0x6C] = &CPU::JMP_ind;
    table_instructions[0x6D] = &CPU::ADC_abs;
    table_instructions[0x6E] = &CPU::ROR_abs;
    table_instructions[0x6F] = &CPU::RRA_abs;

    table_instructions[0x70] = &CPU::BVS_rel;
    table_instructions[0x71] = &CPU::ADC_indY;
    table_instructions[0x72] = &CPU::KIL_imp;
    table_instructions[0x73] = &CPU::RRA_indY;
    table_instructions[0x74] = &CPU::NOP_zpX;
    table_instructions[0x75] = &CPU::ADC_zpX;
    table_instructions[0x76] = &CPU::ROR_zpX;
    table_instructions[0x77] = &CPU::RRA_zpX;
    table_instructions[0x78] = &CPU::SEI_impl;
    table_instructions[0x79] = &CPU::ADC_absY;
    table_instructions[0x7A] = &CPU::NOP_imm;
    table_instructions[0x7B] = &CPU::RRA_absY;
    table_instructions[0x7C] = &CPU::NOP_absX;
    table_instructions[0x7D] = &CPU::ADC_absX;
    table_instructions[0x7E] = &CPU::ROR_absX;
    table_instructions[0x7F] = &CPU::RRA_absX;

    table_instructions[0x80] = &CPU::NOP_imm;
    table_instructions[0x81] = &CPU::STA_indX;
    table_instructions[0x82] = &CPU::NOP_imm;
    table_instructions[0x83] = &CPU::SAX_indX;
    table_instructions[0x84] = &CPU::STY_zp;
    table_instructions[0x85] = &CPU::STA_zp;
    table_instructions[0x86] = &CPU::STX_zp;
    table_instructions[0x87] = &CPU::SAX_zp;
    table_instructions[0x88] = &CPU::DEY_impl;
    table_instructions[0x89] = &CPU::BIT_imm;
    table_instructions[0x8A] = &CPU::TXA_impl;
    table_instructions[0x8B] = &CPU::XAA_imm;
    table_instructions[0x8C] = &CPU::STY_abs;
    table_instructions[0x8D] = &CPU::STA_abs;
    table_instructions[0x8E] = &CPU::STX_abs;
    table_instructions[0x8F] = &CPU::SAX_abs;

    table_instructions[0x90] = &CPU::BCC_rel;
    table_instructions[0x91] = &CPU::STA_indY;
    table_instructions[0x92] = &CPU::KIL_imp;
    table_instructions[0x93] = &CPU::AHX_indY;
    table_instructions[0x94] = &CPU::STY_zpX;
    table_instructions[0x95] = &CPU::STA_zpX;
    table_instructions[0x96] = &CPU::STX_zpX;
    table_instructions[0x97] = &CPU::SAX_zpY;
    table_instructions[0x98] = &CPU::TYA_impl;
    table_instructions[0x99] = &CPU::STA_absY;
    table_instructions[0x9A] = &CPU::TXS_impl;
    table_instructions[0x9B] = &CPU::TAS_absY;
    table_instructions[0x9C] = &CPU::SHY_absX;
    table_instructions[0x9D] = &CPU::STA_absX;
    table_instructions[0x9E] = &CPU::SHX_absY;
    table_instructions[0x9F] = &CPU::AHX_absY;

    table_instructions[0xA0] = &CPU::LDY_imm;
    table_instructions[0xA1] = &CPU::LDA_indX;
    table_instructions[0xA2] = &CPU::LDX_imm;
    table_instructions[0xA3] = &CPU::LAX_indX;
    table_instructions[0xA4] = &CPU::LDY_zp;
    table_instructions[0xA5] = &CPU::LDA_zp;
    table_instructions[0xA6] = &CPU::LDX_zp;
    table_instructions[0xA7] = &CPU::LAX_zp;
    table_instructions[0xA8] = &CPU::TAY_impl;
    table_instructions[0xA9] = &CPU::LDA_imm;
    table_instructions[0xAA] = &CPU::TAX_impl;
    table_instructions[0xAB] = &CPU::LAX_imm;
    table_instructions[0xAC] = &CPU::LDY_abs;
    table_instructions[0xAD] = &CPU::LDA_abs;
    table_instructions[0xAE] = &CPU::LDX_abs;
    table_instructions[0xAF] = &CPU::LAX_abs;

    table_instructions[0xB0] = &CPU::BCS_rel;
    table_instructions[0xB1] = &CPU::LDA_indY;
    table_instructions[0xB2] = &CPU::KIL_imp;
    table_instructions[0xB3] = &CPU::LAX_indY;
    table_instructions[0xB4] = &CPU::LDY_zpX;
    table_instructions[0xB5] = &CPU::LDA_zpX;
    table_instructions[0xB6] = &CPU::LDX_zpY;
    table_instructions[0xB7] = &CPU::LAX_zpY;
    table_instructions[0xB8] = &CPU::CLV_impl;
    table_instructions[0xB9] = &CPU::LDA_absY;
    table_instructions[0xBA] = &CPU::TSX_impl;
    table_instructions[0xBB] = &CPU::LAS_absY;
    table_instructions[0xBC] = &CPU::LDY_absX;
    table_instructions[0xBD] = &CPU::LDA_absX;
    table_instructions[0xBE] = &CPU::LDX_absY;
    table_instructions[0xBF] = &CPU::LAX_absY;

    table_instructions[0xC0] = &CPU::CPY_imm;
    table_instructions[0xC1] = &CPU::CMP_indX;
    table_instructions[0xC2] = &CPU::NOP_imm;
    table_instructions[0xC3] = &CPU::DCP_indX;
    table_instructions[0xC4] = &CPU::CPY_zp;
    table_instructions[0xC5] = &CPU::CMP_zp;
    table_instructions[0xC6] = &CPU::DEC_zp;
    table_instructions[0xC7] = &CPU::DCP_zp;
    table_instructions[0xC8] = &CPU::INY_impl;
    table_instructions[0xC9] = &CPU::CMP_imm;
    table_instructions[0xCA] = &CPU::DEX_impl;
    table_instructions[0xCB] = &CPU::SAX_imm;
    table_instructions[0xCC] = &CPU::CPY_abs;
    table_instructions[0xCD] = &CPU::CMP_abs;
    table_instructions[0xCE] = &CPU::DEC_abs;
    table_instructions[0xCF] = &CPU::DCP_abs;

    table_instructions[0xD0] = &CPU::BNE_rel;
    table_instructions[0xD1] = &CPU::CMP_indY;
    table_instructions[0xD2] = &CPU::KIL_imp;
    table_instructions[0xD3] = &CPU::DCP_indY;
    table_instructions[0xD4] = &CPU::NOP_zpX;
    table_instructions[0xD5] = &CPU::CMP_zpX;
    table_instructions[0xD6] = &CPU::DEC_zpX;
    table_instructions[0xD7] = &CPU::DCP_zpX;
    table_instructions[0xD8] = &CPU::CLD_impl;
    table_instructions[0xD9] = &CPU::CMP_absY;
    table_instructions[0xDA] = &CPU::NOP_impl_DA;
    table_instructions[0xDB] = &CPU::DCP_absY;
    table_instructions[0xDC] = &CPU::NOP_absX;
    table_instructions[0xDD] = &CPU::CMP_absX;
    table_instructions[0xDE] = &CPU::DEC_absX;
    table_instructions[0xDF] = &CPU::DCP_absX;

    table_instructions[0xE0] = &CPU::CPX_imm;
    table_instructions[0xE1] = &CPU::SBC_indX;
    table_instructions[0xE2] = &CPU::NOP_imm;
    table_instructions[0xE3] = &CPU::ISC_indX;
    table_instructions[0xE4] = &CPU::CPX_zp;
    table_instructions[0xE5] = &CPU::SBC_zp;
    table_instructions[0xE6] = &CPU::INC_zp;
    table_instructions[0xE7] = &CPU::ISC_zp;
    table_instructions[0xE8] = &CPU::INX_impl;
    table_instructions[0xE9] = &CPU::SBC_imm;
    table_instructions[0xEA] = &CPU::NOP_impl_EA;
    table_instructions[0xEB] = &CPU::SBC_imm;
    table_instructions[0xEC] = &CPU::CPX_abs;
    table_instructions[0xED] = &CPU::SBC_abs;
    table_instructions[0xEE] = &CPU::INC_abs;
    table_instructions[0xEF] = &CPU::ISC_abs;

    table_instructions[0xF0] = &CPU::BEQ_rel;
    table_instructions[0xF1] = &CPU::SBC_indY;
    table_instructions[0xF2] = &CPU::KIL_imp;
    table_instructions[0xF3] = &CPU::ISC_indY;
    table_instructions[0xF4] = &CPU::NOP_zpX;
    table_instructions[0xF5] = &CPU::SBC_zpX;
    table_instructions[0xF6] = &CPU::INC_zpX;
    table_instructions[0xF7] = &CPU::ISC_zpX;
    table_instructions[0xF8] = &CPU::SED_impl;
    table_instructions[0xF9] = &CPU::SBC_absY;
    table_instructions[0xFA] = &CPU::NOP_impl_FA;
    table_instructions[0xFB] = &CPU::ISC_absY;
    table_instructions[0xFC] = &CPU::NOP_absX;
    table_instructions[0xFD] = &CPU::SBC_absX;
    table_instructions[0xFE] = &CPU::INC_absX;
    table_instructions[0xFF] = &CPU::ISC_absX;

}

CPU::~CPU()
{

    {
        std::lock_guard<std::mutex> lock(mutex_stop);
        start = false;
    }

    if(run_t.joinable())
        run_t.join();
}

void CPU::set_flag(StatusFlags f, bool value)
{
    if(value)
        status |= f;
    else
        status &= ~f;
}

bool CPU::get_flag(StatusFlags f)
{
    return status & f;
}

bool CPU::init_new_cartridge(const QString& path)
{
    {
        std::lock_guard<std::mutex> lock(mutex_stop);

        bool status;
        bus->init_new_cartridge(path, &status);

        if(!status)
            return false;

        reset();
    }

    std::call_once(start_once_flag, [&]
    {
        start = true;
        run_t = std::thread(&CPU::run, this);
    });

    return true;

}

 std::shared_ptr<Bus> CPU::get_bus()
{
    return bus;
}

void CPU::run()
{
    while (start)
    {
        std::lock_guard<std::mutex> lock(mutex_stop);

        if(PC == 0xC000)//bus->get_RESET())
            reset();

        uint8_t val = bus->read(PC);

        auto instr_func = table_instructions[val];
        instr_func(*this);
    }
}

void CPU::reset()
{
    NMI = bus->get_NMI();
    RESET = bus->get_RESET();
    IRQ = bus->get_IRQ();

    A  = 0x00;
    X  = 0x00;
    Y  = 0x00;

    PC = 0xC000; //RESET;

    SP = 0xFD;
    status = 0x24; //0x34;

    cycles = 7;
}

uint8_t CPU::immediate(uint16_t* addr)
{
    ++cycles;

    if(addr)
        *addr = PC;

    return bus->read(PC++);
}

uint8_t CPU::zero_page(uint16_t* addr)
{
    uint8_t a1 = immediate();

    ++cycles;

    if(addr)
        *addr = a1;

    return bus->read(a1);
}

uint8_t CPU::zero_pageX(uint16_t* addr)
{
    uint8_t a1 = immediate();

    cycles += 2;

    if(addr)
        *addr = a1 + X;

    return bus->read(a1 + X);
}

uint8_t CPU::zero_pageY(uint16_t* addr)
{
    uint8_t a1 = immediate();

    cycles += 2;

    if(addr)
        *addr = a1 + Y;

    return bus->read(a1 + Y);
}

uint8_t CPU::accumulator()
{
    return A;
}

uint8_t CPU::absolute(uint16_t* addr)
{
    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    uint16_t addr2 = (a2 << 8) | a1;

    ++cycles;

    if(addr)
        *addr = addr2;

    return bus->read(addr2);
}

uint8_t CPU::absoluteX(uint16_t* addr)
{
    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    uint16_t base = (a2 << 8) | a1;
    uint16_t addr2 = base + X;

    if((base & 0xFF00) != (addr2 & 0xFF00))
        ++cycles;

    ++cycles;

    if(addr)
        *addr = addr2;

    return bus->read(addr2);
}

uint8_t CPU::absoluteY(uint16_t* addr)
{
    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    uint16_t base = (a2 << 8) | a1;
    uint16_t addr2 = base + Y;

    if((base & 0xFF00) != (addr2 & 0xFF00))
        ++cycles;

    ++cycles;

    if(addr)
        *addr = addr2;

    return bus->read(addr2);
}

uint16_t CPU::indirect(uint16_t* addr)
{
    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    uint16_t base = (a2 << 8) | a1;

    if(addr)
        *addr = base;

    uint8_t al = bus->read(base);
    uint16_t ah;

    if ((base & 0x00FF) == 0x00FF)
        ah = bus->read(base & 0xFF00);
    else
        ah = bus->read(base + 1);

    cycles += 4;
    return (ah << 8) | al;
}

uint8_t CPU::indexed_inderectX(uint16_t* addr)
{
    uint8_t a1 = immediate();

    uint8_t al = bus->read((a1 + X) & 0xFF);
    uint8_t ah = bus->read((a1 + X + 1) & 0xFF);
    uint16_t addr2 = (ah << 8) | al;

    cycles += 4;

    if(addr)
        *addr = addr2;

    return bus->read(addr2);
}

uint8_t CPU::indexed_inderectY(uint16_t* addr)
{
    uint8_t a1 = immediate();

    uint8_t al = bus->read(a1);
    uint8_t ah = bus->read((a1 + 1) & 0xFF);
    uint16_t base = (ah << 8) | al;
    uint16_t addr2 = base + Y;

    if((base & 0xFF00) != (addr2 & 0xFF00))
        ++cycles;

    cycles += 3;

    if(addr)
        *addr = addr2;

    return bus->read(addr2);
}

int8_t CPU::relative(uint16_t* addr)
{
    return immediate(addr);
}

void CPU::BRK_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x00, -1, -1};
    LOG::Write(PC, ddd, QString("BRK"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    immediate();

    bus->write(0x0100 + SP--, (PC >> 8) & 0xFF);
    bus->write(0x0100 + SP--, PC & 0xFF);

    set_flag(StatusFlags::B, true);
    set_flag(StatusFlags::U, true);

    bus->write(0x0100 + SP--, status);

    set_flag(StatusFlags::I, true);

    PC = bus->get_IRQ();

    cycles += 6;
}

void CPU::ORA_base(uint8_t val)
{
    A = A | val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    ++cycles;
}

void CPU::ORA_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x15, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("ORA $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x05, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ORA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ddd[] = {0x09, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("ORA #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x0D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ORA $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x1D,
                      (int16_t)((addr - X) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,X @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ORA $%1%2,X @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x19,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ORA $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x11,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("ORA ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ORA_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    ORA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x01,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("ORA ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

uint8_t CPU::ASL_base(uint8_t val)
{
    set_flag(StatusFlags::C, val & 0x80);

    val = val << 1;

    set_flag(StatusFlags::Z, val == 0);
    set_flag(StatusFlags::N, val & 0x80);

    cycles +=2;
    return val;
}

void CPU::ASL_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x06, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ASL $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ASL_A()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    A = ASL_base(A);

#if LOG_ON
    int16_t ddd[] = {0x0A, -1, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ASL A"), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ASL_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x0E,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ASL $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ASL_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x1E,
                      (int16_t)((addr - X) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,X @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ASL $%1%2,X @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ASL_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x16, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("ASL $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::PHP_impl()
{

#if LOG_ON
    int16_t ddd[] = {0x08, -1, -1};
    LOG::Write(PC, ddd, QString("PHP"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    bus->write(0x0100 + SP--, status | 0x30);

    cycles += 3;
}

void CPU::BPL_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::N) == 0)
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0x10, val, -1};
    LOG::Write(old_PC, ddd, QString("BPL $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CLC_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x18, -1, -1};
    LOG::Write(PC, ddd, QString("CLC"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    set_flag(StatusFlags::C, false);

    cycles += 2;
}

void CPU::JSR_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t return_addr = PC + 1;

    bus->write(0x0100 + SP--, (return_addr >> 8) & 0xFF);
    bus->write(0x0100 + SP--, return_addr & 0xFF);

    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    PC = (a2 << 8) | a1;

    cycles += 4;

#if LOG_ON
    int16_t ddd[] = { 0x20,
                      a1,
                      a2
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("JSR $%1%2").arg(a2, 2, 16,QLatin1Char('0')).toUpper().
                    arg(a1, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_base(uint8_t val)
{
    A = A & val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    ++cycles;
}

void CPU::AND_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x21,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("AND ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x31,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("AND ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x25, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("AND $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x35, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("AND $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ddd[] = {0x29, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("AND #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x2D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("AND $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x3D,
                      (int16_t)((addr - X) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,X @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("AND $%1%2,X @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AND_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    AND_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x19,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ORA $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::BIT_base(uint8_t val)
{
    int8_t tmp = static_cast<int8_t>(A & val);

    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, val & 0x80);
    set_flag(StatusFlags::V, val & 0x40);

    ++cycles;
}

void CPU::BIT_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    BIT_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x24, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("BIT $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::BIT_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    BIT_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x2C,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("BIT $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::BIT_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    BIT_base(val);

#if LOG_ON
    int16_t ddd[] = {0x89, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("BIT #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

uint8_t CPU::ROL_base(uint8_t val)
{
    uint8_t val_lod = val;
    bool C_old = get_flag(StatusFlags::C);

    set_flag(StatusFlags::C, val_lod & 0x80);
    val = (val_lod << 1) & 0xFF;

    if(C_old)
        val |= 0x01;

    set_flag(StatusFlags::Z, val == 0);
    set_flag(StatusFlags::N, val & 0x80);

    cycles += 2;

    return val;
}

void CPU::ROL_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x26, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ROL $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROL_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x36, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("ROL $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROL_A()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    A = ROL_base(A);

#if LOG_ON
    int16_t ddd[] = {0x2A, -1, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ROL A"), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROL_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x2E,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ROL $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROL_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x3E,
                      (int16_t)((addr - X) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,X @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ROL $%1%2,X @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::PLP_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x28, -1, -1};
    LOG::Write(PC, ddd, QString("PLP"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    status = bus->read(0x0100 + ++SP);
    status = (status & 0xEF) | 0x20;

    cycles += 4;
}

void CPU::BMI_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::N))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0x30, val, -1};
    LOG::Write(old_PC, ddd, QString("BMI $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SEC_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x38, -1, -1};
    LOG::Write(PC, ddd, QString("SEC"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    set_flag(StatusFlags::C, true);

    cycles += 2;
}

void CPU::RTI_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x40, -1, -1};
    LOG::Write(PC, ddd, QString("RTI"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    status = bus->read(0x0100 + ++SP);

    status |= 0x20;

    uint8_t PCl = bus->read(0x0100 + ++SP);
    uint8_t PCh = bus->read(0x0100 + ++SP);

    PC = (uint16_t)PCh << 8 | PCl;

    cycles += 6;
}

void CPU::EOR_base(uint8_t val)
{
    A = A ^ val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    ++cycles;
}

void CPU::EOR_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x41,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("EOR ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x51,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("EOR ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x45, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("EOR $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x55, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("EOR $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    EOR_base(val);

#if LOG_ON
    int16_t ddd[] = {0x49, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("EOR #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x4D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("EOR $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x5D,
                      (int16_t)((addr - X) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,X @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ROL $%1%2,X @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::EOR_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    EOR_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x59,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("EOR $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

uint8_t CPU::LSR_base(uint8_t val)
{
    set_flag(StatusFlags::C, val & 0x01);
    val = val >> 1;

    set_flag(StatusFlags::Z, val == 0);
    set_flag(StatusFlags::N, false);

    cycles += 2;

    return val;
}

void CPU::LSR_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x46, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LSR $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LSR_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x56, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("LSR $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LSR_A()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    A = LSR_base(A);

#if LOG_ON
    int16_t ddd[] = {0x4A, -1, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LSR A"), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LSR_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x4E,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("LSR $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LSR_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x5E,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("LSR $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::PHA_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x48, -1, -1};
    LOG::Write(PC, ddd, QString("PHA"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    uint16_t addr = 0x0100 | SP--;
    bus->write(addr, A);

    cycles += 3;
}

void CPU::JMP_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t a1 = immediate();
    uint8_t a2 = immediate();

    PC = (a2 << 8) | a1;

    ++cycles;

#if LOG_ON
    int16_t ddd[] = {0x4C, a1, a2};
    LOG::Write(old_PC, ddd, QString("JMP $%1%2").arg(a2, 2, 16, QLatin1Char('0')).toUpper().arg(a1, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::JMP_ind()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    PC = indirect(&addr);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x6C,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $10 = 00

    LOG::Write(old_PC, ddd, QString("JMP %1").arg(addr, 4, 16, QLatin1Char('0')), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::BVC_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    int8_t val = relative();

    if(!get_flag(StatusFlags::V))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0x50, val, -1};
    LOG::Write(old_PC, ddd, QString("BVC $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CLI_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x58, -1, -1};
    LOG::Write(PC, ddd, QString("CLI"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    set_flag(StatusFlags::I, false);

    cycles += 2;
}

void CPU::RTS_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x60, -1, -1};
    LOG::Write(PC, ddd, QString("RTS"), A, X, Y, status, SP, cycles);
#endif

    uint8_t lo = bus->read(0x0100 + ++SP);
    uint8_t hi = bus->read(0x0100 + ++SP);

    PC = ((uint16_t)hi << 8) | lo;
    ++PC;

    cycles += 6;
}

void CPU::ADC_base(uint8_t val)
{
    uint16_t tmp = A + val + (get_flag(StatusFlags::C) ? 1 : 0);
    uint8_t result = (uint8_t)(tmp & 0xFF);
    bool overflow = (~(A ^ val) & (A ^ result) & 0x80) != 0;

    if(!get_flag(StatusFlags::D))
    {
        set_flag(StatusFlags::C, tmp > 0xFF);
        set_flag(StatusFlags::V, overflow);

        A = result;

        set_flag(StatusFlags::Z, A == 0);
        set_flag(StatusFlags::N, A & 0x80);
    }
    else
    {
        uint8_t al = (tmp & 0x0F);

        if (al > 9)
            tmp += 0x06;

        if ((tmp & 0x1F0) > 0x90)
            tmp += 0x60;

        set_flag(StatusFlags::C, tmp > 0xFF);

        A = result;

        set_flag(StatusFlags::Z, A == 0);
        set_flag(StatusFlags::N, A & 0x80);
        set_flag(StatusFlags::V, overflow);
    }

    ++cycles;
}

void CPU::ADC_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x61,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("ADC ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x71,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("ADC ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x46, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ADC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x75, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ADC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);

    ADC_base(val);

#if LOG_ON

    int16_t ddd[] = {0x69, val, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ADC #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x6D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ADC $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x7D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ADC $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ADC_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    ADC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0x79,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ADC $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

uint8_t CPU::ROR_base(uint8_t val)
{
    uint8_t oldC = get_flag(StatusFlags::C);
    uint8_t newC= val & 0x01;

    uint8_t result = (val >> 1) | (oldC << 7);

    set_flag(StatusFlags::C, newC != 0);
    set_flag(StatusFlags::Z, result == 0);
    set_flag(StatusFlags::N, result & 0x80);

    cycles += 2;

    return result;
}

void CPU::ROR_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x66, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ROR $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROR_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x76, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ROR $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROR_A()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    A = ROR_base(A);

#if LOG_ON
    int16_t ddd[] = {0x6A, -1, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ROR A"), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROR_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x6E,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ROR $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ROR_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x7E,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ROR $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::PLA_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x68, -1, -1};
    LOG::Write(PC, ddd, QString("PLA"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    A = bus->read(0x0100 + ++SP);

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 4;
}

void CPU::BVS_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::V))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0x70, val, -1};
    LOG::Write(old_PC, ddd, QString("BVS $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SEI_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x78, -1, -1};
    LOG::Write(PC, ddd, QString("SEI"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    set_flag(StatusFlags::I, true);

    cycles += 2;
}

void CPU::STA_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x81,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("STA ($%1,X)").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STA_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    indexed_inderectY(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x91,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("STA ($%1),Y").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STA_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x85, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STA_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x95, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STA_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x8D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("STA $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STA_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x9D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("STA $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STA_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    bus->write(addr, A);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x99,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("STA $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STY_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    zero_page(&addr);

    bus->write(addr, Y);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x84, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(Y, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STY_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    zero_pageX(&addr);

    bus->write(addr, Y);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x94, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(Y, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STY_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    absolute(&addr);

    bus->write(addr, Y);
    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x8D,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("STY $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(Y, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STX_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    zero_page(&addr);

    bus->write(addr, X);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x86, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(X, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STX_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    zero_pageX(&addr);

    bus->write(addr, X);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x96, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(X, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::STX_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    bus->write(addr, X);
    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x8E,
                     ss,
                     (int16_t)(addr >> 8)};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("STX $%1 = %2").arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DEY_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x88, -1, -1};
    LOG::Write(PC, ddd, QString("DEY"), A, X, Y, status, SP, cycles);
#endif

    ++PC;
    --Y;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    cycles += 2;
}

void CPU::TXA_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x8A, -1, -1};
    LOG::Write(PC, ddd, QString("TXA"), A, X, Y, status, SP, cycles);
#endif

    ++PC;
    A = X;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 2;
}

void CPU::BCC_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    int8_t val = relative();

    if(!get_flag(StatusFlags::C))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0x90, val, -1};
    LOG::Write(old_PC, ddd, QString("BCC $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::TYA_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x98, -1, -1};
    LOG::Write(PC, ddd, QString("TYA"), A, X, Y, status, SP, cycles);
#endif

    ++PC;
    A = Y;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 2;
}

void CPU::TXS_impl()
{
#if LOG_ON
    int16_t ddd[] = {0x9A, -1, -1};
    LOG::Write(PC, ddd, QString("TXS"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    SP = X;

    cycles += 2;
}

void CPU::LDY_base(uint8_t val)
{
    Y = val;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    ++cycles;
}

void CPU::LDY_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    LDY_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xA4, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDY_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    LDY_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xB4, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDY_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();
    LDY_base(val);

#if LOG_ON
    int16_t ddd[] = {0xA0, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("LDY #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDY_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    LDY_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xAC, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDY_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    LDY_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xBC,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("LDY $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_base(uint8_t val)
{
    A = val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    ++cycles;
}

void CPU::LDA_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    if(PC == 0xCFFA)
        int d = 9;

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);
    LDA_base(val);

#if LOG_ON
    uint8_t a1 = bus->read(old_PC + 1);
    int16_t ddd[] = { 0xA1,
                      a1,
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("LDA ($%1,X) @ %2 = %3 = %4").
                    arg(a1, 2, 16,QLatin1Char('0')).toUpper().
                    arg((uint8_t)(a1 + X), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);
    LDA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xB1,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("LDA ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    LDA_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xA5, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    LDA_base(val);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xB5, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("LDA $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();
    LDA_base(val);

#if LOG_ON
    int16_t ddd[] = {0xA9, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("LDA #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    LDA_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xAD, ss, (int16_t)(addr >> 8)}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDA $%1 = %2").arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    LDA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xBD,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("LDA $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDA_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);
    LDA_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xB9,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("LDA $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDX_base(uint8_t val)
{
    X = val;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    ++cycles;
}

void CPU::LDX_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    LDX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xA6, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDX_zpY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageY(&addr);
    LDX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xA4, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDX_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);
    LDX_base(val);

#if LOG_ON
    int16_t ddd[] = {0xA2, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("LDX #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDX_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    LDX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xAE, ss, (int16_t)(addr >> 8)};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LDX $%1 = %2").arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LDX_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);
    LDX_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xBE,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("LDX $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::TAY_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xA8, -1, -1};
    LOG::Write(PC, ddd, QString("TAY"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    Y = A;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    cycles += 2;
}

void CPU::BCS_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::C))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0xB0, val, -1};
    LOG::Write(old_PC, ddd, QString("BCS $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CLV_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xB8, -1, -1};
    LOG::Write(PC, ddd, QString("CLV"), A, X, Y, status, SP, cycles);
#endif


    ++PC;

    set_flag(StatusFlags::V, false);

    cycles += 2;
}

void CPU::TSX_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xBA, -1, -1};
    LOG::Write(PC, ddd, QString("TSX"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    X = SP;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::CPY_base(uint8_t val)
{
    uint8_t tmp = Y - val;

    set_flag(StatusFlags::C, Y >= val);
    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, tmp & 0x80);

    ++cycles;
}

void CPU::CPY_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();
    CPY_base(val);

#if LOG_ON
    int16_t ddd[] = {0xC0, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("CPY #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CPY_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    CPY_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xC4, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("CPY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CPY_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    CPY_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xCC, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("CPY $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_base(uint8_t val)
{
    uint8_t tmp = A - val;

    set_flag(StatusFlags::C, A >= val);
    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, tmp & 0x80);

    ++cycles;
}

void CPU::CMP_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xC1,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("CMP ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xD1,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("CMP ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xC5, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("CMP $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xD5, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("CMP $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();
    CMP_base(val);

#if LOG_ON
    int16_t ddd[] = {0xC9, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("CMP #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xCD, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("CMP $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xDD,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("CMP $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CMP_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);
    CMP_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xD9,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("CMP $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

uint8_t CPU::DEC_base(uint8_t val)
{
    --val;

    set_flag(StatusFlags::Z, val == 0);
    set_flag(StatusFlags::N, val & 0x80);

    cycles += 2;
    return val;
}

void CPU::DEC_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xC6, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("DEC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DEC_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xD6, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("DEC $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DEC_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xCE, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("DEC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DEC_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0xDE,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("DEC $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::INY_impl()
{

#if LOG_ON
    int16_t ddd[] = {0xC8, -1, -1};
    LOG::Write(PC, ddd, QString("INY"), A, X, Y, status, SP, cycles);
#endif

    ++PC;
    ++Y;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    cycles += 2;
}

void CPU::DEX_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xCA, -1, -1};
    LOG::Write(PC, ddd, QString("DEX"), A, X, Y, status, SP, cycles);
#endif

    ++PC;
    --X;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::BNE_rel()
{

#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif


    ++PC;

    uint8_t val = relative();

    if(!get_flag(StatusFlags::Z))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - 2 - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0xD0, val, -1};
    LOG::Write(old_PC, ddd, QString("BNE $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CLD_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xD8, -1, -1};
    LOG::Write(PC, ddd, QString("CLD"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    set_flag(StatusFlags::D, false);

    cycles += 2;
}

void CPU::CPX_base(uint8_t val)
{
    uint8_t tmp = X - val;

    set_flag(StatusFlags::C, X >= val);
    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, tmp & 0x80);

    ++cycles;
}

void CPU::CPX_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    CPX_base(val);

#if LOG_ON
    int16_t ddd[] = {0xE0, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("CPX #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CPX_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    CPX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xE4, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("CPX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::CPX_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    CPX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xEC, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("CPX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_base(uint8_t val)
{
    uint16_t tmp = A - val - (get_flag(StatusFlags::C) ? 0 : 1);

    if(!get_flag(StatusFlags::D))
    {
        uint8_t tmp8 = (uint8_t)tmp;

        set_flag(StatusFlags::C, tmp < 0x100);
        set_flag(StatusFlags::Z, tmp8 == 0);
        set_flag(StatusFlags::N, tmp8 & 0x80);
        set_flag(StatusFlags::V, ((A ^ val) & (A ^ tmp8) & 0x80));

        A = tmp8;
    }
    else
    {
        uint8_t al = (A & 0x0F);
        uint8_t ah = (A >> 4);

        uint8_t ml = (val & 0x0F);
        uint8_t mh = (val >> 4);

        int8_t l = al - ml - (get_flag(StatusFlags::C) ? 0 : 1);
        int8_t h = ah - mh;

        if (l < 0)
        {
            l -= 6;
            h -= 1;
        }

        if (h < 0)
            h -= 6;

        uint8_t result = ((h << 4) & 0xF0) | (l & 0x0F);

        set_flag(StatusFlags::C, tmp < 0x100);
        set_flag(StatusFlags::Z, result == 0);
        set_flag(StatusFlags::N, result & 0x80);
        set_flag(StatusFlags::V, ((A ^ val) & (A ^ tmp) & 0x80));

        A = result;
    }

    ++cycles;
}

void CPU::SBC_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xE1,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("SBC ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xF1,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("SBC ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xE5, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SBC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xF5, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("SBC $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    SBC_base(val);

#if LOG_ON
    int16_t ddd[] = {0xE9, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("SBC #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_abs()
{

#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xED, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SBC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xFD,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("SBC $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SBC_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    SBC_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xF9,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("SBC $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

uint8_t CPU::INC_base(uint8_t val)
{
    ++val;

    set_flag(StatusFlags::Z, val == 0);
    set_flag(StatusFlags::N, val & 0x80);

    cycles += 2;
    return val;
}

void CPU::INC_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xE6, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("INC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::INC_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xF6, ss, -1};//LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("INC $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::INC_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xEE, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("INC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::INC_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0xFE,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("INC $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::INX_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xE8, -1, -1};
    LOG::Write(PC, ddd, QString("INX"), A, X, Y, status, SP, cycles);
#endif

    ++PC;
    ++X;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::BEQ_rel()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = relative();

    if(get_flag(StatusFlags::Z))
    {
        ++cycles;
        PC += val;

        if(PC >> 8 != (PC - val) >> 8)
            ++cycles;
    }

    ++cycles;

#if LOG_ON
    //val = std::abs(val);
    int16_t ddd[] = {0xF0, val, -1};
    LOG::Write(old_PC, ddd, QString("BEQ $%1").arg(old_PC + 2 + val, 4, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SED_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xF8, -1, -1};
    LOG::Write(PC, ddd, QString("SED"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    set_flag(StatusFlags::D, true);

    cycles += 2;
}

void CPU::TAX_impl()
{
#if LOG_ON
    int16_t ddd[] = {0xAA, -1, -1};
    LOG::Write(PC, ddd, QString("TAX"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    X = A;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::NOP_impl_DA()
{
#if LOG_ON
    int16_t ddd[] = {0xDA, -1, -1};
    LOG::Write(PC, ddd, QString("NOP"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    cycles += 2;
}

void CPU::NOP_impl_EA()
{
#if LOG_ON
    int16_t ddd[] = {0xEA, -1, -1};
    LOG::Write(PC, ddd, QString("NOP"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    cycles += 2;
}

void CPU::NOP_impl_FA()
{
#if LOG_ON
    int16_t ddd[] = {0xFA, -1, -1};
    LOG::Write(PC, ddd, QString("NOP"), A, X, Y, status, SP, cycles);
#endif

    ++PC;

    cycles += 2;
}

void CPU::NOP_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    --PC;

    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x04, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("NOP $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::NOP_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    --PC;
    ++cycles;

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x34, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("NOP $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::NOP_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    PC -= 2;

    ++cycles;

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x0C, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("NOP $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::NOP_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0x1C,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("NOP $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::NOP_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    ++cycles;

#if LOG_ON
    int16_t ddd[] = {0x1A, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("NOP #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_base(uint8_t val, uint16_t addr)
{
    set_flag(StatusFlags::C, val & 0x80);

    val = val << 1;
    bus->write(addr, val);
    A = A | val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 3;
}

void CPU::SLO_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x03,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("SLO ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x07, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SLO $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x0F, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SLO $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x13,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("SLO ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x17, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("SLO $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x1B,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("SLO $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SLO_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    SLO_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x1F,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("SLO $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RLA_base(uint8_t val, uint16_t addr)
{
    bool oldC = get_flag(C);

    set_flag(StatusFlags::C, val & 0x80);
    val = ((val << 1) | oldC);
    bus->write(addr, val);

    A = A & val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 3;
}

void CPU::RLA_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x23,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("RLA ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif

}

void CPU::RLA_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x27, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("RLA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RLA_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x2F, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("RLA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RLA_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x33,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("RLA ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RLA_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x37, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("RLA $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RLA_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x3B,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("RLA $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RLA_absX()
{

#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    RLA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x3F,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("RLA $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_base(uint8_t val, uint16_t addr)
{
    uint8_t oldC = val & 0x01;

    val = val >> 1;
    bus->write(addr, val);

    A = A ^ val;

    set_flag(StatusFlags::C, oldC);
    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 3;
}

void CPU::SRE_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x43,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("SRE ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x47, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SRE $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x4F, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SRE $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x53,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("SRE ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x57, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("SRE $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x5B,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("SRE $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SRE_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    SRE_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x5F,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("SRE $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_base(uint8_t val, uint16_t addr)
{
    uint8_t oldC = get_flag(StatusFlags::C);

    bool oldBit0 = val & 0x01;
    val = (val >> 1) | (oldC ? 0x80 : 0x00);
    bus->write(addr, val);

    uint16_t sum = (uint16_t)A + (uint16_t)val + (oldBit0 ? 1 : 0);

    set_flag(StatusFlags::C, sum > 0xFF);
    set_flag(StatusFlags::V, ~(A ^ val) & (A ^ sum));
    A = sum;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 3;
}

void CPU::RRA_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x63,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("RPA ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x67, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("RPA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x6F, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("RPA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif


    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x73,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("RPA ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0x77, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("RPA $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x7B,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("RPA $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::RRA_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    RRA_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0x7F,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("RPA $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_base(uint8_t val, uint16_t addr)
{
    val = (val - 1);
    bus->write(addr, val);

    uint8_t tmp = (A - val);

    set_flag(StatusFlags::C, A >= val);
    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, tmp & 0x80);

    cycles += 3;
}

void CPU::DCP_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xC3,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("DCP ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xC7, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("DCP $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xCF, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("RPA $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xD3,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("DCP ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xD7, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("DCP $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xDB,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("DCP $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::DCP_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    DCP_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xDF,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("DCP $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_base(uint8_t val, uint16_t addr)
{
    val = (val + 1);
    bus->write(addr, val);

    uint16_t value = (uint16_t)val ^ 0x00FF;
    uint16_t sum   = (uint16_t)A + value + (get_flag(C) ? 1 : 0);

    set_flag(StatusFlags::C, sum & 0x100);
    set_flag(StatusFlags::V, ~(A ^ value) & (A ^ sum));
    A = sum;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 3;
}

void CPU::ISC_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xE3,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("ISC ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xE7, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ISC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xEF, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("ISC $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xF3,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("ISC ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_zpX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ss = (addr - X) & 0xFF;
    int16_t ddd[] = {0xF7, ss, -1}; //LDA $10,X @ 20 = F
    LOG::Write(old_PC, ddd, QString("ISC $%1,X @ %2 = %3").arg(ss, 2, 16, QLatin1Char('0')).toUpper().arg(addr, 2, 16).arg(val, 2, 16, QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xFB,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("ISC $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ISC_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    ISC_base(val, addr);

#if LOG_ON
    int16_t ddd[] = { 0xFF,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("ISC $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_base(uint8_t val)
{
    A = val;
    X = val;

    set_flag(StatusFlags::Z, val == 0);
    set_flag(StatusFlags::N, val & 0x80);

    ++cycles;
}

void CPU::LAX_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xA3,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("LAX ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xA7, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LAX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xAF, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LAX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xB3,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("LAX ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ddd[] = { 0xBF,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("LAX $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_zpY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageY(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0xB7, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("LAX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAX_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);

    LAX_base(val);

#if LOG_ON
    int16_t ddd[] = {0xAB, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("LAX #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SAX_base(uint16_t addr)
{
    uint8_t val = A & X;
    bus->write(addr, val);

    ++cycles;
}

void CPU::SAX_indX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    SAX_base(addr);

#if LOG_ON
    int16_t ddd[] = { 0x83,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($80,X) @ 80 = 0200 = AA

    LOG::Write(old_PC, ddd,
               QString("SAX ($%1,X) @ %2 = %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg((bus->read(old_PC + 1) + X) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SAX_zp()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    SAX_base(addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x87, ss, -1}; //ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SAX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SAX_abs()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    SAX_base(addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x8F, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SAX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SAX_zpY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageY(&addr);

    SAX_base(addr);

#if LOG_ON
    int16_t ss = (addr) & 0xFF;
    int16_t ddd[] = {0x97, ss, -1};//ORA $10 = 00
    LOG::Write(old_PC, ddd, QString("SAX $%1 = %2").arg(ss, 2, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SAX_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);

    SAX_base(addr);

#if LOG_ON
    int16_t ddd[] = {0xCB, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("SAX #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ANC_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    A = A & val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);
    set_flag(StatusFlags::C, A & 0x80);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = {0x0B, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("ANC #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ALR_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    A = A & val;

    set_flag(StatusFlags::C, A & 0x01);

    A = A >> 1;
    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, false);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = {0x4B, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("ALR #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::ARR_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    if(!get_flag(StatusFlags::D))
    {
        A = A & val;

        bool C = get_flag(StatusFlags::C);
        set_flag(StatusFlags::C, A & 0x01);

        A = (A >> 1) | (C ? 0x80 : 0);

        set_flag(StatusFlags::Z, A == 0);
        set_flag(StatusFlags::N, A & 0x80);
        set_flag(StatusFlags::V, (A ^ (A << 1)) & 0x40);
    }

    cycles += 2;

#if LOG_ON
    int16_t ddd[] = {0x6B, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("ARR #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::XAA_imm()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint8_t val = immediate();

    A = A & val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = {0x8B, val, -1};//ORA #$00
    LOG::Write(old_PC, ddd, QString("XAA #$%1").arg(val, 2, 16,QLatin1Char('0')).toUpper(), old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AHX_base(uint16_t addr)
{
    uint8_t highp1 = (uint8_t)((addr >> 8) + 1);
    uint8_t val = A & X & highp1;

    bus->write(addr, val);

    cycles += 2;
}

void CPU::AHX_indY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    AHX_base(addr);

#if LOG_ON
    int16_t ddd[] = { 0x93,
                      bus->read(old_PC + 1),
                      -1
                    }; //ORA ($33),Y = 0400 @ 0400 = AA

    LOG::Write(old_PC, ddd,
               QString("AHX ($%1),Y = %2 @ %3 = %4").
                    arg(bus->read(old_PC + 1), 2, 16,QLatin1Char('0')).toUpper().
                    arg(addr - Y, 4, 16,QLatin1Char('0')).toUpper().
                    arg(addr, 4, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::AHX_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    AHX_base(addr);

#if LOG_ON
    int16_t ddd[] = { 0x9F,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("AHX $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SHY_absX()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    absoluteX(&addr);

    uint16_t eff = (uint16_t)(addr + X);
    uint8_t highp1 = (uint8_t)((eff >> 8) + 1);
    uint8_t val = Y & highp1;
    bus->write(eff, val);

    cycles += 2;

#if LOG_ON
    int16_t ddd[] = { 0x9C,
                      (int16_t)((addr) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $2000

    LOG::Write(old_PC, ddd,
               QString("SHY $%1%2 = %3").arg(addr >> 8, 2, 16,QLatin1Char('0')).toUpper().
                    arg((addr) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().
                    arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::TAS_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    absoluteY(&addr);

    uint16_t eff = addr + Y;
    uint8_t highp1 = (uint8_t)((eff >> 8) + 1);
    uint8_t val = SP & highp1;
    bus->write(eff, val);

    cycles += 2;

#if LOG_ON
    int16_t ddd[] = { 0x9B,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("TAS $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::SHX_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t base;
    absoluteY(&base);

    uint16_t addr = base + Y;
    uint8_t highp1 = (uint8_t)((addr >> 8) + 1);
    uint8_t val = X & highp1;
    bus->write(addr, val);

    cycles += 2;

#if LOG_ON
    int16_t ddd[] = { 0x9E,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("SHX $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::LAS_absY()
{
#if LOG_ON
    uint16_t old_PC = PC;
    auto old_cycles = cycles;
    auto old_A = A;
    auto old_X = X;
    auto old_Y = Y;
    auto old_status = status;
    auto old_SP = SP;
#endif

    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    uint8_t tmp = val & SP;
    A  = tmp;
    X  = tmp;
    SP = tmp;

    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, tmp & 0x80);

    ++cycles;

#if LOG_ON
    int16_t ddd[] = { 0xBB,
                      (int16_t)((addr - Y) & 0xFF),
                      (int16_t)(addr >> 8)
                    }; //ORA $C000,Y @ C010 = 42

    LOG::Write(old_PC, ddd,
               QString("LAS $%1%2,Y @ %3 = %4").arg((addr >> 8), 2, 16,QLatin1Char('0')).toUpper().arg((addr - Y) & 0xFF, 2, 16,QLatin1Char('0')).toUpper().arg(addr, 4, 16,QLatin1Char('0')).toUpper().arg(val, 2, 16,QLatin1Char('0')).toUpper(),
               old_A, old_X, old_Y, old_status, old_SP, old_cycles);
#endif
}

void CPU::KIL_imp()
{
    emit signal_error_show();

    start = false;
}

