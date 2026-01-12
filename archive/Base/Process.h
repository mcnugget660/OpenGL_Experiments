#ifndef FINALDAYONEARTH_PROCESS_H
#define FINALDAYONEARTH_PROCESS_H


#include <cstdlib>
#include "ProcessScheduler.h"

class Process {
    friend class ProcessScheduler;

public:
    explicit Process(u_int8_t priority, void (*run)()) : priority(priority), run(run) {}


    explicit Process(u_int8_t priority, void (*run)(), long estimatedTime_micro) : priority(priority), run(run) {}

    u_int8_t getPriority() const;

    bool isRunning();

private:

    long estimatedTime_micro = -1;

    u_int8_t priority = 0;

    void start();

    virtual void begin();

protected:
    bool running = false;

    void (*run)();
    
};


#endif //FINALDAYONEARTH_PROCESS_H
