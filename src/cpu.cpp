#include "cpu.h"
#include "bus.h"
#include <QMessageBox>


CPU::CPU()
{
    bus.reset(new Bus());
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
        run_t = std::thread ([&]() { run(); });
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

       // table_instructions[val];
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

