/*

    ANSI Console view based console view
    N.A. Moseley 2017

    License: GPLv2.

*/

#include <QDebug>
#include <sstream>
#include <iostream>
#include "ansiconsoleview.h"

#define STATE_START     0
#define STATE_ESCAPE    1
#define STATE_BRACKET   2

ANSIConsoleView::ANSIConsoleView(QWidget *parent, uint32_t rows, uint32_t columns)
    : ConsoleView(parent, rows, columns)
{
    reset();
}

void ANSIConsoleView::submitByte(const uint8_t c)
{
    gotoNextState(c);
}

void ANSIConsoleView::enableANSIMode(bool enabled)
{
    m_ANSIEnabled = enabled;
}

void ANSIConsoleView::reset()
{
    m_ANSIEnabled = true;
    m_state = STATE_START;
    submitNewBackgroundColour(0);   // black
    submitNewForegroundColour(2);   // green
    submitByte(12);                 // form-feed / clear screen
}

void ANSIConsoleView::gotoNextState(const uint8_t byte)
{
    // if ansi mode is not enabled, we
    // simply ignore everything and
    // send the raw data directly to
    // the underlying console view
    if (!m_ANSIEnabled)
    {
        m_state = 0;
        ConsoleView::submitByte(byte);
        return;
    }

    // state machine to handle the decoding
    // and processing of the ANSI codes
    switch(m_state)
    {
    case STATE_START: // start state
        if (byte == 27) // escape
        {
            // found the start of the escape sequence
            m_state = STATE_ESCAPE;
        }
        else
        {
            // send non-escape characters to the console
            ConsoleView::submitByte(byte);
        }
        break;
    case STATE_ESCAPE:
        // here detect which type of escape sequence it is.
        // if we don't recognize it, we send everything
        // to the console, including the previous
        // escape character, mainly for debugging
        // and finding out about things we don't
        // (yet) handle properly.

        if (byte == '[')
        {
            m_state = STATE_BRACKET;
            m_ansistring.clear();
        }
        else
        {
            // send everything unhandled to the
            // console
            m_state = STATE_START;
            ConsoleView::submitByte(27);
            ConsoleView::submitByte(byte);
        }
        break;
    case STATE_BRACKET:
        // found <ESC>[
        //
        m_ansistring += byte;
        if ((byte>=64) && (byte<=126))
        {
            // found the end-of-command terminator
            // so now we can parse it
            parse(m_ansistring);
            m_state = STATE_START;
        }
    default:
        break;
    }
    return;
}

void ANSIConsoleView::parse(std::string &s)
{
    std::vector<std::string> strings;

    // check the last character to see which command
    // it is.
    uint8_t last = s.back();
    s.pop_back();
    std::istringstream istream(s);

    std::string param;
    int typecmd;
    switch(last)
    {
    case 'm':   // set SGR parameters
        while(getline(istream, param, ';'))
        {
            strings.push_back(param);
        }
        // check if we have parameters,
        // if not, something went wrong
        // we can't really do anything useful
        // so let's ignore it.
        // TODO: perhaps return false?
        if (strings.size() == 0) return;

        typecmd = atoi(strings[0].c_str());
        if (typecmd == 38)
        {
            // extended foreground color
            // which has either 5,<palette color> or
            // 2,<r>,<g>,<b>

            if (strings.size() < 3) return; // not enough parameters
            uint32_t coltype = atoi(strings[1].c_str());
            if (coltype == 5)
            {
                // palette type
                uint8_t paletteNum = atoi(strings[2].c_str());
                submitNewForegroundColour(paletteNum);
            }
            else if (coltype == 2)
            {
                // rgb type
                if (strings.size() < 5) return; // not enough parameters
                uint8_t r = atoi(strings[2].c_str());
                uint8_t g = atoi(strings[3].c_str());
                uint8_t b = atoi(strings[4].c_str());
            }
        }
        else if (typecmd == 48)
        {
            // extended background color
            // which has either 5,<palette color> or
            // 2,<r>,<g>,<b>

            if (strings.size() < 3) return; // not enough parameters
            uint32_t coltype = atoi(strings[1].c_str());
            if (coltype == 5)
            {
                // palette type
                uint8_t paletteNum = atoi(strings[2].c_str());
                submitNewBackgroundColour(paletteNum);
            }
            else if (coltype == 2)
            {
                // rgb type
                if (strings.size() < 5) return; // not enough parameters
                uint8_t r = atoi(strings[2].c_str());
                uint8_t g = atoi(strings[3].c_str());
                uint8_t b = atoi(strings[4].c_str());
            }
        }
        else
        {
            // unsupported command
            qDebug() << last << ": " << s.c_str();
        }
        break;
    default:
        // unsupported command
        qDebug() << last << ": " << s.c_str();
    }
}
