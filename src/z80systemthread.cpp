/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#include <stdio.h>
#include <QMutexLocker>
#include "z80systemthread.h"

//#include "z80sysmills.h"
#include "z80system.h"

Z80SystemThread::Z80SystemThread(ConsoleView *console, QObject *parent) :
    QThread(parent), m_quit(false), m_z80System(0)
{
    m_z80System = new RC2014System(console);

    // do not reset here, or the CPU will run on bogus
    // memory contents.
    //reset();
}

/** load a file into the ROM */
bool Z80SystemThread::loadROM(const char *filename)
{
    if (m_z80System != 0)
    {
        return m_z80System->loadROM(filename);
    }
    return false;
}

void Z80SystemThread::reset()
{
    if (m_z80System != 0)
    {
        QMutexLocker locker(&m_ctrlMutex);
        m_z80System->reset();
    }
}

void Z80SystemThread::run()
{
    m_quit = false;

    if (m_z80System == 0)
    {
        return;
    }

    // don't reset here or the system will
    // run on bogus memory contents
    //m_z80System->reset();

    while (!m_quit)
    {
        // check for serial data in the queue
        m_queueMutex.lock();
        if (m_rxFIFO.size() != 0)
        {
            if (m_z80System->putSerialData(m_rxFIFO.front()))
            {
                m_rxFIFO.pop();
            }
        }
        m_queueMutex.unlock();

        if (!m_z80System->isHalted())
        {
            m_ctrlMutex.lock();
            m_z80System->execute(10000);
            m_ctrlMutex.unlock();
        }
        else
        {
            //relinquish resources to the OS :)
            msleep(1000);
        }
    }
}

void Z80SystemThread::setSystem(Z80SystemBase *system)
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);

    if (m_z80System != 0)
    {
        delete m_z80System;
    }
    m_z80System = system;
    system->reset();
}

void Z80SystemThread::setCPUState(bool running)
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);

    if (m_z80System == 0)
        return;

    m_z80System->halt(!running);
}

bool Z80SystemThread::isCPURunning()
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);

    if (m_z80System == 0)
        return false;

    return !m_z80System->isHalted();
}

void Z80SystemThread::setBreakpoint(int32_t address)
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);

    if (m_z80System == 0)
        return;

    m_z80System->setBreakpoint(address);
}

void Z80SystemThread::putSerialData(uint8_t c)
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);
    m_rxFIFO.push(c);
}

uint16_t Z80SystemThread::getRegister(RC2014System::reg_t regID)
{
    QMutexLocker locker(&m_ctrlMutex);
    return m_z80System->getRegister(regID);
}

