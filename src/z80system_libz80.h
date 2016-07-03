/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#ifndef Z80SYSTEM_LIBZ80_H
#define Z80SYSTEM_LIBZ80_H

#include <stdint.h>
#include "z80systembase.h"
#include "consoleview.h"

extern "C"
{
  #include "z80.h"
}

class Z80System_libZ80 : public Z80SystemBase
{
public:
    explicit Z80System_libZ80(ConsoleView *console);

    virtual uint16_t getPC() const;

    virtual void reset();
    virtual void interrupt();
    virtual void execute(uint32_t instructions);

protected:

    static uint8_t  ctx_readMemory(void* param, uint16_t address);
    static uint8_t  ctx_readIO(void* param, uint16_t address);
    static void     ctx_writeMemory(void *param, uint16_t address, uint8_t data);
    static void     ctx_writeIO(void *param, uint16_t address, uint8_t data);

    Z80Context      m_ctx;
};

#endif
