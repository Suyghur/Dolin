#include <__bit_reference>
//
// Created by #Suyghur, on 2021/08/20.
//

#include "hawkeye_dumper_utils.h"
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
#include <cstdlib>
#include "hawkeye_log.h"
#include "hawkeye_signal_utils.h"
#include "hawkeye_private.h"

#if __LP64__
#define PRIPTR "016" PRIxPTR
#else
#define PRIPTR "08" PRIxPTR
#endif


//void DumperUtils::RecordNewline(MmapGuard *mmap_ptr, const char *format, ...) {
//
//    // writing file to log may be disabled.
//    if (mmap_ptr == nullptr) {
//        return;
//    }
//
//    char *buffer = static_cast<char *>(malloc(HAWKEYE_LOG_BUFFER_SIZE));
//    //    {
////        va_list args;
////        va_start(args, format);
////        __android_log_vprint(ANDROID_LOG_ERROR, TAG, format, args);
////        va_end(args);
////    }
//
//    // writing to a buffer.
//    int printed;
//    {
//        va_list args;
//        va_start(args, format);
//        printed = vsnprintf(buffer, HAWKEYE_LOG_BUFFER_SIZE, format, args);
//        va_end(args);
//    }
//
//    // printed contains the number of characters that would have been written if n had been sufficiently
//    // large, not counting the terminating null character.
//    if (printed > 0) {
//        if (printed >= HAWKEYE_LOG_BUFFER_SIZE) {
//            printed = HAWKEYE_LOG_BUFFER_SIZE - 1;
//        }
//
//        // replacing last buffer character with new line.
//        buffer[printed] = '\n';
//        // writing to a file including \n character.
//        mmap_ptr->Write(buffer, (size_t) printed + 1);
//
//    }
//    free(buffer);
//}
//
//void DumperUtils::RecordLine(MmapGuard *mmap_ptr, const char *format, ...) {
//    // writing file to log may be disabled.
//    if (mmap_ptr == nullptr) {
//        return;
//    }
//
//    char *buffer = static_cast<char *>(malloc(HAWKEYE_LOG_BUFFER_SIZE));
//
//    // writing to a buffer.
//    int printed;
//    {
//        va_list args;
//        va_start(args, format);
//        printed = vsnprintf(buffer, HAWKEYE_LOG_BUFFER_SIZE, format, args);
//        va_end(args);
//    }
//
//    // printed contains the number of characters that would have been written if n had been sufficiently
//    // large, not counting the terminating null character.
//    if (printed > 0) {
//        if (printed >= HAWKEYE_LOG_BUFFER_SIZE) {
//            printed = HAWKEYE_LOG_BUFFER_SIZE - 1;
//        }
//        // writing to a file not included \n character.
//        mmap_ptr->Write(buffer, (size_t) printed + 1);
//    }
//    free(buffer);
//}

void DumperUtils::DumpHeader(MmapGuard *mmap_ptr, long start_ts, long crash_ts, pid_t pid, pid_t tid, int signo, int si_code, void *falutaddr,
                             struct ucontext *context) {
    // a special marker of crash report beginning
    RecordNewline(mmap_ptr, SEP_HEAD);

    // this buffer we use to read data from system properties and to read other data from files.
    char str_buffer[PROP_VALUE_MAX];
    {
        RecordNewline(mmap_ptr, "Dolin-Hawkeye monitor: '%s'", FULL_VERSION);
        RecordNewline(mmap_ptr, "Scene type: '%s'", SCENE_TYPE);

        Stamp2Standard(start_ts, str_buffer, sizeof(str_buffer));
        RecordNewline(mmap_ptr, "Start time: '%s'", str_buffer);

        Stamp2Standard(crash_ts, str_buffer, sizeof(str_buffer));
        RecordNewline(mmap_ptr, "Crash time: '%s'", str_buffer);
//        RecordNewline(mmap_ptr, "PackageName: '%s'", SCENE_TYPE);
//        RecordNewline(mmap_ptr, "Version: '%s'");

        __system_property_get("ro.build.version.sdk", str_buffer);
        RecordNewline(mmap_ptr, "API version: '%s'", str_buffer);
//
        __system_property_get("ro.build.version.release", str_buffer);
        RecordNewline(mmap_ptr, "OS version: '%s'", str_buffer);

        GetKernelVersion(str_buffer, sizeof(str_buffer));
        RecordNewline(mmap_ptr, "Kernel version: '%s'", str_buffer);

        GetABIs(str_buffer, sizeof(str_buffer));
        RecordNewline(mmap_ptr, "ABIs: '%s'", str_buffer);

        __system_property_get("ro.product.manufacturer", str_buffer);
        RecordNewline(mmap_ptr, "Manufacturer: '%s'", str_buffer);

        __system_property_get("ro.product.brand", str_buffer);
        RecordNewline(mmap_ptr, "Brand: '%s'", str_buffer);

        __system_property_get("ro.product.model", str_buffer);
        RecordNewline(mmap_ptr, "Model: '%s'", str_buffer);

        // getting system properties and writing them to report.
        __system_property_get("ro.build.fingerprint", str_buffer);
        RecordNewline(mmap_ptr, "Build fingerprint: %s", str_buffer);

        __system_property_get("ro.revision", str_buffer);
        RecordNewline(mmap_ptr, "Revision: '0'");
    }

    // writing processor architecture.
#if defined(__arm__)
    RecordNewline(mmap_ptr, "ABI: 'arm'");
#elif defined(__aarch64__)
    RecordNewline(mmap_ptr, "ABI: 'arm64'");
#elif defined(__i386__)
    RecordNewline(mmap_ptr, "ABI: 'x86'");
#elif defined(__x86_64__)
    RecordNewline(mmap_ptr, "ABI: 'x86_64'");
#endif

    // writing a line about process and thread. re-using str_buffer for a process name.
    WriteProcessAndThreadInfo(mmap_ptr, pid, tid, str_buffer, SIZEOF_ARRAY(str_buffer));

    // writing an information about signal.
    DumpSignalInfo(mmap_ptr, signo, si_code, falutaddr, str_buffer, SIZEOF_ARRAY(str_buffer));

    // writing registers to a report.
    const mcontext_t *const ctx = &context->uc_mcontext;
#if defined(__arm__)
    RecordLine(mmap_ptr, "    r0 %08x  r1 %08x  r2 %08x  r3 %08x", ctx->arm_r0, ctx->arm_r1, ctx->arm_r2, ctx->arm_r3);
    RecordLine(mmap_ptr, "    r4 %08x  r5 %08x  r6 %08x  r7 %08x", ctx->arm_r4, ctx->arm_r5, ctx->arm_r6, ctx->arm_r7);
    RecordLine(mmap_ptr, "    r8 %08x  r9 %08x  sl %08x  fp %08x", ctx->arm_r8, ctx->arm_r9, ctx->arm_r10, ctx->arm_fp);
    RecordNewline(mmap_ptr, "    ip %08x  sp %08x  lr %08x  pc %08x  cpsr %08x", ctx->arm_ip, ctx->arm_sp, ctx->arm_lr, ctx->arm_pc, ctx->arm_cpsr);
#elif defined(__aarch64__)
    for (int i = 0; i < 28; i += 4) {
        RecordLine(mmap_ptr, "    x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx", i, ctx->regs[i], i + 1, ctx->regs[i + 1], i + 2,
                      ctx->regs[i + 2], i + 3, ctx->regs[i + 3]);
    }
    RecordNewline(mmap_ptr, "    x28  %016llx  x29  %016llx  x30  %016llx", ctx->regs[28], ctx->regs[29], ctx->regs[30]);
    RecordLine(mmap_ptr, "    sp   %016llx  pc   %016llx  pstate %016llx", ctx->sp, ctx->pc, ctx->pstate);
#elif defined(__i386__)
    RecordNewline(mmap_ptr, "    eax %08lx  ebx %08lx  ecx %08lx  edx %08lx", ctx->gregs[REG_EAX], ctx->gregs[REG_EBX], ctx->gregs[REG_ECX],
                  ctx->gregs[REG_EDX]);
    RecordNewline(mmap_ptr, "    esi %08lx  edi %08lx", ctx->gregs[REG_ESI], ctx->gregs[REG_EDI]);
    RecordNewline(mmap_ptr, "    xcs %08x  xds %08x  xes %08x  xfs %08x  xss %08x", ctx->gregs[REG_CS], ctx->gregs[REG_DS], ctx->gregs[REG_ES],
                  ctx->gregs[REG_FS], ctx->gregs[REG_SS]);
    RecordNewline(mmap_ptr, "    eip %08lx  ebp %08lx  esp %08lx  flags %08lx", ctx->gregs[REG_EIP], ctx->gregs[REG_EBP], ctx->gregs[REG_ESP],
                  ctx->gregs[REG_EFL]);
#elif defined(__x86_64__)
    RecordLine(mmap_ptr, "    rax %016lx  rbx %016lx  rcx %016lx  rdx %016lx", ctx->gregs[REG_RAX], ctx->gregs[REG_RBX], ctx->gregs[REG_RCX],
                  ctx->gregs[REG_RDX]);
    RecordNewline(mmap_ptr, "    rsi %016lx  rdi %016lx", ctx->gregs[REG_RSI], ctx->gregs[REG_RDI]);
    RecordLine(mmap_ptr, "    r8  %016lx  r9  %016lx  r10 %016lx  r11 %016lx", ctx->gregs[REG_R8], ctx->gregs[REG_R9], ctx->gregs[REG_R10],
                  ctx->gregs[REG_R11]);
    RecordLine(mmap_ptr, "    r12 %016lx  r13 %016lx  r14 %016lx  r15 %016lx", ctx->gregs[REG_R12], ctx->gregs[REG_R13], ctx->gregs[REG_R14],
                  ctx->gregs[REG_R15]);
    RecordLine(mmap_ptr, "    cs  %016lx", ctx->gregs[REG_CSGSFS]);
    RecordLine(mmap_ptr, "    rip %016lx  rbp %016lx  rsp %016lx  eflags %016lx", ctx->gregs[REG_RIP], ctx->gregs[REG_RBP], ctx->gregs[REG_RSP],
                  ctx->gregs[REG_EFL]);
#endif
    // writing "backtrace: "
    RecordNewline(mmap_ptr, " ");
    RecordNewline(mmap_ptr, "backtrace: ");
}

void DumperUtils::DumpOtherThreadHeader(MmapGuard *mmap_ptr, pid_t pid, pid_t tid) {
    // a special marker about next (not crashed) thread data beginning.
    RecordNewline(mmap_ptr, "\n%s", SEP_OTHER_INFO);

    // assuming 64 bytes is sufficient for a process name.
    char process_name_buffer[64];

    // writing a line about process and thread.
    WriteProcessAndThreadInfo(mmap_ptr, pid, tid, process_name_buffer, SIZEOF_ARRAY(process_name_buffer));

    // getting signal info by ptrace and writing to a dump.
    siginfo_t si;
    memset(&si, 0, sizeof(si));
    if (ptrace(PTRACE_GETSIGINFO, tid, 0, &si) == -1) {
        LOGE("Couldn't get signal info by ptrace: %s (%d)", strerror(errno), errno);
        return;
    }
    DumpSignalInfo(mmap_ptr, si.si_signo, si.si_code, si.si_addr, process_name_buffer, SIZEOF_ARRAY(process_name_buffer));

    // dumping registers information.
    DumpOtherThreadRegistersByPtrace(mmap_ptr, tid);

    // writing "backtrace: "
    RecordNewline(mmap_ptr, " ");
    RecordNewline(mmap_ptr, "backtrace: ");
}

void DumperUtils::DumpBacktraceLine(MmapGuard *mmap_ptr, int counter, intptr_t pc, const char *map_name, const char *func_name, intptr_t func_offset) {
    if (!map_name) {
        map_name = "<unknown>";
    } else if (!*map_name) {
        map_name = "<anonymous>";
    }
    if (!func_name) {
        RecordNewline(mmap_ptr, "    #%02d pc %" PRIPTR"  %s", counter, pc, map_name);
    } else {
        RecordNewline(mmap_ptr, "    #%02d pc %" PRIPTR"  %s (%s+%d)", counter, pc, map_name, func_name, (int) func_offset);
    }
}

void DumperUtils::DumpLogcatInfo(MmapGuard *mmap_ptr, pid_t pid) {
    /// system lines: 50
    /// native logcat events lines: 50
    /// native logcat main lines: 200
    RecordNewline(mmap_ptr, "\n%s", SEP_OTHER_INFO);
    RecordNewline(mmap_ptr, "logcat:");
    GetLogcatInfo(mmap_ptr, pid, "main", 200, 'D');
    GetLogcatInfo(mmap_ptr, pid, "system", 50, 'W');
    GetLogcatInfo(mmap_ptr, pid, "events", 50, 'I');
}

void DumperUtils::DumpFds(MmapGuard *mmap_ptr, pid_t pid) {
    RecordNewline(mmap_ptr, "\n%s", SEP_OTHER_INFO);
    RecordNewline(mmap_ptr, "open files:");
    GetFds(mmap_ptr, pid);
}

void DumperUtils::DumpNetworkInfo(MmapGuard *mmap_ptr, pid_t pid) {
    RecordNewline(mmap_ptr, "\n%s", SEP_OTHER_INFO);
    RecordNewline(mmap_ptr, "network info:");
    GetNetworkInfo(mmap_ptr, pid);
}

ssize_t DumperUtils::ReadFile(const char *file_name, char *out_buffer, size_t buffer_size) {
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

void DumperUtils::DumpOtherThreadRegistersByPtrace(MmapGuard *mmap_ptr, pid_t tid) {
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
    RecordNewline(mmap_ptr, "    r0 %08x  r1 %08x  r2 %08x  r3 %08x", (uint32_t) r.ARM_r0, (uint32_t) r.ARM_r1, (uint32_t) r.ARM_r2, (uint32_t) r.ARM_r3);
    RecordLine(mmap_ptr, "    r4 %08x  r5 %08x  r6 %08x  r7 %08x", (uint32_t) r.ARM_r4, (uint32_t) r.ARM_r5, (uint32_t) r.ARM_r6, (uint32_t) r.ARM_r7);
    RecordLine(mmap_ptr, "    r8 %08x  r9 %08x  sl %08x  fp %08x", (uint32_t) r.ARM_r8, (uint32_t) r.ARM_r9, (uint32_t) r.ARM_r10, (uint32_t) r.ARM_fp);
    RecordLine(mmap_ptr, "    ip %08x  sp %08x  lr %08x  pc %08x  cpsr %08x", (uint32_t) r.ARM_ip, (uint32_t) r.ARM_sp, (uint32_t) r.ARM_lr,
                  (uint32_t) r.ARM_pc, (uint32_t) r.ARM_cpsr);
#elif defined(__aarch64__)
    for (int i = 0; i < 28; i += 4) {
        RecordNewline(mmap_ptr, "    x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx  x%-2d  %016llx",
                      i, r.regs[i],
                      i + 1, r.regs[i + 1],
                      i + 2, r.regs[i + 2],
                      i + 3, r.regs[i + 3]);
    }
    RecordLine(mmap_ptr, "    x28  %016llx  x29  %016llx  x30  %016llx", r.regs[28], r.regs[29], r.regs[30]);
    RecordLine(mmap_ptr, "    sp   %016llx  pc   %016llx  pstate %016llx", r.sp, r.pc, r.pstate);
#elif defined(__i386__)
    RecordNewline(mmap_ptr, "    eax %08lx  ebx %08lx  ecx %08lx  edx %08lx", r.eax, r.ebx, r.ecx, r.edx);
    RecordNewline(mmap_ptr, "    esi %08lx  edi %08lx", r.esi, r.edi);
    RecordNewline(mmap_ptr, "    xcs %08x  xds %08x  xes %08x  xfs %08x  xss %08x", r.xcs, r.xds, r.xes, r.xfs, r.xss);
    RecordNewline(mmap_ptr, "    eip %08lx  ebp %08lx  esp %08lx  flags %08lx", r.eip, r.ebp, r.esp, r.eflags);
#elif defined(__x86_64__)
    RecordLine(mmap_ptr, "    rax %016lx  rbx %016lx  rcx %016lx  rdx %016lx", r.rax, r.rbx, r.rcx, r.rdx);
    RecordLine(mmap_ptr, "    rsi %016lx  rdi %016lx", r.rsi, r.rdi);
    RecordLine(mmap_ptr, "    r8  %016lx  r9  %016lx  r10 %016lx  r11 %016lx", r.r8, r.r9, r.r10, r.r11);
    RecordNewline(mmap_ptr, "    r12 %016lx  r13 %016lx  r14 %016lx  r15 %016lx", r.r12, r.r13, r.r14, r.r15);
    RecordLine(mmap_ptr, "    cs  %016lx  ss  %016lx", r.cs, r.ss);
    RecordLine(mmap_ptr, "    rip %016lx  rbp %016lx  rsp %016lx  eflags %016lx", r.rip, r.rbp, r.rsp, r.eflags);
#endif
    return;
    // error processing
    error:
    LOGE("Couldn't get registers by ptrace: %s (%d)", strerror(errno), errno);
//    RecordNewline(mmap_ptr, "Couldn't get registers by ptrace: %s (%d)", strerror(errno), errno);
}

void DumperUtils::WriteProcessAndThreadInfo(MmapGuard *mmap_ptr, pid_t pid, pid_t tid,
                                            char *process_name_buffer, size_t process_name_buffer_size) {
    // buffer used for file path formatting. Max theoretical value is "/proc/2147483647/cmdline" 25 characters with terminating characters.
    char proc_file_path[25];

    // thread name. Strings longer than TASK_COMM_LEN (16) characters are silently truncated.
    char proc_comm_content[16];

    // setting first characters for a case when reading is failed.
    process_name_buffer[0] = proc_file_path[0] = '\0';

    // reading a process name.
    if (snprintf(proc_file_path, SIZEOF_ARRAY(proc_file_path), "/proc/%d/cmdline", pid) >= 0) {
        ReadFile(proc_file_path, process_name_buffer, process_name_buffer_size);
    }

    // reading a thread name.
    if (snprintf(proc_file_path, SIZEOF_ARRAY(proc_file_path), "/proc/%d/comm", tid) >= 0) {
        const ssize_t bytes_read = ReadFile(proc_file_path, proc_comm_content, SIZEOF_ARRAY(proc_comm_content));

        // comm usually contains newline character on the end.
        if (bytes_read > 0 && proc_comm_content[bytes_read - 1] == '\n') {
            proc_comm_content[bytes_read - 1] = '\0';
        }
    }

    // writing to a log and to a file.
    RecordNewline(mmap_ptr, "pid: %d, tid: %d, name: %s,  >>> %s <<<", pid, tid, proc_comm_content, process_name_buffer);
}

void DumperUtils::DumpSignalInfo(MmapGuard *mmap_ptr, int signo, int si_code, void *faultaddr, char *str_buffer, size_t str_buffer_size) {
    if (SignalUtils::HasSiAddr(signo, si_code)) {
        snprintf(str_buffer, str_buffer_size, "%p", faultaddr);
    } else {
        snprintf(str_buffer, str_buffer_size, "--------");
    }
    RecordNewline(mmap_ptr, "signal %d (%s), code %d (%s), fault addr %s", signo, SignalUtils::GetSigName(signo), si_code,
                  SignalUtils::GetSigCode(signo, si_code), str_buffer);
}








