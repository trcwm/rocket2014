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
#include <QElapsedTimer>

#include "consoleview.h"
#include "z80systembase.h"
#include "z80systemthread.h"

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

    /** control running of CPU */
    void setCPUState(bool running);

    /** get the run state of the CPU */
    bool isCPURunning();

    /** get Z80 register value */
    uint16_t getRegister(Z80SystemBase::reg_t regID);

    /** get full access to the Z80 subsystem */
    Z80SystemBase* getSystemPtr()
    {
        return m_z80System;
    }

    /** set the Z80 subsystem
        Note: the thread takes ownership of the object/pointer
    */
    void setSystem(Z80SystemBase *system);

    /** set the clock rate of the Z80 in MHz */
    void setClockRate(uint32_t MHz);

    /** set an instruction address breakpoint.
        set to -1 to disable.
    */
    void setBreakpoint(int32_t address);

protected:
    virtual void run();

    std::queue<uint8_t> m_rxFIFO;
    QMutex          m_queueMutex;       // serial data mutex

    QElapsedTimer   m_elapsedTimer;     // for CPU throttling

    QMutex          m_ctrlMutex;        // debug/reset/control mutex
    bool            m_quit;

    Z80SystemBase  *m_z80System;
    uint32_t        m_z80ClockRateMHz;  // the Z80 clock in MHz
};

#endif
