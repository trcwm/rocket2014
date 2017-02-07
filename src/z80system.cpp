/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#include <stdio.h>
#include <QDebug>
#include <QMutexLocker>
#include "z80system.h"


Z80System::Z80System(ConsoleView *console) :
    m_console(console)
{
    m_MC6850_stat = MC6850_TXDATAEMPTY;
}

/** load a file into the ROM */
bool Z80System::loadROM(const char *filename)
{
    // Load ROM
    FILE *fin = fopen(filename,"rb");
    if (fin == 0)
    {
        return false;
    }

    fseek(fin, 0, SEEK_END);
    size_t bytes = ftell(fin);
    if (bytes > 0xFFFF)
        return false;

    rewind(fin);
    fread(m_ROM, 1, bytes, fin);
    fclose(fin);

    reset();

    return true;
}

bool Z80System::putSerialData(uint8_t c)
{
    if (mc6850_RXEmpty())
    {
        mc6850_writeRX(c);
        return true;
    }
    return false;
}

/* Memory access functions */
uint8_t Z80System::readMemory(uint16_t address, int m1_state)
{
    // memory map:
    // 0x0000 .. 0x1FFF ROM (8k)
    // 0x8000 .. 0xFFFF RAM (32k)

#if 0
    if ((address == 0x015B) && (m1_state == 1))
    {
        // instruction fetch at address 0x163
        m_isHalted = true;
    }
#endif

    if (address <= 0x1FFF)
    {
        return m_ROM[address];
    }

    if (address >= 0x8000)
    {
        return m_RAM[address];
    }

    return 0xFF;    // return HALT instruction
}

uint16_t Z80System::readMemory16(uint16_t address)
{
    // memory map:
    // 0x0000 .. 0x1FFF ROM (8k)
    // 0x8000 .. 0xFFFF RAM (32k)

    uint16_t word = ((uint16_t)readMemory(address,0)) << 16;
    word |= ((uint16_t)readMemory(address+1,0));

    return word;
}

void Z80System::writeMemory(uint16_t address, uint8_t data)
{
    if (address >= 0x8000)
    {
        m_RAM[address] = data;
    }
}

/* IO space access functions */
uint8_t Z80System::readIO(uint16_t address)
{
    switch(address & 0xFF)
    {
    case 0x80:  // 68B50 control register
        return m_MC6850_stat;
    case 0x81:  // 68B50 RX data register
        return mc6850_readRX();
    default:
        return 0;
    }
}

void Z80System::writeIO(uint16_t address, uint8_t data)
{
    switch(address & 0xFF)
    {
    case 0x80:  // 68B50 control register
        m_MC6850_ctrl = data;
        //qDebug() << "68B50 write " << data;
        return;
    case 0x81:  // 68B50 TX data register
        if (m_console != 0)
        {
            m_console->submitByte(data);
        }
        return;
    default:
        return;
    }
}

uint8_t Z80System::mc6850_readRX()
{
    if (m_MC6850_stat & MC6850_RXDATAREADY)
    {
        m_MC6850_stat &= ~MC6850_RXDATAREADY;
        return m_MC6850_rx;
    }
    return 0xFF;
}

bool Z80System::mc6850_RXEmpty()
{
    return !(m_MC6850_stat & MC6850_RXDATAREADY);
}

void Z80System::mc6850_writeRX(uint8_t c)
{
    m_MC6850_stat |= MC6850_RXDATAREADY;
    m_MC6850_rx = c;

    // generate an interrupt if MC6850 is
    // so configured
    if (m_MC6850_ctrl & 0x80)
    {
        interrupt();
    }
}

