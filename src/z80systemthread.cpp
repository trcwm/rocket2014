/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#include <stdio.h>
#include <QMutexLocker>
#include "z80systemthread.h"

#include "z80system_libz80.h"
#include "z80system_z80ex.h"

Z80SystemThread::Z80SystemThread(ConsoleView *console, QObject *parent) :
    QThread(parent), m_quit(false)
{
#if 0
    m_z80 = new Z80System_libZ80(console);
#else
    m_z80 = new Z80System_Z80ex(console);
#endif
}

/** load a file into the ROM */
bool Z80SystemThread::loadROM(const char *filename)
{
    if (m_z80 != 0)
    {
        return m_z80->loadROM(filename);
    }
    return false;
}

void Z80SystemThread::reset()
{
    if (m_z80 != 0)
    {
        QMutexLocker locker(&m_dbgMutex);
        m_z80->reset();
    }
}

void Z80SystemThread::run()
{
    m_quit = false;

    if (m_z80 == 0)
    {
        return;
    }

    m_z80->reset();

    while (!m_quit)
    {
        // check for serial data in the queue
        m_queueMutex.lock();
        if (m_rxFIFO.size() != 0)
        {
            if (m_z80->putSerialData(m_rxFIFO.front()))
            {
                m_rxFIFO.pop();
            }
        }
        m_queueMutex.unlock();

        m_dbgMutex.lock();
        m_z80->execute(10000);
        m_dbgMutex.unlock();
    }
}

void Z80SystemThread::putSerialData(uint8_t c)
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);
    m_rxFIFO.push(c);
}

uint16_t Z80SystemThread::getRegister(Z80SystemBase::reg_t regID)
{
    QMutexLocker locker(&m_dbgMutex);
    return m_z80->getRegister(regID);
}

