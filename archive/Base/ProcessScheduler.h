#ifndef FINALDAYONEARTH_PROCESSSCHEDULER_H
#define FINALDAYONEARTH_PROCESSSCHEDULER_H


#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include "RecurentProcess.h"

enum SchedulerMode {
    ACTIVE,
    INACTIVE,
    PRIORITY_SKIP, // A thread using the sleep function will be diverted to any queued process of higher priority
    ESTIMATED_TIME, // A thread using the sleep function will be diverted to the first queued process with an estimated time less than the sleep value
    ESTIMATED_TIME_PRIORITY_SKIP // A thread using the sleep function will be diverted according to queued processes' priority first, then estimated time
};

class ProcessScheduler {
public:
    ProcessScheduler(u_int16_t maxThreads, u_int16_t reservedThreads);


    void scheduleProcess(Process *process, u_int16_t delay_micro, bool join);

    void scheduleProcesses(std::vector<Process *> processes, u_int16_t delay_micro, bool join);

    u_int16_t reserveProcess(Process *process, u_int16_t delay_micro, bool join);


    void setMode(SchedulerMode mode);


    void sleep(long micro_seconds);

private:
    long THREAD_WAIT_MICRO = 5000;


    std::vector<std::thread> reservedThreads;

    std::vector<std::thread> queuedThreads;

    std::map<u_int8_t, std::list<Process *>> threadPriorityQueue;

    u_int16_t maxThreads = 0;
    u_int16_t maxQueueThreads = 0;
    u_int16_t maxReservedThreads = 0;
    u_int8_t maxPriority = 10;

    SchedulerMode mode = INACTIVE;
    int reservedThreadCount = 0;
    int queueThreadCount = 0;

    std::mutex mtx;
    std::mutex mtxReserved;
    std::mutex mtxQueue;

    void addToQueue(Process *process);

    void addToQueue(auto itr, u_int16_t count);

    Process *popQueue();

    void executeProcess(Process *process);

    // Will wait for the specified wait duration
    void wait();
};


#endif //FINALDAYONEARTH_PROCESSSCHEDULER_H
