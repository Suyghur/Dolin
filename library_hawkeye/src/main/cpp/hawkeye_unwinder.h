//
// Created by #Suyghur, on 2021/08/23.
//

#ifndef DOLIN_HAWKEYE_UNWINDER_H
#define DOLIN_HAWKEYE_UNWINDER_H

#include <sys/types.h>
#include <ucontext.h>
#include <unwindstack/Elf.h>
#include <unwindstack/MapInfo.h>
#include <unwindstack/Maps.h>
#include <unwindstack/Memory.h>
#include <unwindstack/Regs.h>
#include "hawkeye_mmap_buffer.h"

#if defined(__cplusplus)
extern "C" {
#endif


class Unwinder {
public:
    static void *InitUnwinder(pid_t pid);

    static void ReleaseUnwinder(void *data);

    static void DoUnwind(MmapBuffer *mmap_ptr, int log_fd, pid_t tid, struct ucontext *context, void *data);

private:
    static void __DoUnwind(MmapBuffer *mmap_ptr, int log_fd, const std::unique_ptr<unwindstack::Regs> &regs, unwindstack::Maps &maps,
                           const std::shared_ptr<unwindstack::Memory> &memory);

};

#if defined(__cplusplus)
}
#endif

#endif //DOLIN_HAWKEYE_UNWINDER_H
