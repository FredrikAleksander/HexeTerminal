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

#ifndef WIN32
// Windows has its own source file
#include "Hexe/Terminal/PseudoTerminal.h"
#include <cstdio>
#include <poll.h>
#include <pty.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>

using namespace Hexe::Terminal;

PseudoTerminal::~PseudoTerminal() {}
PseudoTerminal::PseudoTerminal(int columns, int rows, AutoHandle &&master,
                               AutoHandle &&slave)
    : m_columns(columns), m_rows(rows), m_master(std::move(master)), m_slave(std::move(slave)) {}

bool PseudoTerminal::Resize(int columns, int rows)
{
    struct winsize w;

    w.ws_row = rows;
    w.ws_col = columns;
    w.ws_xpixel = 0;
    w.ws_ypixel = 0;

    if (ioctl((int)m_master, TIOCSWINSZ, &w) < 0)
    {
        perror("PseudoTerminal::Resize");
        return false;
    }

    return true;
}

int PseudoTerminal::GetNumColumns() const { return m_columns; }

int PseudoTerminal::GetNumRows() const { return m_rows; }

int PseudoTerminal::Write(const char *s, size_t n)
{
    size_t c = n;
    while (n > 0)
    {
        ssize_t r = write((int)m_master, s, n);
        if (r < 0)
        {
            return -1;
        }
        n -= r;
        s += r;
    }
    return c;
}

int PseudoTerminal::Read(char *s, size_t n, bool block)
{
    struct pollfd pfd;
    pfd.fd = (int)m_master;
    pfd.events = POLLIN;
    pfd.revents = 0;

    if (!block)
    {
        auto i = poll(&pfd, 1, 0);
        if (i == 0 || !(pfd.revents & POLLIN))
        {
            return 0;
        }
        if (i < 0)
        {
            perror("PseudoTerminal::Reader(poll)");
            return -1;
        }
    }
    ssize_t r = read((int)m_master, s, n);
    return (int)r;
}

std::unique_ptr<PseudoTerminal> PseudoTerminal::Create(int columns, int rows)
{
    AutoHandle master;
    AutoHandle slave;

    struct winsize win;

    memset(&win, 0, sizeof(win));

    win.ws_col = columns;
    win.ws_row = rows;

    if (openpty(master.Get(), slave.Get(), nullptr, NULL, &win) != 0)
    {
        perror("PseudoTerminal::Create(openpty)");
        return nullptr;
    }

    return std::unique_ptr<PseudoTerminal>(
        new PseudoTerminal(columns, rows, std::move(master), std::move(slave)));
}

#endif
