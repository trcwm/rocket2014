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
    m_z80ClockRateMHz = 7000000; // 7 MHz
    m_z80System = new RC2014System(console);

    // do not reset here, or the CPU will run on bogus
    // memory contents.
    //reset();
}

void Z80SystemThread::setClockRate(uint32_t MHz)
{
    // we must protect the queue using
    // a mutex because this function
    // is called from the GUI thread
    QMutexLocker locker(&m_queueMutex);

    m_z80ClockRateMHz = MHz; // 7 MHz
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

    m_elapsedTimer.start();
    while (!m_quit)
    {
        m_elapsedTimer.restart();

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
            // lock the control mutex so
            // the GUI thread won't interfere.
            m_ctrlMutex.lock();
            uint32_t Tstates = m_z80System->execute(10000);
            m_ctrlMutex.unlock();

            // calculate the time an actual Z80 would take
            // to run the 1000 instructions
            uint32_t z80time_ms = Tstates*1000/m_z80ClockRateMHz;

            // actual time elapsed in milliseconds
            quint64 elapsedTime = m_elapsedTimer.elapsed(); // milliseconds

            // calculate actual remaining time to delay
            if (z80time_ms > elapsedTime)
            {
                msleep(z80time_ms - elapsedTime);
            }
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

