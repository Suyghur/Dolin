//
// Created by #Suyghur, on 2021/08/23.
//

#include "hawkeye_unwinder.h"
#include "hawkeye_log.h"
#include "hawkeye_private.h"
#include "hawkeye_dumper_utils.h"


void *Unwinder::InitUnwinder(pid_t pid) {
    // using RemoteMaps instance as an opaque data.
    auto *const maps = new unwindstack::RemoteMaps(pid);
    if (!maps->Parse()) {
        LOGE("libunwindstack: failed to parse remote /pro/pid/maps.");
    }
    return maps;
}

void Unwinder::ReleaseUnwinder(void *data) {
    delete static_cast<unwindstack::RemoteMaps *>(data);
}

void Unwinder::DoUnwind(MmapGuard *mmap_ptr, int log_fd, pid_t tid, struct ucontext *context, void *data) {
    auto *const maps = static_cast<unwindstack::RemoteMaps *>(data);
    const std::shared_ptr<unwindstack::Memory> memory(new unwindstack::MemoryRemote(tid));
    std::unique_ptr<unwindstack::Regs> regs;
    if (context) {
        regs.reset(unwindstack::Regs::CreateFromUcontext(unwindstack::Regs::CurrentArch(), context));
    } else {
        regs.reset(unwindstack::Regs::RemoteGet(tid));
        if (!regs) {
            LOGE("libunwindstack: Couldn't get registers by ptrace for tid: %d", (int) tid);
            return;
        }
    }
    __DoUnwind(mmap_ptr, log_fd, regs, *maps, memory);
}

void Unwinder::__DoUnwind(MmapGuard *mmap_ptr, int log_fd, const std::unique_ptr<unwindstack::Regs> &regs, unwindstack::Maps &maps,
                          const std::shared_ptr<unwindstack::Memory> &memory) {
    // string for function name.
    std::string unw_function_name;
    for (size_t frame_num = 0; frame_num < HAWKEYE_MAX_FRAMES; frame_num++) {
        // looking for a map info item for pc on this unwinding step.
        unwindstack::MapInfo *const map_info = maps.Find(regs->pc());
        if (!map_info) {
            DumperUtils::DumpBacktraceLine(mmap_ptr, log_fd, (int) frame_num, (intptr_t) regs->pc(), nullptr, nullptr, 0);
            break;
        }

        // loading data from ELF.
        unwindstack::Elf *const elf = map_info->GetElf(memory, true);
        if (!elf) {
            DumperUtils::DumpBacktraceLine(mmap_ptr, log_fd, (int) frame_num, (intptr_t) regs->pc(), map_info->name.c_str(), nullptr, 0);
            break;
        }

        // getting value of program counter relative module where a function is located.
        const uint64_t rel_pc = elf->GetRelPc(regs->pc(), map_info);
        uint64_t adjusted_rel_pc = rel_pc;
        if (frame_num != 0) {
            // if it's not a first frame we need to rewind program counter value to previous instruction.
            // for the first frame pc from ucontext points exactly to a failed instruction, for other
            // frames rel_pc will contain return address after function call instruction.
            adjusted_rel_pc -= regs->GetPcAdjustment(rel_pc, elf);
        }

        // getting function name and writing value to a log.
        uint64_t func_offset = 0;
        if (elf->GetFunctionName(rel_pc, &unw_function_name, &func_offset)) {
            DumperUtils::DumpBacktraceLine(mmap_ptr, log_fd, (int) frame_num, (intptr_t) rel_pc, map_info->name.c_str(), unw_function_name.c_str(),
                                           (intptr_t) func_offset);
        } else {
            unw_function_name.clear();
            DumperUtils::DumpBacktraceLine(mmap_ptr, log_fd, (int) frame_num, (intptr_t) rel_pc, map_info->name.c_str(), nullptr, 0);
        }

        // trying to switch to a next frame.
        bool finished = false;
        if (!elf->Step(rel_pc, adjusted_rel_pc, map_info->elf_offset, regs.get(), memory.get(), &finished)) {
            break;
        }
    }
}
