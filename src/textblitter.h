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

    struct ColorChar
    {
            uint8_t m_char;
            uint8_t m_foreground;
            uint8_t m_background;
            uint8_t m_flags;

            ColorChar ()
                : m_char(0)
                , m_flags(m_dirtyFlag)
                , m_foreground(1)
                , m_background(0)
            {}
    };

    int writeText(QImage &target, int x, int y, ColorChar* str, int length);

    int lineSpacing() const;

    void setPaletteEntry(uint8_t index, QRgb const& value);

private:
    QRgb m_palette[256];

};

#endif // TEXTBLITTER_H
