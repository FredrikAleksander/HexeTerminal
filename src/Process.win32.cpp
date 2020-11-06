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

#ifdef WIN32
#include "Hexe/System/Process.h"
#include "WindowsErrors.h"
#include <assert.h>
#include <iomanip>
#include <sstream>

using namespace Hexe::System;
using namespace Hexe;

static std::wstring stringToWideString(const std::string &str)
{
    if (str.empty())
        return std::wstring();
    int size_needed =
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0],
                        size_needed);
    return wstrTo;
}

#define HANDLE_WIN_ERR(err) HRESULT_FROM_WIN32(err), PrintWinApiError(err)

static HRESULT
InitializeStartupInfoAttachedToPseudoConsole(STARTUPINFOEXW *pStartupInfo,
                                             HPCON hPC)
{
    HRESULT hr{E_UNEXPECTED};

    if (pStartupInfo)
    {
        size_t attrListSize{};

        pStartupInfo->StartupInfo.cb = sizeof(STARTUPINFOEXW);
        pStartupInfo->StartupInfo.dwFlags = STARTF_USESTDHANDLES;

        // Get the size of the thread attribute list.
        InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);

        // Allocate a thread attribute list of the correct size
        pStartupInfo->lpAttributeList =
            reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(
                HeapAlloc(GetProcessHeap(), 0, attrListSize));

        // Initialize thread attribute list
        if (pStartupInfo->lpAttributeList &&
            InitializeProcThreadAttributeList(pStartupInfo->lpAttributeList, 1,
                                              0, &attrListSize))
        {
            // Set Pseudo Console attribute
            hr = UpdateProcThreadAttribute(pStartupInfo->lpAttributeList, 0,
                                           PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                                           hPC, sizeof(hPC), NULL, NULL)
                     ? S_OK
                     : HANDLE_WIN_ERR(GetLastError());
        }
        else
        {
            hr = HANDLE_WIN_ERR(GetLastError());
        }
    }
    return hr;
}

Process::Process(AutoHandle &&hProcess,
                 LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList)
    : m_status(ProcessStatus::RUNNING), m_leaveRunning(false), m_exitCode(1), m_hProcess(std::move(hProcess)), m_lpAttributeList(lpAttributeList) {}

Process::~Process()
{
    if (m_lpAttributeList)
    {
        DeleteProcThreadAttributeList(m_lpAttributeList);
        HeapFree(GetProcessHeap(), 0, m_lpAttributeList);
    }
    if (!m_leaveRunning)
    {
        Terminate();
    }
}

void Process::CheckExitStatus()
{
    if (m_status == ProcessStatus::RUNNING)
    {
        DWORD exitCode;
        if (GetExitCodeProcess((HANDLE)m_hProcess, &exitCode) &&
            exitCode != STILL_ACTIVE)
        {
            m_status = ProcessStatus::EXITED;
            m_exitCode = (int)exitCode;
        }
    }
}

bool Process::HasExited() const { return m_status == ProcessStatus::EXITED; }

int Process::GetExitCode() const
{
    return m_status == ProcessStatus::EXITED ? m_exitCode : STILL_ACTIVE;
}

void Process::Terminate()
{
    if (m_status == ProcessStatus::RUNNING)
    {
        TerminateProcess((HANDLE)m_hProcess, EXIT_FAILURE);
        m_exitCode = EXIT_FAILURE;
        m_status = ProcessStatus::EXITED;
        m_hProcess.Release();
    }
}

void Process::WaitForExit()
{
    CheckExitStatus();

    if (m_status == ProcessStatus::RUNNING)
    {
        WaitForSingleObject((HANDLE)m_hProcess, INFINITE);
    }
}

std::unique_ptr<Process>
Process::CreateWithPseudoTerminal(const std::string &program,
                                  const std::vector<std::string> &args,
                                  const std::string &workingDirectory,
                                  Terminal::PseudoTerminal &pseudoTerminal)
{
    HRESULT hr{E_UNEXPECTED};

    AutoHandle hProcess{};
    AutoHandle hThread{};

    STARTUPINFOEXW startupInfo{};
    PROCESS_INFORMATION piClient{};

    std::ostringstream oss;
    oss << std::quoted(program);
    for (auto &arg : args)
    {
        oss << ' ' << arg;
    }
    std::wstring commandLine = stringToWideString(oss.str());
    // TODO: If workingDirectory is relative, make it absolute (relative to the
    // current process working directory)
    std::wstring workingDir = stringToWideString(workingDirectory);

    if ((hr = InitializeStartupInfoAttachedToPseudoConsole(
             &startupInfo, pseudoTerminal.m_phPC)) != S_OK)
    {
        PrintErrorResult(hr);
        goto fail;
    }

    hr = CreateProcessW(
             NULL,                           // No module name - use Command Line
             (wchar_t *)commandLine.c_str(), // Command Line
             NULL,                           // Process handle not inheritable
             NULL,                           // Thread handle not inheritable
             FALSE,                          // Inherit handles
             EXTENDED_STARTUPINFO_PRESENT,   // Creation flags
             NULL,                           // Use parent's environment block
             workingDir.empty()
                 ? NULL
                 : workingDir.c_str(), // Use parent's starting directory
             &startupInfo.StartupInfo, // Pointer to STARTUPINFO
             &piClient)                // Pointer to PROCESS_INFORMATION
             ? S_OK
             : HANDLE_WIN_ERR(GetLastError());

    if (hr != S_OK)
    {
        goto fail;
    }

    hProcess = AutoHandle(piClient.hProcess);
    hThread = AutoHandle(piClient.hThread);

    pseudoTerminal.m_attached = true;

    return std::unique_ptr<Process>(
        new Process(std::move(hProcess), startupInfo.lpAttributeList));
fail:
    return std::unique_ptr<Process>();
}

#endif
