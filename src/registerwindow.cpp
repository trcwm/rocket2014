/*

    Register list view
    N.A. Moseley 2016

    License: GPLv2.

*/

#include "registerwindow.h"
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QHeaderView>
#include <QFontDatabase>
#include <QItemDelegate>

RegisterView::RegisterView(QWidget *parent)
    : QWidget(parent)
{
    m_registers = {"A","B","C","D","E","H","L","BC","DE","HL","IX","IY","SP","PC"};
    m_digits    = {2,2,2,2,2,2,2,4,4,4,4,4,4,4};
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    // calculate line size based on display font
    QFontMetrics metrics(font());
    m_lineHeight = metrics.height();
    m_lineWidth  = metrics.width(" XXXX XXXXX X");  // 1 extra char at the end!
    m_regYPos    = m_lineHeight*2;                  // skip one line for header
    m_regXPos    = metrics.width(" ");
    m_valueXPos  = metrics.width(" XXXX ");
    m_values.resize(m_registers.size());

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

RegisterView::~RegisterView()
{
}


QSize RegisterView::sizeHint() const
{
    return QSize(m_lineWidth,m_lineHeight*(m_registers.size()+2));
}


void RegisterView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(rect(),Qt::white);

    QRect headerRect(0,0, m_lineWidth, m_lineHeight);
    painter.fillRect(headerRect, Qt::black);
    painter.setPen(Qt::white);
    painter.drawText(headerRect, Qt::AlignCenter | Qt::AlignHCenter, "REGISTERS");
    painter.setPen(Qt::black);

    if (m_registers.size() != m_values.size())
        return;

    uint32_t ypos = m_regYPos;
    for(int i=0; i<m_registers.size(); i++)
    {
        QString hexvalue;
        hexvalue = QString("%1").arg(m_values[i], m_digits[i], 16, QChar('0'));
        painter.drawText(m_regXPos, ypos, m_registers[i]);
        painter.drawText(m_valueXPos, ypos, hexvalue);
        ypos += m_lineHeight;
    }

    //QWidget::paintEvent(event);
}

void RegisterView::setRegisterValue(uint32_t index, uint32_t value)
{
    if (index < m_values.size())
    {
        m_values[index] = value;
    }
}
