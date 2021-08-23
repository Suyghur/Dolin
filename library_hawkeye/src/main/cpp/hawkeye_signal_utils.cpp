//
// Created by #Suyghur, on 2021/08/20.
//

#include "hawkeye_signal_utils.h"
#include "hawkeye_private.h"
#include <cstring>

bool SignalUtils::HasSiAddr(int si_signo, int si_code) {
    if (si_code == SI_USER || si_code == SI_QUEUE || si_code == SI_TKILL) {
        return false;
    }

    switch (si_signo) {
        case SIGBUS:
        case SIGFPE:
        case SIGILL:
        case SIGSEGV:
        case SIGTRAP:
            return true;
        default:
            return false;
    }
}

const char *SignalUtils::GetSigName(int sig) {
    switch (sig) {
        case SIGABRT:
            return "SIGABRT";
        case SIGBUS:
            return "SIGBUS";
        case SIGFPE:
            return "SIGFPE";
        case SIGILL:
            return "SIGILL";
        case SIGSEGV:
            return "SIGSEGV";
#if defined(SIGSTKFLT)
        case SIGSTKFLT:
            return "SIGSTKFLT";
#endif
        case SIGSTOP:
            return "SIGSTOP";
        case SIGSYS:
            return "SIGSYS";
        case SIGTRAP:
            return "SIGTRAP";
        default:
            return "?";
    }
}

const char *SignalUtils::GetSigCode(int signo, int code) {
    switch (signo) {
        case SIGILL:
            switch (code) {
                case ILL_ILLOPC:
                    return "ILL_ILLOPC";
                case ILL_ILLOPN:
                    return "ILL_ILLOPN";
                case ILL_ILLADR:
                    return "ILL_ILLADR";
                case ILL_ILLTRP:
                    return "ILL_ILLTRP";
                case ILL_PRVOPC:
                    return "ILL_PRVOPC";
                case ILL_PRVREG:
                    return "ILL_PRVREG";
                case ILL_COPROC:
                    return "ILL_COPROC";
                case ILL_BADSTK:
                    return "ILL_BADSTK";
            }
            break;
        case SIGBUS:
            switch (code) {
                case BUS_ADRALN:
                    return "BUS_ADRALN";
                case BUS_ADRERR:
                    return "BUS_ADRERR";
                case BUS_OBJERR:
                    return "BUS_OBJERR";
            }
            break;
        case SIGFPE:
            switch (code) {
                case FPE_INTDIV:
                    return "FPE_INTDIV";
                case FPE_INTOVF:
                    return "FPE_INTOVF";
                case FPE_FLTDIV:
                    return "FPE_FLTDIV";
                case FPE_FLTOVF:
                    return "FPE_FLTOVF";
                case FPE_FLTUND:
                    return "FPE_FLTUND";
                case FPE_FLTRES:
                    return "FPE_FLTRES";
                case FPE_FLTINV:
                    return "FPE_FLTINV";
                case FPE_FLTSUB:
                    return "FPE_FLTSUB";
            }
            break;
        case SIGSEGV:
            switch (code) {
                case SEGV_MAPERR:
                    return "SEGV_MAPERR";
                case SEGV_ACCERR:
                    return "SEGV_ACCERR";
#if defined(SEGV_BNDERR)
                case SEGV_BNDERR:
                    return "SEGV_BNDERR";
#endif
#if defined(SEGV_PKUERR)
                case SEGV_PKUERR:
                    return "SEGV_PKUERR";
#endif
#if defined(SEGV_ACCADI)
                case SEGV_ACCADI:
                    return "SEGV_ACCADI";
#endif

#if defined(SEGV_ADIDERR)
                case SEGV_ADIDERR:
                    return "SEGV_ADIDERR";
#endif
#if defined(SEGV_ADIPERR)
                case SEGV_ADIPERR:
                    return "SEGV_ADIPERR";
#endif
            }
            break;
#if defined(SYS_SECCOMP)
        case SIGSYS:
            switch (code) {
                case SYS_SECCOMP:
                    return "SYS_SECCOMP";
            }
            break;
#endif
        case SIGTRAP:
            switch (code) {
                case TRAP_BRKPT:
                    return "TRAP_BRKPT";
                case TRAP_TRACE:
                    return "TRAP_TRACE";
            }
            break;
    }
    switch (code) {
        case SI_USER:
            return "SI_USER";
        case SI_KERNEL:
            return "SI_KERNEL";
        case SI_QUEUE:
            return "SI_QUEUE";
        case SI_TIMER:
            return "SI_TIMER";
        case SI_MESGQ:
            return "SI_MESGQ";
        case SI_ASYNCIO:
            return "SI_ASYNCIO";
        case SI_SIGIO:
            return "SI_SIGIO";
        case SI_TKILL:
            return "SI_TKILL";
        case SI_DETHREAD:
            return "SI_DETHREAD";
    }
    return "?";
}

bool SignalUtils::RegisterSignalHandler(hawkeye_signal_handler_function handler, struct sigaction *old_handlers) {
    struct sigaction action{};
    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    for (int signo : SIGNALS_TO_CATCH) {
        if (sigaction(signo, &action, &old_handlers[signo])) {
            return false;
        }
    }
    return true;
}

void SignalUtils::UnregisterSignalHandler(struct sigaction *old_handlers) {
    for (int signo = 0; signo < NSIG; ++signo) {
        const struct sigaction *old_handler = &old_handlers[signo];
        if (!old_handler->sa_handler) continue;
        sigaction(signo, old_handler, nullptr);
    }
}
