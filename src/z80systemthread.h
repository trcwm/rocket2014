/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#ifndef Z80SYSTEMTHREAD_H
#define Z80SYSTEMTHREAD_H

#include <stdint.h>
#include <queue>
#include <QObject>
#include <QThread>
#include <QMutex>
#include "consoleview.h"
#include "Z80SystemBase.h"
#include "Z80SystemThread.h"

class Z80SystemThread : public QThread
{
    Q_OBJECT

public:
    explicit Z80SystemThread(ConsoleView *console, QObject *parent = 0);
    void quit() {m_quit = true;}

    /** send serial data to the Z80 system */
    void putSerialData(uint8_t c);

    /** load a file into the ROM */
    bool loadROM(const char *filename);

    /** reset the system */
    void reset();

    /** get Z80 register value */
    uint16_t getRegister(Z80SystemBase::reg_t regID);

    /** get full access to the Z80 subsystem */
    Z80SystemBase* getSystemPtr()
    {
        return m_z80;
    }

protected:
    virtual void run();

    std::queue<uint8_t> m_rxFIFO;
    QMutex          m_queueMutex;   // serial data mutex

    QMutex          m_dbgMutex;     // debug/reset mutex
    bool            m_quit;
    Z80SystemBase   *m_z80;
};

#endif
