/*

    Z80 system base class
    N.A. Moseley 2016,2017

    License: GPLv2.

*/

#ifndef Z80SYSTEMBASE_H
#define Z80SYSTEMBASE_H

#include <stdint.h>
#include <queue>
#include <QObject>
#include <QThread>
#include <QMutex>
#include "z80cpubase.h"
#include "consoleview.h"

/** Z80 system base class.
    A pointer to a console is used to output
    data to the terminal. This pointer can be
    NULL; output is then disregarded.
*/

class Z80SystemBase : public Z80CPUBase
{
public:
    explicit Z80SystemBase(ConsoleView *console = NULL);

    /** call this function to simulate serial data coming
        from a device connected to the Z80System */
    virtual bool putSerialData(uint8_t c) = 0;

    /** load a file into the ROM */
    virtual bool loadROM(const char *filename) = 0;

    /** reset our custom hardware */
    virtual void reset()  = 0;

protected:
    virtual uint8_t readMemory(uint16_t address, int m1_state) override = 0;
    virtual uint16_t readMemory16(uint16_t address) override = 0;
    virtual void    writeMemory(uint16_t address, uint8_t data) override = 0;
    virtual uint8_t readIO(uint16_t address) override = 0;
    virtual void    writeIO(uint16_t address, uint8_t data) override = 0;

    ConsoleView *m_console; // console to send serial data to.
};

#endif
