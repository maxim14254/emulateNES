#include "cpu.h"
#include "bus.h"
#include <QMessageBox>


CPU::CPU()
{
    bus.reset(new Bus());

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
    table_instructions[0x11] = &CPU::ORA_indX;
    table_instructions[0x12] = &CPU::KIL_imp;
    table_instructions[0x13] = &CPU::SLO_indY;
    table_instructions[0x14] = &CPU::NOP_zp;
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
    table_instructions[0xDA] = &CPU::NOP_impl;
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
    table_instructions[0xEA] = &CPU::NOP_impl;
    table_instructions[0xEB] = &CPU::SBC_imm;
    table_instructions[0xEC] = &CPU::CPX_abs;
    table_instructions[0xED] = &CPU::SBC_abs;
    table_instructions[0xEE] = &CPU::DEC_absX;
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
    table_instructions[0xFA] = &CPU::NOP_impl;
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

QSharedPointer<Bus> CPU::get_bus()
{
    return bus;
}

void CPU::run()
{
    while (start)
    {
        std::lock_guard<std::mutex> lock(mutex_stop);

        if(PC == bus->get_RESET())
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

    PC = RESET;

    SP = 0xFD;
    status = 0x34;

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

uint16_t CPU::indirect()
{
    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    uint16_t base = (a2 << 8) | a1;

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

    uint8_t al = bus->read(a1 + X);
    uint8_t ah = bus->read(a1 + X + 1);
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
    uint8_t ah = bus->read(a1 + 1);
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
    ++PC;

    uint8_t val = zero_pageX();

    ORA_base(val);
}

void CPU::ORA_zp()
{
    ++PC;

    uint8_t val = zero_page();

    ORA_base(val);
}

void CPU::ORA_imm()
{
    ++PC;

    uint8_t val = immediate();

    ORA_base(val);
}

void CPU::ORA_abs()
{
    ++PC;

    uint8_t val = absolute();

    ORA_base(val);
}

void CPU::ORA_absX()
{
    ++PC;

    uint8_t val = absoluteX();

    ORA_base(val);
}

void CPU::ORA_absY()
{
    ++PC;

    uint8_t val = absoluteY();

    ORA_base(val);
}

void CPU::ORA_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();

    ORA_base(val);
}

void CPU::ORA_indX()
{
    ++PC;

    uint8_t val = indexed_inderectX();

    ORA_base(val);
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
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ASL_A()
{
    ++PC;

    ASL_base(A);
}

void CPU::ASL_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ASL_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ASL_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = ASL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::PHP_impl()
{
    ++PC;

    set_flag(StatusFlags::B, true);
    set_flag(StatusFlags::U, true);

    bus->write(0x0100 + SP--, status);

    cycles += 3;
}

void CPU::BPL_rel()
{
    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::N) == 0)
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::CLC_impl()
{
    ++PC;

    set_flag(StatusFlags::C, false);

    cycles += 2;
}

void CPU::JSR_abs()
{
    ++PC;

    uint16_t return_addr = PC + 1;

    bus->write(0x0100 + SP--, (return_addr >> 8) & 0xFF);
    bus->write(0x0100 + SP--, return_addr & 0xFF);

    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    PC = (a2 << 8) | a1;

    cycles += 4;
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
    ++PC;

    uint8_t val = indexed_inderectX();

    AND_base(val);
}

void CPU::AND_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();

    AND_base(val);
}

void CPU::AND_zp()
{
    ++PC;

    uint8_t val = zero_page();

    AND_base(val);
}

void CPU::AND_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();

    AND_base(val);
}

void CPU::AND_imm()
{
    ++PC;

    uint8_t val = immediate();

    AND_base(val);
}

void CPU::AND_abs()
{
    ++PC;

    uint8_t val = absolute();

    AND_base(val);
}

void CPU::AND_absX()
{
    ++PC;

    uint8_t val = absoluteX();

    AND_base(val);
}

void CPU::AND_absY()
{
    ++PC;

    uint8_t val = absoluteY();

    AND_base(val);
}

void CPU::BIT_base(uint8_t val)
{
    uint8_t tmp = A & val;

    set_flag(StatusFlags::Z, tmp == 0);

    ++cycles;
}

void CPU::BIT_zp()
{
    ++PC;

    uint8_t val = zero_page();

    BIT_base(val);
}

void CPU::BIT_abs()
{
    ++PC;

    uint8_t val = absolute();

    BIT_base(val);
}

void CPU::BIT_imm()
{
    ++PC;

    uint8_t val = immediate();

    BIT_base(val);
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
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ROL_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ROL_A()
{
    ++PC;

    ROL_base(A);
}

void CPU::ROL_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ROL_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = ROL_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::PLP_impl()
{
    ++PC;

    status = bus->read(0x0100 + ++SP);

    cycles += 4;
}

void CPU::BMI_rel()
{
    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::N))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::SEC_impl()
{
    ++PC;

    set_flag(StatusFlags::C, true);

    cycles += 2;
}

void CPU::RTI_impl()
{
    ++PC;

    status = bus->read(0x0100 + ++SP);
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
    ++PC;

    uint8_t val = indexed_inderectX();

    EOR_base(val);
}

void CPU::EOR_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();

    EOR_base(val);
}

void CPU::EOR_zp()
{
    ++PC;

    uint8_t val = zero_page();

    EOR_base(val);
}

void CPU::EOR_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();

    EOR_base(val);
}

void CPU::EOR_imm()
{
    ++PC;

    uint8_t val = immediate();

    EOR_base(val);
}

void CPU::EOR_abs()
{
    ++PC;

    uint8_t val = absolute();

    EOR_base(val);
}

void CPU::EOR_absX()
{
    ++PC;

    uint8_t val = absoluteX();

    EOR_base(val);
}

void CPU::EOR_absY()
{
    ++PC;

    uint8_t val = absoluteY();

    EOR_base(val);
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
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::LSR_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::LSR_A()
{
    ++PC;

    LSR_base(A);
}

void CPU::LSR_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::LSR_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = LSR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::PHA_impl()
{
    ++PC;

    uint16_t addr = 0x0100 | SP--;
    bus->write(addr, A);

    cycles += 3;
}

void CPU::JMP_abs()
{
    ++PC;

    uint8_t a1 = immediate();
    uint8_t a2 = immediate();
    PC = (a2 << 8) | a1;

    ++cycles;
}

void CPU::JMP_ind()
{
    ++PC;

    PC = indirect();

    ++cycles;
}

void CPU::BVC_rel()
{
    ++PC;

    int8_t val = relative();

    if(!get_flag(StatusFlags::V))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::CLI_impl()
{
    ++PC;

    set_flag(StatusFlags::I, false);

    cycles += 2;
}

void CPU::RTS_impl()
{
    uint8_t lo = bus->read(0x0100 + ++SP);
    uint8_t hi = bus->read(0x0100 + ++SP);

    PC = ((uint16_t)hi << 8) | lo;
    ++PC;

    cycles += 6;
}

void CPU::ADC_base(uint8_t val)
{
    uint16_t tmp = A + val + (get_flag(StatusFlags::C) ? 1 : 0);
    bool overflow = (~(A ^ val) & (A ^ tmp) & 0x80) != 0;

    if(!get_flag(StatusFlags::D))
    {
        set_flag(StatusFlags::C, tmp > 0xFF);
        set_flag(StatusFlags::V, overflow);

        A = tmp;

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

        A = tmp;

        set_flag(StatusFlags::Z, A == 0);
        set_flag(StatusFlags::N, A & 0x80);
        set_flag(StatusFlags::V, overflow);
    }

    ++cycles;
}

void CPU::ADC_indX()
{
    ++PC;

    uint8_t val = indexed_inderectX();

    ADC_base(val);
}

void CPU::ADC_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();

    ADC_base(val);
}

void CPU::ADC_zp()
{
    ++PC;

    uint8_t val = zero_page();

    ADC_base(val);
}

void CPU::ADC_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();

    ADC_base(val);
}

void CPU::ADC_imm()
{
    ++PC;

    uint8_t val = immediate();

    ADC_base(val);
}

void CPU::ADC_abs()
{
    ++PC;

    uint8_t val = absolute();

    ADC_base(val);
}

void CPU::ADC_absX()
{
    ++PC;

    uint8_t val = absoluteX();

    ADC_base(val);
}

void CPU::ADC_absY()
{
    ++PC;

    uint8_t val = absoluteY();

    ADC_base(val);
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
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ROR_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ROR_A()
{
    ++PC;

    ROR_base(A);
}

void CPU::ROR_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::ROR_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);
    val = ROR_base(val);

    bus->write(addr, val);
    ++cycles;
}

void CPU::PLA_impl()
{
    ++PC;

    A = bus->read(0x0100 + ++SP);

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 4;
}

void CPU::BVS_rel()
{
    ++PC;

    int8_t val = relative();

    if(get_flag(StatusFlags::V))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::SEI_impl()
{
    ++PC;

    set_flag(StatusFlags::I, true);

    cycles += 2;
}

void CPU::STA_indX()
{
    ++PC;

    uint16_t addr;
    indexed_inderectX(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_indY()
{
    ++PC;

    uint16_t addr;
    indexed_inderectY(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_zp()
{
    ++PC;

    uint16_t addr;
    zero_page(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_zpX()
{
    ++PC;

    uint16_t addr;
    zero_pageX(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_imm()
{
    ++PC;

    uint16_t addr;
    immediate(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_abs()
{
    ++PC;

    uint16_t addr;
    absolute(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_absX()
{
    ++PC;

    uint16_t addr;
    absoluteX(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STA_absY()
{
    ++PC;

    uint16_t addr;
    absoluteY(&addr);

    bus->write(addr, A);
    ++cycles;
}

void CPU::STY_zp()
{
    ++PC;

    uint16_t addr;
    zero_page(&addr);

    bus->write(addr, Y);
    ++cycles;
}

void CPU::STY_zpX()
{
    ++PC;

    uint16_t addr;
    zero_pageX(&addr);

    bus->write(addr, Y);
    ++cycles;
}

void CPU::STY_abs()
{
    ++PC;

    uint16_t addr;
    absolute(&addr);

    bus->write(addr, Y);
    ++cycles;
}

void CPU::STX_zp()
{
    ++PC;

    uint16_t addr;
    zero_page(&addr);

    bus->write(addr, X);
    ++cycles;
}

void CPU::STX_zpX()
{
    ++PC;

    uint16_t addr;
    zero_pageX(&addr);

    bus->write(addr, X);
    ++cycles;
}

void CPU::STX_abs()
{
    ++PC;

    uint16_t addr;
    absolute(&addr);

    bus->write(addr, X);
    ++cycles;
}

void CPU::DEY_impl()
{
    ++PC;
    --Y;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    cycles += 2;
}

void CPU::TXA_impl()
{
    ++PC;
    A = X;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 2;
}

void CPU::BCC_rel()
{
    ++PC;

    int8_t val = relative();

    if(!get_flag(StatusFlags::C))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::TYA_impl()
{
    ++PC;
    A = Y;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    cycles += 2;
}

void CPU::TXS_impl()
{
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
    ++PC;

    uint8_t val = zero_page();
    LDY_base(val);
}

void CPU::LDY_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();
    LDY_base(val);
}

void CPU::LDY_imm()
{
    ++PC;

    uint8_t val = immediate();
    LDY_base(val);
}

void CPU::LDY_abs()
{
    ++PC;

    uint8_t val = absolute();
    LDY_base(val);
}

void CPU::LDY_absX()
{
    ++PC;

    uint8_t val = absoluteX();
    LDY_base(val);
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
    ++PC;

    uint8_t val = indexed_inderectX();
    LDA_base(val);
}

void CPU::LDA_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();
    LDA_base(val);
}

void CPU::LDA_zp()
{
    ++PC;

    uint8_t val = zero_page();
    LDA_base(val);
}

void CPU::LDA_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();
    LDA_base(val);
}

void CPU::LDA_imm()
{
    ++PC;

    uint8_t val = immediate();
    LDA_base(val);
}

void CPU::LDA_abs()
{
    ++PC;

    uint8_t val = absolute();
    LDA_base(val);
}

void CPU::LDA_absX()
{
    ++PC;

    uint8_t val = absoluteX();
    LDA_base(val);
}

void CPU::LDA_absY()
{
    ++PC;

    uint8_t val = absoluteY();
    LDA_base(val);
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
    ++PC;

    uint8_t val = zero_page();
    LDX_base(val);
}

void CPU::LDX_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();
    LDX_base(val);
}

void CPU::LDX_zpY()
{
    ++PC;

    uint8_t val = zero_pageY();
    LDX_base(val);
}

void CPU::LDX_imm()
{
    ++PC;

    uint8_t val = immediate();
    LDX_base(val);
}

void CPU::LDX_abs()
{
    ++PC;

    uint8_t val = absolute();
    LDX_base(val);
}

void CPU::LDX_absY()
{
    ++PC;

    uint8_t val = absoluteY();
    LDX_base(val);
}

void CPU::TAY_impl()
{
    ++PC;

    Y = A;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    cycles += 2;
}

void CPU::BCS_rel()
{
    ++PC;

    uint8_t val = relative();

    if(get_flag(StatusFlags::C))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::CLV_impl()
{
    ++PC;

    set_flag(StatusFlags::V, false);

    cycles += 2;
}

void CPU::TSX_impl()
{
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
    ++PC;

    uint8_t val = immediate();
    CPY_base(val);
}

void CPU::CPY_zp()
{
    ++PC;

    uint8_t val = zero_page();
    CPY_base(val);
}

void CPU::CPY_abs()
{
    ++PC;

    uint8_t val = absolute();
    CPY_base(val);
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
    ++PC;

    uint8_t val = indexed_inderectX();
    CMP_base(val);
}

void CPU::CMP_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();
    CMP_base(val);
}

void CPU::CMP_zp()
{
    ++PC;

    uint8_t val = zero_page();
    CMP_base(val);
}

void CPU::CMP_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();
    CMP_base(val);
}

void CPU::CMP_imm()
{
    ++PC;

    uint8_t val = immediate();
    CMP_base(val);
}

void CPU::CMP_abs()
{
    ++PC;

    uint8_t val = absolute();
    CMP_base(val);
}

void CPU::CMP_absX()
{
    ++PC;

    uint8_t val = absoluteX();
    CMP_base(val);
}

void CPU::CMP_absY()
{
    ++PC;

    uint8_t val = absoluteY();
    CMP_base(val);
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
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::DEC_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::DEC_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::DEC_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    val = DEC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::INY_impl()
{
    ++PC;
    --Y;

    set_flag(StatusFlags::Z, Y == 0);
    set_flag(StatusFlags::N, Y & 0x80);

    cycles += 2;
}

void CPU::DEX_impl()
{
    ++PC;
    --X;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::BNE_rel()
{
    ++PC;

    uint8_t val = relative();

    if(!get_flag(StatusFlags::Z))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::CLD_impl()
{
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
    ++PC;

    uint8_t val = immediate();

    CPX_base(val);
}

void CPU::CPX_zp()
{
    ++PC;

    uint8_t val = zero_page();

    CPX_base(val);
}

void CPU::CPX_abs()
{
    ++PC;

    uint8_t val = absolute();

    CPX_base(val);
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
}

void CPU::SBC_indX()
{
    ++PC;

    uint8_t val = indexed_inderectX();

    SBC_base(val);
}

void CPU::SBC_indY()
{
    ++PC;

    uint8_t val = indexed_inderectY();

    SBC_base(val);
}

void CPU::SBC_zp()
{
    ++PC;

    uint8_t val = zero_page();

    SBC_base(val);
}

void CPU::SBC_zpX()
{
    ++PC;

    uint8_t val = zero_pageX();

    SBC_base(val);
}

void CPU::SBC_imm()
{
    ++PC;

    uint8_t val = immediate();

    SBC_base(val);
}

void CPU::SBC_abs()
{
    ++PC;

    uint8_t val = absolute();

    SBC_base(val);
}

void CPU::SBC_absX()
{
    ++PC;

    uint8_t val = absoluteX();

    SBC_base(val);
}

void CPU::SBC_absY()
{
    ++PC;

    uint8_t val = absoluteY();

    SBC_base(val);
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
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::INC_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::INC_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::INC_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    val = INC_base(val);
    bus->write(addr, val);

    ++cycles;
}

void CPU::INX_impl()
{
    ++PC;
    ++X;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::BEQ_rel()
{
    ++PC;

    uint8_t val = relative();

    if(get_flag(StatusFlags::Z))
    {
        ++cycles;
        PC += val;

        if(val != 0)
            ++cycles;
    }

    ++cycles;
}

void CPU::SED_impl()
{
    ++PC;

    set_flag(StatusFlags::D, true);

    cycles += 2;
}

void CPU::TAX_impl()
{
    ++PC;

    X = A;

    set_flag(StatusFlags::Z, X == 0);
    set_flag(StatusFlags::N, X & 0x80);

    cycles += 2;
}

void CPU::NOP_impl()
{
    ++PC;

    cycles += 2;
}

void CPU::NOP_zp()
{
    ++PC;

    cycles += 3;
}

void CPU::NOP_zpX()
{
    ++PC;

    cycles += 4;
}

void CPU::NOP_abs()
{
    ++PC;

    cycles += 4;
}

void CPU::NOP_absX()
{
    ++PC;

    absoluteX();

    cycles += 4;
}

void CPU::NOP_imm()
{
    ++PC;

    cycles += 2;
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    SLO_base(val, addr);
}

void CPU::SLO_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    SLO_base(val, addr);
}

void CPU::SLO_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    SLO_base(val, addr);
}

void CPU::SLO_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    SLO_base(val, addr);
}

void CPU::SLO_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    SLO_base(val, addr);
}

void CPU::SLO_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    SLO_base(val, addr);
}

void CPU::SLO_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    SLO_base(val, addr);
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    RLA_base(val, addr);
}

void CPU::RLA_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    RLA_base(val, addr);
}

void CPU::RLA_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    RLA_base(val, addr);
}

void CPU::RLA_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    RLA_base(val, addr);
}

void CPU::RLA_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    RLA_base(val, addr);
}

void CPU::RLA_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    RLA_base(val, addr);
}

void CPU::RLA_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    RLA_base(val, addr);
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    SRE_base(val, addr);
}

void CPU::SRE_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    SRE_base(val, addr);
}

void CPU::SRE_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    SRE_base(val, addr);
}

void CPU::SRE_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    SRE_base(val, addr);
}

void CPU::SRE_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    SRE_base(val, addr);
}

void CPU::SRE_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    SRE_base(val, addr);
}

void CPU::SRE_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    SRE_base(val, addr);
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    RRA_base(val, addr);
}

void CPU::RRA_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    RRA_base(val, addr);
}

void CPU::RRA_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    RRA_base(val, addr);
}

void CPU::RRA_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    RRA_base(val, addr);
}

void CPU::RRA_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    RRA_base(val, addr);
}

void CPU::RRA_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    RRA_base(val, addr);
}

void CPU::RRA_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    RRA_base(val, addr);
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    DCP_base(val, addr);
}

void CPU::DCP_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    DCP_base(val, addr);
}

void CPU::DCP_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    DCP_base(val, addr);
}

void CPU::DCP_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    DCP_base(val, addr);
}

void CPU::DCP_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    DCP_base(val, addr);
}

void CPU::DCP_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    DCP_base(val, addr);
}

void CPU::DCP_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    DCP_base(val, addr);
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    ISC_base(val, addr);
}

void CPU::ISC_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    ISC_base(val, addr);
}

void CPU::ISC_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    ISC_base(val, addr);
}

void CPU::ISC_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    ISC_base(val, addr);
}

void CPU::ISC_zpX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageX(&addr);

    ISC_base(val, addr);
}

void CPU::ISC_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    ISC_base(val, addr);
}

void CPU::ISC_absX()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteX(&addr);

    ISC_base(val, addr);
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
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectX(&addr);

    LAX_base(val);
}

void CPU::LAX_zp()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_page(&addr);

    LAX_base(val);
}

void CPU::LAX_abs()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absolute(&addr);

    LAX_base(val);
}

void CPU::LAX_indY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = indexed_inderectY(&addr);

    LAX_base(val);
}

void CPU::LAX_absY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = absoluteY(&addr);

    LAX_base(val);
}

void CPU::LAX_zpY()
{
    ++PC;

    uint16_t addr;
    uint8_t val = zero_pageY(&addr);

    LAX_base(val);
}

void CPU::LAX_imm()
{
    ++PC;

    uint16_t addr;
    uint8_t val = immediate(&addr);

    LAX_base(val);
}

void CPU::SAX_base(uint16_t addr)
{
    uint8_t val = A & X;
    bus->write(addr, val);

    ++cycles;
}

void CPU::SAX_indX()
{
    ++PC;

    uint16_t addr;
    indexed_inderectX(&addr);

    SAX_base(addr);
}

void CPU::SAX_zp()
{
    ++PC;

    uint16_t addr;
    zero_page(&addr);

    SAX_base(addr);
}

void CPU::SAX_abs()
{
    ++PC;

    uint16_t addr;
    absolute(&addr);

    SAX_base(addr);
}

void CPU::SAX_zpY()
{
    ++PC;

    uint16_t addr;
    zero_pageY(&addr);

    SAX_base(addr);
}

void CPU::SAX_imm()
{
    ++PC;

    uint16_t addr;
    immediate(&addr);

    SAX_base(addr);
}

void CPU::ANC_imm()
{
    ++PC;

    uint8_t val = immediate();

    A = A & val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);
    set_flag(StatusFlags::C, A & 0x80);

    ++cycles;
}

void CPU::ALR_imm()
{
    ++PC;

    uint8_t val = immediate();

    A = A & val;

    set_flag(StatusFlags::C, A & 0x01);

    A = A >> 1;
    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, false);

    ++cycles;
}

void CPU::ARR_imm()
{
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
}

void CPU::XAA_imm()
{
    ++PC;

    uint8_t val = immediate();

    A = A & val;

    set_flag(StatusFlags::Z, A == 0);
    set_flag(StatusFlags::N, A & 0x80);

    ++cycles;
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
    ++PC;

    uint16_t addr;
    indexed_inderectY(&addr);

    AHX_base(addr);
}

void CPU::AHX_absY()
{
    ++PC;

    uint16_t addr;
    absoluteY(&addr);

    AHX_base(addr);
}

void CPU::SHY_absX()
{
    ++PC;

    uint16_t addr;
    absoluteX(&addr);

    uint16_t eff = (uint16_t)(addr + X);
    uint8_t highp1 = (uint8_t)((eff >> 8) + 1);
    uint8_t val = Y & highp1;
    bus->write(eff, val);

    cycles += 2;
}

void CPU::TAS_absY()
{
    ++PC;

    uint16_t addr;
    absoluteY(&addr);

    uint16_t eff = addr + Y;
    uint8_t highp1 = (uint8_t)((eff >> 8) + 1);
    uint8_t val = SP & highp1;
    bus->write(eff, val);

    cycles += 2;
}

void CPU::SHX_absY()
{
    ++PC;

    uint16_t base;
    absoluteY(&base);

    uint16_t addr = base + Y;
    uint8_t highp1 = (uint8_t)((addr >> 8) + 1);
    uint8_t val = X & highp1;
    bus->write(addr, val);

    cycles += 2;
}

void CPU::LAS_absY()
{
    ++PC;

    uint8_t val = absoluteY();

    uint8_t tmp = val & SP;
    A  = tmp;
    X  = tmp;
    SP = tmp;

    set_flag(StatusFlags::Z, tmp == 0);
    set_flag(StatusFlags::N, tmp & 0x80);

    ++cycles;
}

void CPU::KIL_imp()
{
    QMessageBox message(QMessageBox::Icon::Critical, "Error", "Ошибка эмуляции KIL", QMessageBox::StandardButton::Ok);
    message.exec();

    {
        std::lock_guard<std::mutex> lock(mutex_stop);
        start = false;
    }

    if(run_t.joinable())
        run_t.join();
}

