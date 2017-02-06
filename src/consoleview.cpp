/*

    Console view based on QImage
    N.A. Moseley 2016

    License: GPLv2.

*/

#include <QDebug>
#include <QPainter>
#include "consoleview.h"

ConsoleView::ConsoleView(QWidget *parent, uint32_t rows, uint32_t columns)
    : QWidget(parent)
    , m_image(0)
    , m_textBlitter(new TextBlitter())
    , m_lineCount(rows)
    , m_colCount(columns)
    , m_bgcolor(0)
    , cx(0)
    , cy(0)
    , m_bgPalIdx(0)
    , m_fgPalIdx(1)
{
    setFocusPolicy (Qt::StrongFocus);
    m_framebuffer.resize(m_lineCount * m_colCount);

    m_textBlitter->setPaletteEntry(0, m_bgcolor);
    m_textBlitter->setPaletteEntry(1, 0x0000A000);

    for (uint32_t i = 0; i < m_framebuffer.size(); i++)
    {
        m_framebuffer[i].m_char = 0;
        m_framebuffer[i].m_flags = TextBlitter::m_dirtyFlag;
        m_framebuffer[i].m_foreground = 01;  // palette entries
        m_framebuffer[i].m_background = 00;  // palette entries
    }

    setAttribute(Qt::WA_OpaquePaintEvent);  // widget draws it's own background.

    m_refreshTimer = new QTimer(this);

    connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimer()));
    m_refreshTimer->start(100);

    setFixedSize(8*m_colCount, m_textBlitter->lineSpacing()*m_lineCount);
}

ConsoleView::~ConsoleView()
{
    delete m_image;
    delete m_textBlitter;
}

void ConsoleView::paintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter painter(this);

    if (m_image)
        painter.drawImage(0,0, *m_image);
}

void ConsoleView::resizeEvent(QResizeEvent *event)
{
    int w = event->size().width();
    int h = event->size().height();

    if (m_image != 0)
    {
        delete m_image;
    }

    m_image = new QImage(w,h, QImage::Format_RGB32);
    m_image->fill(m_bgcolor);

    // make everything dirty..
    for(uint32_t y=0; y<m_lineCount; y++)
    {
        for(uint32_t x=0; x<m_colCount; x++)
        {
            m_framebuffer[x+y*m_colCount].m_flags = TextBlitter::m_dirtyFlag;
        }
    }

    redraw();
}

void ConsoleView::redraw()
{
    if (m_image == 0)
        return;

    const uint32_t lineSpacing = m_textBlitter->lineSpacing();

    uint32_t visibleChars = m_image->width() / 8;
    uint32_t visibleRows  = m_image->height() / lineSpacing;

    visibleRows  = std::min(visibleRows, m_lineCount);
    visibleChars = std::min(visibleChars, m_colCount);

    for(uint32_t y=0; y<visibleRows; y++)
    {
        TextBlitter::ColorChar* line = &m_framebuffer[m_colCount*y];
        if (y == cy)
        {
            // cursor is on this row!
            m_framebuffer[m_colCount*y+cx].m_flags |= TextBlitter::m_dirtyFlag;
            m_textBlitter->writeText(*m_image, 0, y*lineSpacing, line, visibleChars, cx);
        }
        else
        {
            // normal row
            m_textBlitter->writeText(*m_image, 0, y*lineSpacing, line, visibleChars, -1);
        }
    }
    update();
}

void ConsoleView::scrollUp()
{
    TextBlitter::ColorChar* src = &m_framebuffer[m_colCount];
    TextBlitter::ColorChar* dst = &m_framebuffer[0];
    TextBlitter::ColorChar* end = &m_framebuffer[m_colCount*m_lineCount-1];

    end++;
    while(src != end)
    {
        *dst = *src++;
        dst->m_flags |= TextBlitter::m_dirtyFlag;
        dst++;
    }
    for(uint32_t x=0; x<m_colCount; x++)
    {
        m_framebuffer[x+(m_lineCount-1)*m_colCount].m_char = 0;
        m_framebuffer[x+(m_lineCount-1)*m_colCount].m_flags = TextBlitter::m_dirtyFlag;
    }
}

void ConsoleView::onRefreshTimer()
{
    QMutexLocker locker(&m_eventMutex);
    while (m_events.size() > 0)
    {
        char c = m_events.front();
        m_events.pop();

        TextBlitter::ColorChar cc;

        switch(c)
        {
        case 8:     // backspace
            if (cx > 0)
            {
                // mark old cursor position as dirty
                m_framebuffer[cx+cy*m_colCount].m_flags |= TextBlitter::m_dirtyFlag;
                cx--;
                cc.m_background = m_bgPalIdx;
                cc.m_foreground = m_fgPalIdx;
                cc.m_char = 0;
                cc.m_flags |= TextBlitter::m_dirtyFlag;
                m_framebuffer[cx+cy*m_colCount] = cc;
            }
            break;
        case 10:    // line feed
            // mark old cursor position as dirty
            m_framebuffer[cx+cy*m_colCount].m_flags |= TextBlitter::m_dirtyFlag;
            cy++;
            if (cy >= m_lineCount)
            {
                scrollUp();
                cy = m_lineCount-1;
            }
            break;
        case 12:    // clear screen / form feed
            cc.m_background = m_bgPalIdx;
            cc.m_foreground = m_fgPalIdx;
            cc.m_char = 0;
            cc.m_flags |= TextBlitter::m_dirtyFlag;
            m_framebuffer.assign(m_framebuffer.size(), cc);
            cx = 0;
            cy = 0;
            break;
        case 13:    // carriage return
            // mark old cursor position as dirty
            m_framebuffer[cx+cy*m_colCount].m_flags |= TextBlitter::m_dirtyFlag;
            // set cursor to beginning of line
            cx = 0;
            break;
        default:
            TextBlitter::ColorChar cc;
            cc.m_char = c;
            cc.m_background = m_bgPalIdx;
            cc.m_foreground = m_fgPalIdx;
            cc.m_flags |= TextBlitter::m_dirtyFlag;
            m_framebuffer[cx+cy*m_colCount] = cc;
            cx++;
            if (cx >= m_colCount)
            {
                cx = 0;
                cy++;
                if (cy >= m_lineCount)
                {
                    scrollUp();
                    cy = m_lineCount-1;
                }
            }
        }
    }
    redraw();
}

void ConsoleView::submitByte(uint8_t c)
{
    QMutexLocker locker(&m_eventMutex);
    m_events.push(c);
}
