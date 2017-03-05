/*

    Console view based on QImage
    N.A. Moseley 2016

    License: GPLv2.

*/

#include <QDebug>
#include <QPainter>
#include "consoleview.h"

static const unsigned int gs_XTermColours[256] =
{
    0x00000000,0x00000080,0x00008000,0x00008080,
    0x00800000,0x00800080,0x00808000,0x00C0C0C0,
    0x00808080,0x000000FF,0x0000FF00,0x0000FFFF,
    0x00FF0000,0x00FF00FF,0x00FFFF00,0x00FFFFFF,
    0x00000000,0x005F0000,0x00870000,0x00AF0000,
    0x00DF0000,0x00FF0000,0x00005F00,0x005F5F00,
    0x00875F00,0x00AF5F00,0x00DF5F00,0x00FF5F00,
    0x00008700,0x005F8700,0x00878700,0x00AF8700,
    0x00DF8700,0x00FF8700,0x0000AF00,0x005FAF00,
    0x0087AF00,0x00AFAF00,0x00DFAF00,0x00FFAF00,
    0x0000DF00,0x005FDF00,0x0087DF00,0x00AFDF00,
    0x00DFDF00,0x00FFDF00,0x0000FF00,0x005FFF00,
    0x0087FF00,0x00AFFF00,0x00DFFF00,0x00FFFF00,
    0x0000005F,0x005F005F,0x0087005F,0x00AF005F,
    0x00DF005F,0x00FF005F,0x00005F5F,0x005F5F5F,
    0x00875F5F,0x00AF5F5F,0x00DF5F5F,0x00FF5F5F,
    0x0000875F,0x005F875F,0x0087875F,0x00AF875F,
    0x00DF875F,0x00FF875F,0x0000AF5F,0x005FAF5F,
    0x0087AF5F,0x00AFAF5F,0x00DFAF5F,0x00FFAF5F,
    0x0000DF5F,0x005FDF5F,0x0087DF5F,0x00AFDF5F,
    0x00DFDF5F,0x00FFDF5F,0x0000FF5F,0x005FFF5F,
    0x0087FF5F,0x00AFFF5F,0x00DFFF5F,0x00FFFF5F,
    0x00000087,0x005F0087,0x00870087,0x00AF0087,
    0x00DF0087,0x00FF0087,0x00005F87,0x005F5F87,
    0x00875F87,0x00AF5F87,0x00DF5F87,0x00FF5F87,
    0x00008787,0x005F8787,0x00878787,0x00AF8787,
    0x00DF8787,0x00FF8787,0x0000AF87,0x005FAF87,
    0x0087AF87,0x00AFAF87,0x00DFAF87,0x00FFAF87,
    0x0000DF87,0x005FDF87,0x0087DF87,0x00AFDF87,
    0x00DFDF87,0x00FFDF87,0x0000FF87,0x005FFF87,
    0x0087FF87,0x00AFFF87,0x00DFFF87,0x00FFFF87,
    0x000000AF,0x005F00AF,0x008700AF,0x00AF00AF,
    0x00DF00AF,0x00FF00AF,0x00005FAF,0x005F5FAF,
    0x00875FAF,0x00AF5FAF,0x00DF5FAF,0x00FF5FAF,
    0x000087AF,0x005F87AF,0x008787AF,0x00AF87AF,
    0x00DF87AF,0x00FF87AF,0x0000AFAF,0x005FAFAF,
    0x0087AFAF,0x00AFAFAF,0x00DFAFAF,0x00FFAFAF,
    0x0000DFAF,0x005FDFAF,0x0087DFAF,0x00AFDFAF,
    0x00DFDFAF,0x00FFDFAF,0x0000FFAF,0x005FFFAF,
    0x0087FFAF,0x00AFFFAF,0x00DFFFAF,0x00FFFFAF,
    0x000000DF,0x005F00DF,0x008700DF,0x00AF00DF,
    0x00DF00DF,0x00FF00DF,0x00005FDF,0x005F5FDF,
    0x00875FDF,0x00AF5FDF,0x00DF5FDF,0x00FF5FDF,
    0x000087DF,0x005F87DF,0x008787DF,0x00AF87DF,
    0x00DF87DF,0x00FF87DF,0x0000AFDF,0x005FAFDF,
    0x0087AFDF,0x00AFAFDF,0x00DFAFDF,0x00FFAFDF,
    0x0000DFDF,0x005FDFDF,0x0087DFDF,0x00AFDFDF,
    0x00DFDFDF,0x00FFDFDF,0x0000FFDF,0x005FFFDF,
    0x0087FFDF,0x00AFFFDF,0x00DFFFDF,0x00FFFFDF,
    0x000000FF,0x005F00FF,0x008700FF,0x00AF00FF,
    0x00DF00FF,0x00FF00FF,0x00005FFF,0x005F5FFF,
    0x00875FFF,0x00AF5FFF,0x00DF5FFF,0x00FF5FFF,
    0x000087FF,0x005F87FF,0x008787FF,0x00AF87FF,
    0x00DF87FF,0x00FF87FF,0x0000AFFF,0x005FAFFF,
    0x0087AFFF,0x00AFAFFF,0x00DFAFFF,0x00FFAFFF,
    0x0000DFFF,0x005FDFFF,0x0087DFFF,0x00AFDFFF,
    0x00DFDFFF,0x00FFDFFF,0x0000FFFF,0x005FFFFF,
    0x0087FFFF,0x00AFFFFF,0x00DFFFFF,0x00FFFFFF,
    0x00080808,0x00121212,0x001C1C1C,0x00262626,
    0x00303030,0x003A3A3A,0x00444444,0x004E4E4E,
    0x00585858,0x00606060,0x00666666,0x00767676,
    0x00808080,0x008A8A8A,0x00949494,0x009E9E9E,
    0x00A8A8A8,0x00B2B2B2,0x00BCBCBC,0x00C6C6C6,
    0x00D0D0D0,0x00DADADA,0x00E4E4E4,0x00EEEEEE
};

ConsoleView::ConsoleView(QWidget *parent, uint32_t rows, uint32_t columns)
    : QWidget(parent)
    , m_image(0)
    , m_textBlitter(new TextBlitter())
    , m_lineCount(rows)
    , m_colCount(columns)
    , cx(0)
    , cy(0)
    , m_bgPalIdx(0)
    , m_fgPalIdx(2)
    , m_LFasCRLF(true)
{
    setFocusPolicy (Qt::StrongFocus);
    m_framebuffer.resize(m_lineCount * m_colCount);

    for(uint32_t i=0; i<256; i++)
    {
        uint32_t r = gs_XTermColours[i] & 0xff;
        uint32_t g = (gs_XTermColours[i] >> 8) & 0xff;
        uint32_t b = (gs_XTermColours[i] >> 16) & 0xff;
        uint32_t col = (r<<16) | (g<<8) | b;
        m_textBlitter->setPaletteEntry(i, col);
    }

    setAttribute(Qt::WA_OpaquePaintEvent);  // widget draws it's own background.

    m_refreshTimer = new QTimer(this);

    connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(onRefreshTimer()));
    m_refreshTimer->start(100);

    setFixedSize(8*m_colCount, m_textBlitter->lineSpacing()*m_lineCount);

    // store the thread this view was created, so
    // we know later on when to use a mutex
    m_creationThreadID = QThread::currentThreadId();
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
        m_framebuffer[x+(m_lineCount-1)*m_colCount].m_background = m_bgPalIdx;
        m_framebuffer[x+(m_lineCount-1)*m_colCount].m_foreground = m_fgPalIdx;
        m_framebuffer[x+(m_lineCount-1)*m_colCount].m_flags = TextBlitter::m_dirtyFlag;
    }
}

void ConsoleView::onRefreshTimer()
{
    QMutexLocker locker(&m_eventMutex);
    while (m_events.size() > 0)
    {
        event_t e = m_events.front();
        m_events.pop();

        TextBlitter::ColorChar cc;

        if (e.m_type == event_t::TYPE_CHAR)
        {
            switch(e.m_data)
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
                if (m_LFasCRLF)
                {
                    cx = 0;
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
                cc.m_char = e.m_data;
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
            } // switch
        }
        else if (e.m_type == event_t::TYPE_BG)
        {
            m_bgPalIdx = e.m_data;
        }
        else if (e.m_type == event_t::TYPE_FG)
        {
            m_fgPalIdx = e.m_data;
        }

    } // end while
    redraw();
}

void ConsoleView::submitByte(const uint8_t c)
{
    event_t evt = {event_t::TYPE_CHAR, c};
    if (QThread::currentThreadId() != m_creationThreadID)
    {
        // get mutex if not called from the GUI thread
        QMutexLocker locker(&m_eventMutex);
        m_events.push(evt);
    }
    else
    {
        m_events.push(evt);
    }
}

void ConsoleView::submitNewForegroundColour(const uint8_t palIdx)
{
    event_t evt = {event_t::TYPE_FG, palIdx};
    if (QThread::currentThreadId() != m_creationThreadID)
    {
        // get mutex if not called from the GUI thread
        QMutexLocker locker(&m_eventMutex);
        m_events.push(evt);
    }
    else
    {
        m_events.push(evt);
    }
}

void ConsoleView::submitNewBackgroundColour(const uint8_t palIdx)
{
    event_t evt = {event_t::TYPE_BG, palIdx};
    if (QThread::currentThreadId() != m_creationThreadID)
    {
        // get mutex if not called from the GUI thread
        QMutexLocker locker(&m_eventMutex);
        m_events.push(evt);
    }
    else
    {
        m_events.push(evt);
    }
}
