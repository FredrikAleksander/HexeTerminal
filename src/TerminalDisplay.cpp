// The MIT License (MIT)

// Copyright (c) 2020 Fredrik A. Kristiansen

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
#include "Hexe/Terminal/TerminalDisplay.h"
#include "Hexe/Terminal/TerminalEmulator.h"
#include <assert.h>

#define MODBIT(x, set, bit) ((set) ? ((x) |= (bit)) : ((x) &= ~(bit)))

using namespace Hexe::Terminal;

TerminalDisplay::TerminalDisplay()
    : m_mode(MODE_VISIBLE), m_cursorMode(SteadyBar), m_emulator(nullptr)
{
}
void TerminalDisplay::Attach(TerminalEmulator *terminal)
{
    assert(m_emulator == nullptr);
    m_emulator = terminal;
}
void TerminalDisplay::Detach(TerminalEmulator *terminal)
{
    assert(m_emulator == terminal);
    m_emulator = nullptr;
}
void TerminalDisplay::Bell() {}
void TerminalDisplay::ResetColors() {}
int TerminalDisplay::ResetColor(int index, const char *name) { return 0; }

void TerminalDisplay::SetMode(win_mode mode, int set)
{
    int m = m_mode;
    MODBIT(((int &)m_mode), set, mode);
    if ((m_mode & MODE_REVERSE) != (m & MODE_REVERSE) && m_emulator)
    {
        m_emulator->Redraw();
    }
}

void TerminalDisplay::SetCursorMode(cursor_mode cursor)
{
    m_cursorMode = cursor;
}

cursor_mode TerminalDisplay::GetCursorMode() const
{
    return m_cursorMode;
}
void TerminalDisplay::SetTitle(const char *title) {}
void TerminalDisplay::SetIconTitle(const char *title) {}
void TerminalDisplay::SetClipboard(const char *text) {}
const char *TerminalDisplay::GetClipboard() const { return ""; }
