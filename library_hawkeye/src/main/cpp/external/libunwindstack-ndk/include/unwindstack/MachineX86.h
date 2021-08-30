/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBUNWINDSTACK_MACHINE_X86_H
#define _LIBUNWINDSTACK_MACHINE_X86_H

#include <stdint.h>

namespace unwindstack {

// Matches the numbers for the registers as generated by compilers.
// If this is changed, then unwinding will fail.
    enum X86Reg : uint16_t {
        X86_REG_EAX = 0,
        X86_REG_ECX = 1,
        X86_REG_EDX = 2,
        X86_REG_EBX = 3,
        X86_REG_ESP = 4,
        X86_REG_EBP = 5,
        X86_REG_ESI = 6,
        X86_REG_EDI = 7,
        X86_REG_EIP = 8,
        X86_REG_EFL = 9,
        X86_REG_CS = 10,
        X86_REG_SS = 11,
        X86_REG_DS = 12,
        X86_REG_ES = 13,
        X86_REG_FS = 14,
        X86_REG_GS = 15,
        X86_REG_LAST,

        X86_REG_SP = X86_REG_ESP,
        X86_REG_PC = X86_REG_EIP,
    };

}  // namespace unwindstack

#endif  // _LIBUNWINDSTACK_MACHINE_X86_H
