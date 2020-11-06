#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Hexe/Terminal/PseudoTerminal.h"

#if !defined(WIN32) && !defined(__REFLECTION_PARSER__)
#include <sys/types.h>
#include <unistd.h>
#endif

namespace Hexe {

namespace System {
enum class ProcessStatus { RUNNING = 0, EXITED };

class Process final {
  private:
    ProcessStatus m_status;
    bool m_leaveRunning;
    int m_exitCode;
#ifdef WIN32
    AutoHandle m_hProcess;
    LPPROC_THREAD_ATTRIBUTE_LIST m_lpAttributeList;

    Process(AutoHandle &&processHandle,
            LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList);
#else
    int m_pid;
#endif

  public:
    ~Process();
    Process(const Process &) = delete;
    Process(Process &&) = delete;
    Process &operator=(const Process &) = delete;
    Process &operator=(Process &&) = delete;

    void CheckExitStatus();
    bool HasExited() const;
    int GetExitCode() const;

    void Terminate();
    void WaitForExit();

    static std::unique_ptr<Process>
    CreateWithPseudoTerminal(const std::string &program,
                             const std::vector<std::string> &args,
                             const std::string &workingDirectory,
                             Terminal::PseudoTerminal &pseudoTerminal);
};

} // namespace System

} // namespace Hexe
