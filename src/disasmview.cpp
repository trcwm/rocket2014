/*

    Disassembly view
    N.A. Moseley 2016

    License: GPLv2.

*/

#include "disasmview.h"
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QHeaderView>
#include <QFontDatabase>
#include <QItemDelegate>

DisasmView::DisasmView(QWidget *parent)
    : QWidget(parent), m_model(0)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    // calculate line size based on display font
    QFontMetrics metrics(font());
    m_lineHeight    = metrics.height();
    m_lineWidth     = metrics.width(" XXXX  XXXXXXXXXXXXXXXXXXXX");
    m_disasmYPos    = m_lineHeight*2;                  // skip one line for header
    m_addressXPos   = metrics.width(" ");
    m_disasmXPos    = metrics.width(" XXXX  ");
    m_descent       = metrics.descent();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

DisasmView::~DisasmView()
{
}


QSize DisasmView::sizeHint() const
{
    return QSize(m_lineWidth,m_lineHeight*20);
}


void DisasmView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(rect(),Qt::white);

    QRect headerRect(0,0, m_lineWidth, m_lineHeight);
    painter.fillRect(headerRect, Qt::black);
    painter.setPen(Qt::white);
    painter.drawText(headerRect, Qt::AlignCenter | Qt::AlignHCenter, "DISASSEMBLY");
    painter.setPen(Qt::black);

    uint32_t i = 0;
    uint16_t m_address = m_startAddress;
    uint32_t ypos = m_disasmYPos;
    QString hexvalue;
    QString txt;
    while(i < m_instructions)
    {
        i++;
        if (m_model != 0)
        {
            hexvalue = QString("%1").arg(m_address, 4, 16, QChar('0'));
            uint16_t bytes = m_model->getDisassembly(m_address, txt);
            if (m_address == m_markAddress)
            {
                QRect markRect(0,ypos-m_lineHeight+m_descent, m_lineWidth, m_lineHeight);
                painter.fillRect(markRect, Qt::green);
                painter.drawText(m_addressXPos, ypos, hexvalue);
                painter.drawText(m_disasmXPos, ypos, txt);
            }
            else
            {
                painter.drawText(m_addressXPos, ypos, hexvalue);
                painter.drawText(m_disasmXPos, ypos, txt);
            }
            m_address += bytes;
            ypos += m_lineHeight;
        }
    }
}
