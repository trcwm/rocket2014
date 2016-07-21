/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#ifndef Z80SYSTEMBASE_H
#define Z80SYSTEMBASE_H

#define MC6850_TXDATAEMPTY 0x02
#define MC6850_RXDATAREADY 0x01
#define MC6850_DCD_N 0x04
#define MC6850_CTS_N 0x08

#include <stdint.h>
#include <queue>
#include <QObject>
#include <QThread>
#include <QMutex>
#include "consoleview.h"

class Z80SystemBase
{
public:
    explicit Z80SystemBase(ConsoleView *console);

    bool putSerialData(uint8_t c);  // called from GUI thread to

    /** load a file into the ROM */
    bool loadROM(const char *filename);

    enum reg_t {REG_A, REG_B, REG_C, REG_D, REG_E, REG_H, REG_L, REG_IX, REG_IY, REG_SP, REG_PC, REG_HL, REG_DE, REG_BC};

    /** get a register value */
    virtual uint16_t getRegister(reg_t regID) const = 0;

    virtual void execute(uint32_t instructions) = 0;
    virtual void interrupt() = 0;
    virtual void reset()
    {
        m_MC6850_stat = MC6850_TXDATAEMPTY;
    }

protected:
    uint8_t readMemory(uint16_t address);
    void    writeMemory(uint16_t address, uint8_t data);
    uint8_t readIO(uint16_t address);
    void    writeIO(uint16_t address, uint8_t data);

    // memories
    uint8_t     m_ROM[65536];
    uint8_t     m_RAM[65536];

    // MC6850 peripheral
    bool        mc6850_RXEmpty();
    void        mc6850_writeRX(uint8_t c);
    uint8_t     mc6850_readRX();
    uint8_t     m_MC6850_stat;
    uint8_t     m_MC6850_ctrl;
    uint8_t     m_MC6850_rx;

    ConsoleView *m_console; // console to receive serial data
};

#endif
