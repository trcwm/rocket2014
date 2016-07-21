/*


*/

#ifndef z80system_z80ex_h
#define z80system_z80ex_h

#include <stdint.h>
#include "z80systembase.h"
#include "consoleview.h"

extern "C"
{
  #include "z80ex.h"
  #include "z80ex_dasm.h"
}

class Z80System_Z80ex : public Z80SystemBase
{
public:
    Z80System_Z80ex(ConsoleView *console);

    /** get the value of a z80 register */
    virtual uint16_t getRegister(Z80SystemBase::reg_t regID) const;

    /** get disassembly starting from a certain address */
    virtual bool getDisassembly(char *txtBufferPtr, size_t txtBufferSize, uint16_t address, uint32_t instructions);

    virtual void reset();
    virtual void interrupt();
    virtual void execute(uint32_t instructions);

protected:
    static uint8_t  ex_readMemory(Z80EX_CONTEXT *ctx, uint16_t address, int m1_state, void *userdata);
    static uint8_t ex_readMemory(uint16_t address, void *userdata); // for disasm
    static void     ex_writeMemory(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata);
    static uint8_t  ex_readIO(Z80EX_CONTEXT *ctx, uint16_t address, void *userdata);
    static void     ex_writeIO(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata);
    static uint8_t  ex_int(Z80EX_CONTEXT *ctx, void *userdata);

    Z80EX_CONTEXT *m_context;
};


#endif
