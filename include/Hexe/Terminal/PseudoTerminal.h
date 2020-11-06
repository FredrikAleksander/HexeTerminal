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
#pragma once

#include "Hexe/AutoHandle.h"
#include <memory>

namespace Hexe
{
    namespace System
    {
        class Process;
    }
    namespace Terminal
    {
        class PseudoTerminal final
        {
        private:
            friend class ::Hexe::System::Process;
#ifdef WIN32
            COORD m_size;

            AutoHandle m_hInput;
            AutoHandle m_hOutput;
            HPCON m_phPC;

            bool m_attached;

            PseudoTerminal(int columns, int rows, AutoHandle &&hInput, AutoHandle &&hOutput, HPCON hPC);
#else
            int m_columns;
            int m_rows;

            AutoHandle m_master;
            AutoHandle m_slave;

            PseudoTerminal(int columns, int rows, AutoHandle &&master, AutoHandle &&slave);
#endif
        public:
            virtual ~PseudoTerminal();

            PseudoTerminal(PseudoTerminal &&) = delete;
            PseudoTerminal(const PseudoTerminal &) = delete;
            PseudoTerminal &operator=(PseudoTerminal &&) = delete;
            PseudoTerminal &operator=(const PseudoTerminal &) = delete;

            int GetNumColumns() const;
            int GetNumRows() const;

            bool Resize(int columns, int rows);
            int Write(const char *s, size_t n);
            int Read(char *buf, size_t n, bool block = false);

            static std::unique_ptr<PseudoTerminal> Create(int columns, int rows);
        };
    } // namespace Terminal
} // namespace Hexe
