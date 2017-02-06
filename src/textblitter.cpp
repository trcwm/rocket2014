/*

    Textblitter
    T.J. Hofmeijer 2016

    License: GPLv2.

*/

#include "textblitter.h"
#include <stdarg.h>

#define BIGFONT 1

#if BIGFONT
#include "font14.h"
#define FONT_HEIGHT 14
#else
#include "font.h"
#define FONT_HEIGHT 8
#endif

TextBlitter::TextBlitter()
{
    m_doubleSize = false;

    //preprocess the font data
    for (int i = 0; i < 256; i++)
    {
        m_palette[i] = qRgb(0, 0, 0);
    }
    m_palette[1] = qRgb(255, 255, 255);
}

TextBlitter::~TextBlitter()
{

}

int TextBlitter::lineSpacing() const
{
    return FONT_HEIGHT;
}


/*! \brief Writes text with color attributes to a QImage
  */
int TextBlitter::writeText (QImage &target, int x, int y, ColorChar *str, int length, int cursorpos)
{
    int l;
    if (m_doubleSize)
    {
        l= std::min(length, (target.width () - x) / 16);
    }
    else
    {
        l= std::min(length, (target.width () - x) / 8);
    }

    for (int i = 0; i < FONT_HEIGHT; i++)
    {
        if ((y + i) > target.height ())
            break;

        QRgb* pixels = (QRgb*) target.scanLine (y + i);
        int o = x;
        for (int j = 0; j < l; j++)
        {
            if ((str[j].m_flags & m_dirtyFlag) != 0)
            {
                QRgb fore,back;
                if (j == cursorpos)
                {
                    back = m_palette[str[j].m_foreground];
                    fore = m_palette[str[j].m_background];
                }
                else
                {
                    fore = m_palette[str[j].m_foreground];
                    back = m_palette[str[j].m_background];
                }

                uint8_t bitmap = g_fontData[(unsigned int)(str[j].m_char) * FONT_HEIGHT + i + 8];

                if (m_doubleSize)
                {
                    for(uint32_t p=0; p<8; p++)
                    {
                        pixels[o+2*p]   = bitmap & 0x80 ? fore : back;
                        pixels[o+2*p+1] = bitmap & 0x80 ? fore : back;
                        bitmap <<= 1;
                    }
                }
                else
                {
                    for(uint32_t p=0; p<8; p++)
                    {
                        pixels[o+p]   = bitmap & 0x80 ? fore : back;
                        bitmap <<= 1;
                    }
                }
            }
            if (m_doubleSize)
            {
                o += 16;
            }
            else
            {
                o += 8;
            }
        }
    }

    // clear dirty flags
    for (int j = 0; j < l; j++)
        str[j].m_flags &= ~m_dirtyFlag;

    return (l * 8);
}


/*! \brief Changes the value of a palette entry
  */
void TextBlitter::setPaletteEntry (uint8_t index, const QRgb &value)
{
    m_palette[index] = value;
}


