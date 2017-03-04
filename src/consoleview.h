/*

    Console view based on QImage
    N.A. Moseley, T.J. Hofmeijer 2016

    License: GPLv2.

*/

#ifndef consoleview_h
#define consoleview_h

#include <QWidget>
#include <QImage>
#include <QKeyEvent>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <queue>
#include <vector>
#include "textblitter.h"

/** Console view with multi-threading support.
    A timer is used to refresh the console display.

    The console supports backspace, linefeed,
    clear screen/form feed, carriage return.

*/
class ConsoleView : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleView(QWidget *parent = 0, uint32_t rows = 50, uint32_t columns = 80);
    ~ConsoleView();

    /** submit a character to the console.
        this function is multi-threading safe. */
    virtual void submitByte(const uint8_t c);

    /** set a new foreground colour */
    virtual void submitNewForegroundColour(const uint8_t palIdx);

    /** set a new background colour */
    virtual void submitNewBackgroundColour(const uint8_t palIdx);

private:
    /** scroll the display up */
    void scrollUp();

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    void redraw();

    QImage      *m_image;
    TextBlitter *m_textBlitter;
    uint32_t    m_lineCount;
    uint32_t    m_colCount;

    uint32_t    cx; // cursor position x
    uint32_t    cy; // cursor position y

    uint32_t    m_bgcolor;
    uint32_t    m_bgPalIdx;
    uint32_t    m_fgPalIdx;

    QTimer      *m_refreshTimer;

    std::vector<TextBlitter::ColorChar> m_framebuffer;

    QMutex      m_eventMutex;

    /** event description */
    struct event_t
    {
        enum {TYPE_CHAR, TYPE_FG, TYPE_BG} m_type;
        uint8_t m_data; // character or palette number
    };

    std::queue<event_t> m_events;

    Qt::HANDLE  m_creationThreadID;

private slots:
    void onRefreshTimer();
};

#endif
