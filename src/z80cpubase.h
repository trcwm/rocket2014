/*


*/

#ifndef Z80CPUBase_h
#define Z80CPUBase_h

#include <stdint.h>
#include <vector>
#include "disasmmodel.h"

extern "C"
{
  #include "z80ex.h"
  #include "z80ex_dasm.h"
}

/** CPU base class providing Z80 emulation */
class Z80CPUBase : public DisasmModel
{
public:
    Z80CPUBase();

    /** register definitions for getRegister() */
    enum reg_t {REG_A, REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_IX, REG_IY, REG_SP, REG_PC, REG_HL, REG_DE, REG_BC};

    /** get the value of a z80 register */
    virtual uint16_t getRegister(const reg_t regID);

    /** get disassembly starting from a certain address */
    virtual uint32_t getDisassembly(uint16_t address, QString &txt);

    virtual void reset();
    virtual void interrupt();

    /** execute a set number of instructions,
        returning the number of Z80 clock cycles
        that it took to execute them */
    virtual uint32_t execute(uint32_t instructions);

    void halt(bool state=true)
    {
        m_isHalted = state;
    }

    bool isHalted() const
    {
        return m_isHalted;
    }

    void setBreakpoint(int32_t address)
    {
        m_brkAddress = address;
    }

protected:
    /* static functions used by underlying Z80 C library */
    static uint8_t  ex_readMemory(Z80EX_CONTEXT *ctx, uint16_t address, int m1_state, void *userdata);
    static uint8_t  ex_readMemory(uint16_t address, void *userdata); // for disasm
    static void     ex_writeMemory(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata);
    static uint8_t  ex_readIO(Z80EX_CONTEXT *ctx, uint16_t address, void *userdata);
    static void     ex_writeIO(Z80EX_CONTEXT *ctx, uint16_t address, uint8_t data, void *userdata);
    static uint8_t  ex_int(Z80EX_CONTEXT *ctx, void *userdata);

    Z80EX_CONTEXT *m_context;

    /* provide functions to read/write memory and IO space
       must be overridden by user of this base class */

    virtual uint8_t readMemory(uint16_t address, int m1_state = -1) = 0;
    virtual uint16_t readMemory16(uint16_t address) = 0;
    virtual void    writeMemory(uint16_t address, uint8_t data) = 0;
    virtual uint8_t readIO(uint16_t address) = 0;
    virtual void    writeIO(uint16_t address, uint8_t data) = 0;

    int32_t m_brkAddress;   // != -1 if a breakpoint has been set
    bool    m_isHalted;
};


#endif
