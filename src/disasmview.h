/*

    Disassembly view
    N.A. Moseley 2016

    License: GPLv2.

*/

#ifndef disasmview_h
#define disasmview_h

#include "disasmmodel.h"

#include <stdint.h>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QWidget>


/** A view for the disassembly */
class DisasmView : public QWidget
{
    Q_OBJECT
public:
    DisasmView(QWidget *parent = 0);
    ~DisasmView();

    void setModel(DisasmModel *model)
    {
        m_model = model;
    }

    void setViewParameters(uint16_t startAddress, uint16_t markAddress, uint32_t instructions)
    {
        m_startAddress = startAddress;
        m_markAddress  = markAddress;
        m_instructions = instructions;
        update();
    }

    QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent * event);

    DisasmModel *m_model;       // the model supplying the disassmbly data

    uint32_t    m_lineHeight;   // line height in pixels
    uint32_t    m_lineWidth;    // maximum line with in pixels
    uint32_t    m_addressXPos;  // xposition of address
    uint32_t    m_disasmXPos;   // xposition of disassembly
    uint32_t    m_disasmYPos;      // yposition to start displaying disassembly
    uint32_t    m_descent;

    uint16_t    m_startAddress; // address at the top of the display
    uint16_t    m_markAddress;  // the current address
    uint16_t    m_instructions; // the number of instructions to display
};


#endif
