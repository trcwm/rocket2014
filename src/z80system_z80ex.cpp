
#include <QMutexLocker>
#include "z80system_z80ex.h"

Z80System_Z80ex::Z80System_Z80ex(ConsoleView *console)
    : Z80SystemBase(console)
{
    m_context = z80ex_create(
                ex_readMemory, this,
                ex_writeMemory, this,
                ex_readIO, this,
                ex_writeIO, this,
                ex_int, this
                );
}

void Z80System_Z80ex::reset()
{
    if (m_context == 0) return;

    z80ex_reset(m_context);
    Z80SystemBase::reset();
}

void Z80System_Z80ex::interrupt()
{
    if (m_context == 0) return;

    z80ex_int(m_context);
}

uint16_t Z80System_Z80ex::getRegister(Z80SystemBase::reg_t regID)
{
    if (m_context == 0) return 0;

    switch(regID)
    {
    case Z80SystemBase::REG_A:
        return z80ex_get_reg(m_context, regAF) >> 8;
    case Z80SystemBase::REG_B:
        return z80ex_get_reg(m_context, regBC) >> 8;
    case Z80SystemBase::REG_C:
        return z80ex_get_reg(m_context, regBC) & 0xFF;
    case Z80SystemBase::REG_D:
        return z80ex_get_reg(m_context, regDE) >> 8;
    case Z80SystemBase::REG_E:
        return z80ex_get_reg(m_context, regDE) & 0xFF;
    case Z80SystemBase::REG_H:
        return z80ex_get_reg(m_context, regHL) >> 8;
    case Z80SystemBase::REG_L:
        return z80ex_get_reg(m_context, regHL) & 0xFF;
    case Z80SystemBase::REG_IX:
        return z80ex_get_reg(m_context, regBC);
    case Z80SystemBase::REG_IY:
        return z80ex_get_reg(m_context, regDE);
    case Z80SystemBase::REG_BC:
        return z80ex_get_reg(m_context, regBC);
    case Z80SystemBase::REG_DE:
        return z80ex_get_reg(m_context, regDE);
    case Z80SystemBase::REG_HL:
        return z80ex_get_reg(m_context, regHL);
    case Z80SystemBase::REG_SP:
        return z80ex_get_reg(m_context, regSP);
    case Z80SystemBase::REG_PC:
        return z80ex_get_reg(m_context, regPC);
    default:
        return 0;
    }
}

uint32_t Z80System_Z80ex::getDisassembly(uint16_t address, QString &txt)
{
    char buffer[100];
    int dummy1, dummy2;

    if (m_context == 0) return false;

    uint32_t instrLen = z80ex_dasm(buffer, sizeof(buffer), 0, &dummy1, &dummy2, ex_readMemory, address, this);
    txt = QString(buffer);
    return instrLen;
}

void Z80System_Z80ex::execute(uint32_t instructions)
{
    if (m_context == 0) return;

    for(uint32_t i=0; i<instructions; i++)
        z80ex_step(m_context);
}

uint8_t Z80System_Z80ex::ex_readMemory(Z80EX_CONTEXT *ctx, uint16_t address, int m1_state, void *userdata)
{
    if (userdata != 0)
    {
        Z80System_Z80ex *obj = (Z80System_Z80ex*)userdata;
        return obj->readMemory(address);
    }
    return 0;
}

uint8_t Z80System_Z80ex::ex_readMemory(uint16_t address, void *userdata)
{
    if (userdata != 0)
    {
        Z80System_Z80ex *obj = (Z80System_Z80ex*)userdata;
        return obj->readMemory(address);
    }
    return 0;
}


uint8_t Z80System_Z80ex::ex_readIO(Z80EX_CONTEXT *ctx, uint16_t address, void *userdata)
{
    if (userdata != 0)
    {
        Z80System_Z80ex *obj = (Z80System_Z80ex*)userdata;
        return obj->readIO(address);
    }
    return 0;
}

void Z80System_Z80ex::ex_writeMemory(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata)
{
    if (userdata != 0)
    {
        Z80System_Z80ex *obj = (Z80System_Z80ex*)userdata;
        obj->writeMemory(address, data);
    }
}

void Z80System_Z80ex::ex_writeIO(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata)
{
    if (userdata != 0)
    {
        Z80System_Z80ex *obj = (Z80System_Z80ex*)userdata;
        obj->writeIO(address, data);
    }
}

uint8_t Z80System_Z80ex::ex_int(Z80EX_CONTEXT *ctx, void *userdata)
{
    return 0;
}
