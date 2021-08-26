//
// Created by #Suyghur, on 2021/08/20.
//

#include <ucontext.h>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <android/log.h>
#include <cinttypes>
#include <cstring>
#include <cerrno>
#include <sys/system_properties.h>
#include <sys/ptrace.h>
#include <linux/elf.h>
#include <sys/system_properties.h>
#include "hawkeye_dumper_utils.h"
#include "hawkeye_sizeof_array.h"
#include "hawkeye_log.h"
#include "hawkeye_signal_utils.h"

#if __LP64__
#define PRIPTR "016" PRIxPTR
#else
#define PRIPTR "08" PRIxPTR
#endif

int DumperUtils::DumpCreateFile(const char *path) {
    const int result = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (result < 0) {
        LOGE("Error creating dump file %s: %s (%d)", path, strerror(errno), errno);
    }
    return result;
}

#ifndef HAWKEYE_LOG_BUFFER_SIZE
#define HAWKEYE_LOG_BUFFER_SIZE 256
#endif


void DumperUtils::Record2Buffer(MmapBuffer *mmap_ptr, const char *format, ...) {
    char buffer[HAWKEYE_LOG_BUFFER_SIZE];

    // writing to a log as is.
    {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_ERROR, TAG, format, args);
        va_end(args);
    }

    // writing file to log may be disabled.
    if (mmap_ptr == nullptr) {
        return;
    }

    // writing to a buffer.
    int printed;
    {
        va_list args;
        va_start(args, format);
        printed = vsnprintf(buffer, HAWKEYE_LOG_BUFFER_SIZE, format, args);
        va_end(args);
    }

    // printed contains the number of characters that would have been written if n had been sufficiently
    // large, not counting the terminating null character.
    if (printed > 0) {
        if (printed >= HAWKEYE_LOG_BUFFER_SIZE) {
            printed = HAWKEYE_LOG_BUFFER_SIZE - 1;
        }

        // replacing last buffer character with new line.
        buffer[printed] = '\n';

        // writing to a file including \n character.
//        write(fd, buffer, (size_t) printed + 1);
        mmap_ptr->Write(buffer);
    }
}

void DumperUtils::DumpWriteLine(int log_fd, const char *format, ...) {
    char buffer[HAWKEYE_LOG_BUFFER_SIZE];

    // writing to a log as is.
    {
        va_list args;
        va_start(args, format);
        __android_log_vprint(ANDROID_LOG_ERROR, TAG, format, args);
        va_end(args);
    }

    // writing file to log may be disabled.
    if (log_fd < 0) {
        return;
    }

    // writing to a buffer.
    int printed;
    {
        va_list args;
        va_start(args, format);
        printed = vsnprintf(buffer, HAWKEYE_LOG_BUFFER_SIZE, format, args);
        va_end(args);
    }

    // printed contains the number of characters that would have been written if n had been sufficiently
    // large, not counting the terminating null character.
    if (printed > 0) {
        if (printed >= HAWKEYE_LOG_BUFFER_SIZE) {
            printed = HAWKEYE_LOG_BUFFER_SIZE - 1;
        }

        // replacing last buffer character with new line.
        buffer[printed] = '\n';

        // writing to a file including \n character.
        write(log_fd, buffer, (size_t) printed + 1);
    }
}

//static inline void WriteBacktraceTitle(int out_file) {
//    DumperUtils::DumpWriteLine(out_file, " ");
//    DumperUtils::DumpWriteLine(out_file, "backtrace: ");
//}
void DumperUtils::DumpHeader(int log_fd, pid_t pid, pid_t tid, int signo, int si_code, void *falutaddr, struct ucontext *context) {
    // a special marker of crash report beginning
    DumpWriteLine(log_fd, SEP_HEAD);

    // this buffer we use to read data from system properties and to read other data from files.
    char str_buffer[PROP_VALUE_MAX];
    {
        // getting system properties and writing them to report.
        __system_property_get("ro.build.fingerprint", str_buffer);
        DumpWriteLine(log_fd, "Build fingerprint: %s", str_buffer);
        __system_property_get("ro.revision", str_buffer);
        DumpWriteLine(log_fd, "Revision: '0'");
    }

    // writing processor architecture.
#if defined(__arm__)
    DumpWriteLine(log_fd, "ABI: 'arm'");
#elif defined(__aarch64__)
    DumpWriteLine(log_fd, "ABI: 'arm64'");
#elif defined(__i386__)
    DumpWriteLine(log_fd, "ABI: 'x86'");
#elif defined(__x86_64__)
    DumpWriteLine(log_fd, "ABI: 'x86_64'");
#endif

    // writing a line about process and thread. re-using str_buffer for a process name.
    __WriteProcessAndThreadInfo(log_fd, pid, tid, str_buffer, SIZEOF_ARRAY(str_buffer));

    // writing an information about signal.
    __DumpSignalInfo(log_fd, signo, si_code, falutaddr, str_buffer, SIZEOF_ARRAY(str_buffer));

    // writing registers to a report.
    const mcontext_t *const ctx = &context->uc_mcontext;
#if defined(__arm__)
    DumpWriteLine(log_fd, "    r0 %08x  r1 %08x  r2 %08x  r3 %08x", ctx->arm_r0, ctx->arm_r1, ctx->arm_r2, ctx->arm_r3);
    DumpWriteLine(log_fd, "    r4 %08x  r5 %08x  r6 %08x  r7 %08x", ctx->arm_r4, ctx->arm_r5, ctx->arm_r6, ctx->arm_r7);
    DumpWriteLine(log_fd, "    r8 %08x  r9 %08x  sl %08x  fp %08x", ctx->arm_r8, ctx->arm_r9, ctx->arm_r10, ctx->arm_fp);
    DumpWriteLine(log_fd, "    ip %08x  sp %08x  lr %08x  pc %08x  cpsr %08x", ctx->arm_ip, ctx->arm_sp, ctx->arm_lr, ctx->arm_pc, ctx->arm_cpsr);
#elif defined(__aarch64__)
    for (int i = 0; i < 28; i += 4) {
        DumpWriteLine(log_fd, "    x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx", i, ctx->regs[i], i + 1, ctx->regs[i + 1], i + 2,
                           ctx->regs[i + 2], i + 3, ctx->regs[i + 3]);
    }
    DumpWriteLine(log_fd, "    x28  %016llx  x29  %016llx  x30  %016llx", ctx->regs[28], ctx->regs[29], ctx->regs[30]);
    DumpWriteLine(log_fd, "    sp   %016llx  pc   %016llx  pstate %016llx", ctx->sp, ctx->pc, ctx->pstate);
#elif defined(__i386__)
    DumpWriteLine(log_fd, "    eax %08lx  ebx %08lx  ecx %08lx  edx %08lx", ctx->gregs[REG_EAX], ctx->gregs[REG_EBX], ctx->gregs[REG_ECX],
                  ctx->gregs[REG_EDX]);
    DumpWriteLine(log_fd, "    esi %08lx  edi %08lx", ctx->gregs[REG_ESI], ctx->gregs[REG_EDI]);
    DumpWriteLine(log_fd, "    xcs %08x  xds %08x  xes %08x  xfs %08x  xss %08x", ctx->gregs[REG_CS], ctx->gregs[REG_DS], ctx->gregs[REG_ES],
                  ctx->gregs[REG_FS], ctx->gregs[REG_SS]);
    DumpWriteLine(log_fd, "    eip %08lx  ebp %08lx  esp %08lx  flags %08lx", ctx->gregs[REG_EIP], ctx->gregs[REG_EBP], ctx->gregs[REG_ESP],
                  ctx->gregs[REG_EFL]);
#elif defined(__x86_64__)
    DumpWriteLine(log_fd, "    rax %016lx  rbx %016lx  rcx %016lx  rdx %016lx", ctx->gregs[REG_RAX], ctx->gregs[REG_RBX], ctx->gregs[REG_RCX],
                       ctx->gregs[REG_RDX]);
    DumpWriteLine(log_fd, "    rsi %016lx  rdi %016lx", ctx->gregs[REG_RSI], ctx->gregs[REG_RDI]);
    DumpWriteLine(log_fd, "    r8  %016lx  r9  %016lx  r10 %016lx  r11 %016lx", ctx->gregs[REG_R8], ctx->gregs[REG_R9], ctx->gregs[REG_R10],
                       ctx->gregs[REG_R11]);
    DumpWriteLine(log_fd, "    r12 %016lx  r13 %016lx  r14 %016lx  r15 %016lx", ctx->gregs[REG_R12], ctx->gregs[REG_R13], ctx->gregs[REG_R14],
                       ctx->gregs[REG_R15]);
    DumpWriteLine(log_fd, "    cs  %016lx", ctx->gregs[REG_CSGSFS]);
    DumpWriteLine(log_fd, "    rip %016lx  rbp %016lx  rsp %016lx  eflags %016lx", ctx->gregs[REG_RIP], ctx->gregs[REG_RBP], ctx->gregs[REG_RSP],
                       ctx->gregs[REG_EFL]);
#endif
    // writing "backtrace: "
    DumpWriteLine(log_fd, " ");
    DumpWriteLine(log_fd, "backtrace: ");
}

void DumperUtils::DumpOtherThreadHeader(int log_fd, pid_t pid, pid_t tid) {
    // a special marker about next (not crashed) thread data beginning.
    DumpWriteLine(log_fd, SEP_OTHER_INFO);

    // assuming 64 bytes is sufficient for a process name.
    char process_name_buffer[64];

    // writing a line about process and thread.
    __WriteProcessAndThreadInfo(log_fd, pid, tid, process_name_buffer, SIZEOF_ARRAY(process_name_buffer));

    // getting signal info by ptrace and writing to a dump.
    siginfo_t si;
    memset(&si, 0, sizeof(si));
    if (ptrace(PTRACE_GETSIGINFO, tid, 0, &si) == -1) {
        LOGE("Couldn't get signal info by ptrace: %s (%d)", strerror(errno), errno);
        return;
    }
    __DumpSignalInfo(log_fd, si.si_signo, si.si_code, si.si_addr, process_name_buffer, SIZEOF_ARRAY(process_name_buffer));

    // dumping registers information.
    __DumpOtherThreadRegistersByPtrace(log_fd, tid);

    // writing "backtrace: "
    DumpWriteLine(log_fd, " ");
    DumpWriteLine(log_fd, "backtrace: ");
}

void DumperUtils::DumpBacktraceLine(int log_fd, int counter, intptr_t pc, const char *map_name, const char *func_name, intptr_t func_offset) {
    if (!map_name) {
        map_name = "<unknown>";
    } else if (!*map_name) {
        map_name = "<anonymous>";
    }
    if (!func_name) {
        DumpWriteLine(log_fd, "    #%02d pc %" PRIPTR"  %s", counter, pc, map_name);
    } else {
        DumpWriteLine(log_fd, "    #%02d pc %" PRIPTR"  %s (%s+%d)", counter, pc, map_name, func_name, (int) func_offset);
    }
}

ssize_t DumperUtils::__ReadFile(const char *file_name, char *out_buffer, size_t buffer_size) {
    const int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    ssize_t bytes_read;
    ssize_t overall_read = 0;
    while ((bytes_read = read(fd, out_buffer + overall_read, buffer_size - overall_read - 1)) > 0) {
        overall_read += bytes_read;
        if (overall_read >= buffer_size - 1) {
            break;
        }
    }
    if (bytes_read < 0) {
        close(fd);
        return -1;
    }
    out_buffer[overall_read] = '\0';
    close(fd);
    return overall_read;
}

void DumperUtils::__DumpOtherThreadRegistersByPtrace(int log_fd, pid_t tid) {
#if defined(__aarch64__)
    // For arm64 modern PTRACE_GETREGSET request should be executed
    struct user_pt_regs r{};
    struct iovec io{};
    io.iov_base = &r;
    io.iov_len = sizeof(r);
    if (ptrace(PTRACE_GETREGSET, tid, (void *) NT_PRSTATUS, &io) == -1) {
        goto error;
    }
#else
    // for other architectures PTRACE_GETERGS is sufficient.
#if defined(__x86_64__)
    struct user_regs_struct r{};
#else
    struct pt_regs r{};
#endif
    if (ptrace(PTRACE_GETREGS, tid, 0, &r) == -1) {
        goto error;
    }
#endif
#if  defined(__arm__)
    DumpWriteLine(log_fd, "    r0 %08x  r1 %08x  r2 %08x  r3 %08x", (uint32_t) r.ARM_r0, (uint32_t) r.ARM_r1, (uint32_t) r.ARM_r2, (uint32_t) r.ARM_r3);
    DumpWriteLine(log_fd, "    r4 %08x  r5 %08x  r6 %08x  r7 %08x", (uint32_t) r.ARM_r4, (uint32_t) r.ARM_r5, (uint32_t) r.ARM_r6, (uint32_t) r.ARM_r7);
    DumpWriteLine(log_fd, "    r8 %08x  r9 %08x  sl %08x  fp %08x", (uint32_t) r.ARM_r8, (uint32_t) r.ARM_r9, (uint32_t) r.ARM_r10, (uint32_t) r.ARM_fp);
    DumpWriteLine(log_fd, "    ip %08x  sp %08x  lr %08x  pc %08x  cpsr %08x", (uint32_t) r.ARM_ip, (uint32_t) r.ARM_sp, (uint32_t) r.ARM_lr,
                  (uint32_t) r.ARM_pc, (uint32_t) r.ARM_cpsr);
#elif defined(__aarch64__)
    for (int i = 0; i < 28; i += 4) {
        DumpWriteLine(log_fd, "    x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx",
                      i, r.regs[i],
                      i + 1, r.regs[i + 1],
                      i + 2, r.regs[i + 2],
                      i + 3, r.regs[i + 3]);
    }
    DumpWriteLine(log_fd, "    x28  %016llx  x29  %016llx  x30  %016llx", r.regs[28], r.regs[29], r.regs[30]);
    DumpWriteLine(log_fd, "    sp   %016llx  pc   %016llx  pstate %016llx", r.sp, r.pc, r.pstate);
#elif defined(__i386__)
    DumpWriteLine(log_fd, "    eax %08lx  ebx %08lx  ecx %08lx  edx %08lx", r.eax, r.ebx, r.ecx, r.edx);
    DumpWriteLine(log_fd, "    esi %08lx  edi %08lx", r.esi, r.edi);
    DumpWriteLine(log_fd, "    xcs %08x  xds %08x  xes %08x  xfs %08x  xss %08x", r.xcs, r.xds, r.xes, r.xfs, r.xss);
    DumpWriteLine(log_fd, "    eip %08lx  ebp %08lx  esp %08lx  flags %08lx", r.eip, r.ebp, r.esp, r.eflags);
#elif defined(__x86_64__)
    DumpWriteLine(log_fd, "    rax %016lx  rbx %016lx  rcx %016lx  rdx %016lx", r.rax, r.rbx, r.rcx, r.rdx);
    DumpWriteLine(log_fd, "    rsi %016lx  rdi %016lx", r.rsi, r.rdi);
    DumpWriteLine(log_fd, "    r8  %016lx  r9  %016lx  r10 %016lx  r11 %016lx", r.r8, r.r9, r.r10, r.r11);
    DumpWriteLine(log_fd, "    r12 %016lx  r13 %016lx  r14 %016lx  r15 %016lx", r.r12, r.r13, r.r14, r.r15);
    DumpWriteLine(log_fd, "    cs  %016lx  ss  %016lx", r.cs, r.ss);
    DumpWriteLine(log_fd, "    rip %016lx  rbp %016lx  rsp %016lx  eflags %016lx", r.rip, r.rbp, r.rsp, r.eflags);
#endif
    return;
    // error processing
    error:
    LOGE("Couldn't get registers by ptrace: %s (%d)", strerror(errno), errno);
}

void DumperUtils::__WriteProcessAndThreadInfo(int log_fd, pid_t pid, pid_t tid, char *process_name_buffer, size_t process_name_buffer_size) {
    // buffer used for file path formatting. Max theoretical value is "/proc/2147483647/cmdline" 25 characters with terminating characters.
    char proc_file_path[25];

    // thread name. Strings longer than TASK_COMM_LEN (16) characters are silently truncated.
    char proc_comm_content[16];

    // setting first characters for a case when reading is failed.
    process_name_buffer[0] = proc_file_path[0] = '\0';

    // reading a process name.
    if (snprintf(proc_file_path, SIZEOF_ARRAY(proc_file_path), "/proc/%d/cmdline", pid) >= 0) {
        __ReadFile(proc_file_path, process_name_buffer, process_name_buffer_size);
    }

    // reading a thread name.
    if (snprintf(proc_file_path, SIZEOF_ARRAY(proc_file_path), "/proc/%d/comm", tid) >= 0) {
        const ssize_t bytes_read = __ReadFile(proc_file_path, proc_comm_content, SIZEOF_ARRAY(proc_comm_content));

        // comm usually contains newline character on the end.
        if (bytes_read > 0 && proc_comm_content[bytes_read - 1] == '\n') {
            proc_comm_content[bytes_read - 1] = '\0';
        }
    }

    // writing to a log and to a file.
    DumpWriteLine(log_fd, "pid: %d, tid: %d, name: %s,  >>> %s <<<", pid, tid, proc_comm_content, process_name_buffer);
}

void DumperUtils::__DumpSignalInfo(int log_fd, int signo, int si_code, void *faultaddr, char *str_buffer, size_t str_buffer_size) {
    if (SignalUtils::HasSiAddr(signo, si_code)) {
        snprintf(str_buffer, str_buffer_size, "%p", faultaddr);
    } else {
        snprintf(str_buffer, str_buffer_size, "--------");
    }
    DumpWriteLine(log_fd, "signal %d (%s), code %d (%s), fault addr %s", signo, SignalUtils::GetSigName(signo), si_code,
                  SignalUtils::GetSigCode(signo, si_code), str_buffer);
}








