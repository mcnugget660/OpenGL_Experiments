#include <thread>
#include "Process.h"

void Process::start() {
    running = true;
    begin();
    running = false;
}


u_int8_t Process::getPriority() const {
    return priority;
}


bool Process::isRunning() {
    return running;
}

void Process::begin() {
    run();
}

