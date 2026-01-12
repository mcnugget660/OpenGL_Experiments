#include "RecurentProcess.h"
#include <chrono>
#include <thread>

long getTimeMicro() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void RecurentProcess::begin() {
    isActive = true;

    previousCycle_micro = getTimeMicro();

    while (isActive) {
        long currentTime = getTimeMicro();

        long sleepTime = (previousCycle_micro + interval_micro) - currentTime;
        usleep(sleepTime > 0 ? sleepTime : 0);

        if (!isActive)
            break;

        running = true;

        currentTime = getTimeMicro();
        long deltaTime = currentTime - previousCycle_micro;
        previousCycle_micro = currentTime;

        ((void (*)(long)) run)(deltaTime);

        running = false;
    }
}

void RecurentProcess::end() {
    isActive = false;
}

RecurentProcess::RecurentProcess(double interval, u_int8_t priority, void (*onActivation)(long)) :
        Process(priority, reinterpret_cast<void (*)(void)>(onActivation)), interval_micro(interval * 1000000) {
}
