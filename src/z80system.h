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
#include "z80cpubase.h"
#include "consoleview.h"

class Z80System : public Z80CPUBase
{
public:
    explicit Z80System(ConsoleView *console);

    /** call this function to simulate serial data coming
        a device connected to the Z80System */
    bool putSerialData(uint8_t c);

    /** load a file into the ROM */
    bool loadROM(const char *filename);

    /** reset our custom hardware */
    virtual void reset()
    {
        m_MC6850_stat = MC6850_TXDATAEMPTY;
    }

protected:
    virtual uint8_t readMemory(uint16_t address) override;
    virtual uint16_t readMemory16(uint16_t address) override;
    virtual void    writeMemory(uint16_t address, uint8_t data) override;
    virtual uint8_t readIO(uint16_t address) override;
    virtual void    writeIO(uint16_t address, uint8_t data) override;

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
