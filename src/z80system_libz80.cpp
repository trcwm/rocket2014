
#include "z80system_libz80.h"


Z80System_libZ80::Z80System_libZ80(ConsoleView *console)
    : Z80SystemBase(console)
{
    memset(&m_ctx, 0, sizeof(m_ctx));
    m_ctx.ioParam = this;
    m_ctx.ioRead  = ctx_readIO;
    m_ctx.ioWrite = ctx_writeIO;
    m_ctx.memParam = this;
    m_ctx.memRead  = ctx_readMemory;
    m_ctx.memWrite = ctx_writeMemory;
}

void Z80System_libZ80::reset()
{
    Z80RESET(&m_ctx);
    Z80SystemBase::reset();
}

void Z80System_libZ80::interrupt()
{
    Z80INT(&m_ctx, 0);
}

uint16_t Z80System_libZ80::getPC() const
{
    return m_ctx.PC;
}

void Z80System_libZ80::execute(uint32_t instructions)
{
    // FIXME: T-states are not instructions ..
    Z80ExecuteTStates(&m_ctx, instructions);
}

uint8_t Z80System_libZ80::ctx_readMemory(void* param, unsigned short address)
{
    if (param != 0)
    {
        Z80System_libZ80 *obj = (Z80System_libZ80*)param;
        return obj->readMemory(address);
    }
    return 0;
}

uint8_t Z80System_libZ80::ctx_readIO(void* param, unsigned short address)
{
    if (param != 0)
    {
        Z80System_libZ80 *obj = (Z80System_libZ80*)param;
        return obj->readIO(address);
    }
    return 0;
}

void Z80System_libZ80::ctx_writeMemory(void *param, unsigned short address, unsigned char data)
{
    if (param != 0)
    {
        Z80System_libZ80 *obj = (Z80System_libZ80*)param;
        obj->writeMemory(address, data);
    }
}

void Z80System_libZ80::ctx_writeIO(void *param, unsigned short address, unsigned char data)
{
    if (param != 0)
    {
        Z80System_libZ80 *obj = (Z80System_libZ80*)param;
        obj->writeIO(address, data);
    }
}

