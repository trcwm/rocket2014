/*

    Textblitter
    T.J. Hofmeijer 2016

    License: GPLv2.

*/

#ifndef TEXTBLITTER_H
#define TEXTBLITTER_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <stdint.h>

class TextBlitter : public QObject
{
public:
    TextBlitter();
    virtual ~TextBlitter();

    static const uint8_t m_dirtyFlag = 1;

    /** console character with fore and background colors. */
    struct ColorChar
    {
            uint8_t m_char;
            uint8_t m_flags;
            uint8_t m_foreground;
            uint8_t m_background;

            ColorChar ()
                : m_char(0)
                , m_flags(m_dirtyFlag)
                , m_foreground(1)
                , m_background(0)
            {}
    };

    /** Blit text to a QImage at point (x,y).
        The text is supplied via a pointer to ColorChar array of size specified in 'length'.
        If cursorpos != -1, the character at index == cursorpos will be displayed inverted
        to simulate a cursor. Note: the m_dirtyFlag must be set for characters to be drawn.
    */
    int writeText(QImage &target, int x, int y, ColorChar* str, int length, int cursorpos);

    int lineSpacing() const;

    void setPaletteEntry(uint8_t index, QRgb const& value);

private:
    bool m_doubleSize;
    QRgb m_palette[256];

};

#endif // TEXTBLITTER_H
