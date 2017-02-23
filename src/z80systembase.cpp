/*

    Z80 system base class
    N.A. Moseley 2016,2017

    License: GPLv2.

*/

#include <stdio.h>
#include <QDebug>
#include <QMutexLocker>
#include "z80systembase.h"


Z80SystemBase::Z80SystemBase(ConsoleView *console) :
    m_console(console)
{
}
