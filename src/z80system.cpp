/*

    RC2014 / Z80 system thread
    N.A. Moseley 2016

    License: GPLv2.

*/

#include <stdio.h>
#include <QDebug>
#include <QMutexLocker>
#include "z80system.h"


RC2014System::RC2014System(ConsoleView *console, systemType_t systype)
    : Z80SystemBase(console),
      m_systype(systype)
{
    m_MC6850_stat = MC6850_TXDATAEMPTY;
    m_intPending  = false;
}

/** load a file into the ROM */
bool RC2014System::loadROM(const char *filename)
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
    uint32_t address=0;
    while(!feof(fin))
    {
        uint8_t data;
        fread(&data,1,1,fin);
        switch(m_systype)
        {
        case SYS_32KRAM:
            if (address < 8192)
            {
                m_ROM[address] = data;
            }
            if ((address >= 32768) && (address <= 65535))
            {
                m_RAM[address] = data;
            }
            break;
        case SYS_64KRAM:
            if (address <= 65535)
            {
                m_RAM[address] = data;
            }
            break;
        default:
            ;
        }
        address++;
    }
    fclose(fin);

    reset();

    return true;
}

bool RC2014System::putSerialData(uint8_t c)
{
    if (mc6850_RXEmpty())
    {
        mc6850_writeRX(c);
        return true;
    }
    return false;
}

/* Memory access functions */
uint8_t RC2014System::readMemory(uint16_t address, int m1_state)
{
    switch(m_systype)
    {
    case SYS_32KRAM:
        // memory map:
        // 0x0000 .. 0x1FFF ROM (8k)
        // 0x8000 .. 0xFFFF RAM (32k)

        if (address <= 0x1FFF)
        {
            return m_ROM[address];
        }

        if (address >= 0x8000)
        {
            return m_RAM[address];
        }
        break;
    case SYS_64KRAM:
        return m_RAM[address];
        break;
    default:
        ; // intentional
    }

    return 0xFF;    // return HALT instruction
}

uint16_t RC2014System::readMemory16(uint16_t address)
{
    uint16_t word = ((uint16_t)readMemory(address,0)) << 16;
    word |= ((uint16_t)readMemory(address+1,0));

    return word;
}

void RC2014System::writeMemory(uint16_t address, uint8_t data)
{
    switch(m_systype)
    {
    case SYS_32KRAM:
        // memory map:
        // 0x0000 .. 0x1FFF ROM (8k)
        // 0x8000 .. 0xFFFF RAM (32k)
        if (address >= 0x8000)
        {
            m_RAM[address] = data;
        }
        break;
    case SYS_64KRAM:
        m_RAM[address] = data;
        break;
    default:
        ; // intentional
    }
}

/* IO space access functions */
uint8_t RC2014System::readIO(uint16_t address)
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

void RC2014System::writeIO(uint16_t address, uint8_t data)
{
    switch(address & 0xFF)
    {
    case 0x80:  // 68B50 control register
        m_MC6850_ctrl = data;
        qDebug() << "68B50 write " << data;
        // cause interrupt if TX buffer is empty
        // and interrupt is enabled!
        if ((m_MC6850_ctrl & 0x60) == 0x20)
        {
            // TX interrupt enabled
            m_MC6850_stat |=MC6850_IRQ;  // set IRQ bit
            m_intPending = true;
        }
        return;
    case 0x81:  // 68B50 TX data register
        if (m_console != 0)
        {
            m_MC6850_stat &= ~MC6850_IRQ;        // clear IRQ bit
            m_console->submitByte(data);
            m_MC6850_stat |= MC6850_TXDATAEMPTY; // TX buffer empty!
            if ((m_MC6850_ctrl & 0x60) == 0x20)
            {
                // TX interrupt enabled
                m_MC6850_stat |= MC6850_IRQ;  // set IRQ bit
                m_intPending = true;
            }
        }
        return;
    default:
        return;
    }
}

uint8_t RC2014System::mc6850_readRX()
{
    if (m_MC6850_stat & MC6850_RXDATAREADY)
    {
        m_MC6850_stat &= ~MC6850_IRQ;
        m_MC6850_stat &= ~MC6850_RXDATAREADY;
        return m_MC6850_rx;
    }
    return 0xFF;
}

bool RC2014System::mc6850_RXEmpty()
{
    return !(m_MC6850_stat & MC6850_RXDATAREADY);
}

void RC2014System::mc6850_writeRX(uint8_t c)
{
    m_MC6850_stat |= MC6850_RXDATAREADY;
    m_MC6850_rx = c;

    // generate an interrupt if MC6850 is
    // so configured
    if (m_MC6850_ctrl & 0x80)
    {
        m_intPending = true;
    }
}

