
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

uint16_t Z80System_Z80ex::getPC() const
{
    if (m_context == 0) return 0;

    return z80ex_get_reg(m_context, regPC);
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
