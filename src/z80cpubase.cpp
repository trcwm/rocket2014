
#include <QMutexLocker>
#include "z80cpubase.h"

Z80CPUBase::Z80CPUBase() : m_isHalted(true), m_intPending(false)
{
    m_context = z80ex_create(
                ex_readMemory, this,
                ex_writeMemory, this,
                ex_readIO, this,
                ex_writeIO, this,
                ex_int, this
                );    
}

void Z80CPUBase::reset()
{
    if (m_context == 0) return;

    z80ex_reset(m_context);
    m_isHalted = false;
    m_intPending=false;
}

void Z80CPUBase::interrupt()
{
    if (m_context == 0) return;
    m_intPending = true;
}

uint16_t Z80CPUBase::getRegister(const reg_t regID)
{
    if (m_context == 0) return 0;

    switch(regID)
    {
    case REG_A:
        return z80ex_get_reg(m_context, regAF) >> 8;
    case REG_B:
        return z80ex_get_reg(m_context, regBC) >> 8;
    case REG_C:
        return z80ex_get_reg(m_context, regBC) & 0xFF;
    case REG_D:
        return z80ex_get_reg(m_context, regDE) >> 8;
    case REG_E:
        return z80ex_get_reg(m_context, regDE) & 0xFF;
    case REG_H:
        return z80ex_get_reg(m_context, regHL) >> 8;
    case REG_L:
        return z80ex_get_reg(m_context, regHL) & 0xFF;
    case REG_IX:
        return z80ex_get_reg(m_context, regIX);
    case REG_IY:
        return z80ex_get_reg(m_context, regIY);
    case REG_BC:
        return z80ex_get_reg(m_context, regBC);
    case REG_DE:
        return z80ex_get_reg(m_context, regDE);
    case REG_HL:
        return z80ex_get_reg(m_context, regHL);
    case REG_SP:
        return z80ex_get_reg(m_context, regSP);
    case REG_PC:
        return z80ex_get_reg(m_context, regPC);
    default:
        return 0;
    }
}

uint32_t Z80CPUBase::getDisassembly(uint16_t address, QString &txt)
{
    char buffer[100];
    int dummy1, dummy2;

    if (m_context == 0) return false;

    uint32_t instrLen = z80ex_dasm(buffer, sizeof(buffer), 0, &dummy1, &dummy2, ex_readMemory, address, this);
    txt = QString(buffer);
    return instrLen;
}

uint32_t Z80CPUBase::execute(uint32_t instructions)
{
    if (m_context == 0) return 0;

    uint32_t Tstates = 0;
    while((instructions > 0) && (!m_isHalted))
    {
        if (m_intPending)
        {
            if (z80ex_int(m_context) != 0)
            {
                m_intPending = false;
            }
        }
        Tstates += z80ex_step(m_context);
        instructions--;
    }
    return Tstates;
}

uint8_t Z80CPUBase::ex_readMemory(Z80EX_CONTEXT *ctx, uint16_t address, int m1_state, void *userdata)
{
    if (userdata != 0)
    {        
        Z80CPUBase *obj = (Z80CPUBase*)userdata;

        // check for a breakpoint
        if (obj->getRegister(REG_PC) == obj->m_brkAddress)
        {
            obj->m_isHalted = true;
        }

        return obj->readMemory(address, m1_state);
    }
    return 0;
}

uint8_t Z80CPUBase::ex_readMemory(uint16_t address, void *userdata)
{
    if (userdata != 0)
    {
        Z80CPUBase *obj = (Z80CPUBase*)userdata;
        return obj->readMemory(address, 0);
    }
    return 0;
}


uint8_t Z80CPUBase::ex_readIO(Z80EX_CONTEXT *ctx, uint16_t address, void *userdata)
{
    if (userdata != 0)
    {
        Z80CPUBase *obj = (Z80CPUBase*)userdata;
        return obj->readIO(address);
    }
    return 0;
}

void Z80CPUBase::ex_writeMemory(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata)
{
    if (userdata != 0)
    {
        Z80CPUBase *obj = (Z80CPUBase*)userdata;
        obj->writeMemory(address, data);
    }
}

void Z80CPUBase::ex_writeIO(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata)
{
    if (userdata != 0)
    {
        Z80CPUBase *obj = (Z80CPUBase*)userdata;
        obj->writeIO(address, data);
    }
}

uint8_t Z80CPUBase::ex_int(Z80EX_CONTEXT *ctx, void *userdata)
{
    return 0;
}
