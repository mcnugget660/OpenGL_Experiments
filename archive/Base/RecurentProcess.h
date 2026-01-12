#ifndef FINALDAYONEARTH_RECURENTPROCESS_H
#define FINALDAYONEARTH_RECURENTPROCESS_H


#include "Process.h"

class RecurentProcess : public Process {
public:
    // In seconds
    explicit RecurentProcess(double interval, u_int8_t priority, void (*onActivation)(long deltaTime));

private:
    long interval_micro = 0;
    long previousCycle_micro = 0;

    bool isActive = false;

    void end();

    void begin() override;
};


#endif //FINALDAYONEARTH_RECURENTPROCESS_H
