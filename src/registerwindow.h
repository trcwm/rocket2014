/*

    Register list view
    N.A. Moseley 2016

    License: GPLv2.

*/

#ifndef registerwindow_h
#define registerwindow_h

#include <stdint.h>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QStaticText>
#include <QWidget>

class RegisterView : public QWidget
{
    Q_OBJECT
public:
    RegisterView(QWidget *parent = 0);
    ~RegisterView();

    void setRegisterName(uint32_t index, const QString &name);
    void setRegisterValue(uint32_t index, uint32_t value);
    void setLine(uint32_t index, const QString &txt);

    QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent * event);

    uint32_t    m_lineHeight;   // line height in pixels
    uint32_t    m_lineWidth;    // maximum line with in pixels
    uint32_t    m_valueXPos;    // xposition of register value
    uint32_t    m_regXPos;      // xposition of register name
    uint32_t    m_regYPos;      // yposition to start displaying registers
    uint32_t    m_textDescent;  // number of pixels below the baseline

    QVector<QString> m_registers;   // names of the register
    QVector<uint32_t> m_values;     // register values
    QVector<uint32_t> m_digits;     // number of hex digits in register
};


#endif
