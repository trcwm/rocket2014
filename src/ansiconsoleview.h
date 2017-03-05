/*

    ANSI Console view based console view
    N.A. Moseley 2017

    License: GPLv2.

*/

#ifndef ansiconsoleview_h
#define ansiconsoleview_h

#include <vector>
#include <string>
#include "consoleview.h"

class ANSIConsoleView : public ConsoleView
{
    Q_OBJECT

public:
    ANSIConsoleView(QWidget *parent = 0, uint32_t rows = 50, uint32_t columns = 80);
    virtual ~ANSIConsoleView() {}

    /** submit a character to the console.
        this function is multi-threading safe. */
    virtual void submitByte(const uint8_t c) override;

    /** enable ANSI mode */
    void enableANSIMode(bool enabled=true);

    /** reset the terminal to standard colours
        and clear the screen */
    void reset();

protected:
    void gotoNextState(const uint8_t byte);

    /** returns true if the byte represents an ASCII digit */
    bool isDigit(uint8_t byte)
    {
        return (byte >= '0') && (byte <= '9');
    }

    void parse(std::string &s);

    bool    m_ANSIEnabled;
    uint8_t m_state;                        // current state for state machine
    std::string m_ansistring;
};

#endif
