//
// Created by #Suyghur, on 4/6/21.
//

#include "libs/terminator.h"

static volatile bool terminated = false;

void setTerminated(bool is_terminated) {
    terminated = is_terminated;
}

bool isTerminated() {
    return terminated;
}
