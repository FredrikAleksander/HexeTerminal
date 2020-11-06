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
#include "Hexe/System/Process.h"
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace Hexe::System;

Process::~Process()
{
    if (m_pid != -1)
    {
        kill(m_pid, SIGHUP);
    }
}

void Process::CheckExitStatus()
{
    if (m_status == ProcessStatus::EXITED)
    {
        return;
    }
    int status = 0;
    int p = waitpid(m_pid, &status, WNOHANG);
    if (p < 0)
    {
        perror("Process::CheckExitStatus(waitpid)");
    }
    if (p == 0)
    {
        return;
    }
    m_status = ProcessStatus::EXITED;
    m_exitCode = WEXITSTATUS(status);
}

void Process::Terminate()
{
    if (m_status == ProcessStatus::EXITED)
    {
        return;
    }
    if (m_pid != -1)
    {
        kill(m_pid, SIGHUP);
        m_exitCode = 1;
        m_status = ProcessStatus::EXITED;
    }
}

void Process::WaitForExit()
{
    if (m_status == ProcessStatus::EXITED)
    {
        return;
    }
    int status = 0;
    int p = waitpid(m_pid, &status, 0);
    if (p < 0)
    {
        perror("Process::WaitForExit(waitpid)");
        return;
    }
    m_status = ProcessStatus::EXITED;
    m_exitCode = WEXITSTATUS(status);
}

std::unique_ptr<Process>
Process::CreateWithPseudoTerminal(const std::string &program,
                                  const std::vector<std::string> &args,
                                  const std::string &workingDirectory,
                                  Terminal::PseudoTerminal &pseudoTerminal)
{

    int x;
    int y;

    return nullptr;
}

#endif
