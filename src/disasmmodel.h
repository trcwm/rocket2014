/*

    Disassembly view
    N.A. Moseley 2016

    License: GPLv2.

*/

#ifndef disasmmodel_h
#define disasmmodel_h

#include <QString>
#include <stdint.h>

/** The disassembly model provides an interface
    to supply disassembly data to the view.

    For increased speed, one should consider
    caching data here.
    */
class DisasmModel
{
public:

    /** get the disassembly of instructions at 'address'.
        the disassembly is returned in 'txt'.
        the return value gives the number of bytes of the instructions.
       */
    virtual uint32_t getDisassembly(uint16_t address, QString &txt) = 0;
};

#endif
