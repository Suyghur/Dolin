//
// Created by #Suyghur, on 2021/08/18.
//

#include "hawkeye_core.h"

struct hawkeye_core_context *context = nullptr;


/// perform signal
void PerformSignalCallback(int signo, struct siginfo *siginfo, void *ctx) {
    // Restoring an old handler to make built-in Android crash mechanism work.
    sigaction(signo, &context->old_handlers[signo], nullptr);

    // Filling message fields.
//    struct
}