#include <iostream>
#include "ProcessScheduler.h"

void ProcessScheduler::scheduleProcess(Process *process, u_int16_t delay_micro, bool join) {
    std::lock_guard<std::mutex> lock(mtx);
    if (queueThreadCount < maxQueueThreads)
        reservedThreads.emplace(reservedThreads.begin() + queueThreadCount++, &ProcessScheduler::executeProcess, this, process);
    else
        addToQueue(process);
}

void ProcessScheduler::scheduleProcesses(std::vector<Process *> processes, u_int16_t delay_micro, bool join) {
    std::lock_guard<std::mutex> lock(mtx);

    int remainingThreads = maxQueueThreads - queueThreadCount;

    int i = 0;
    while (remainingThreads-- > 0 && i < processes.size())
        reservedThreads.emplace(reservedThreads.begin() + queueThreadCount++, &ProcessScheduler::executeProcess, this, processes[i++]);

    if (i < processes.size())
        addToQueue(processes.begin() + i, processes.size() - i);
}

u_int16_t ProcessScheduler::reserveProcess(Process *process, u_int16_t delay_micro, bool join) {
    std::lock_guard<std::mutex> lock(mtxReserved);
    if (reservedThreadCount >= maxReservedThreads) {
        std::cout << "Failed to reserve thread for process\n";
        return -1;
    }
    int index = reservedThreadCount++;

    reservedThreads.emplace(reservedThreads.begin() + index, &ProcessScheduler::executeProcess, this, process);

    return index;
}

void ProcessScheduler::executeProcess(Process *process) {
    process->start();
    while (mode != INACTIVE) {
        Process *nextProcess = popQueue();
        if (nextProcess == nullptr) {
            wait();
            continue;
        }
        nextProcess->start();
    }
    // Remove thread
}

void ProcessScheduler::setMode(SchedulerMode mode) {
    ProcessScheduler::mode = mode;
    switch (mode) {
        case INACTIVE:
            // Shutdown all threads - lock until that happens
            break;
    }
}


void ProcessScheduler::addToQueue(Process *process) {
    std::lock_guard<std::mutex> lock(mtxQueue);

    threadPriorityQueue[process->getPriority()].push_back(process);
}

void ProcessScheduler::addToQueue(auto itr, u_int16_t count) {
    std::lock_guard<std::mutex> lock(mtxQueue);

    for (int i = 0; i < count; i++) {
        Process *process = *(itr + i);
        threadPriorityQueue[process->getPriority()].push_back(process);
    }
}

Process *ProcessScheduler::popQueue() {
    std::lock_guard<std::mutex> lock(mtxQueue);

    if (threadPriorityQueue.empty())
        return nullptr;


    Process *process = threadPriorityQueue.begin()->second.front();
    threadPriorityQueue.begin()->second.pop_front();

    if (threadPriorityQueue.begin()->second.empty())
        threadPriorityQueue.erase(threadPriorityQueue.begin());

    return process;
}

void ProcessScheduler::wait() {
    usleep(THREAD_WAIT_MICRO);
}

ProcessScheduler::ProcessScheduler(u_int16_t queueThreads, u_int16_t maxReservedThreads) : maxThreads(queueThreads + maxReservedThreads),
                                                                                           maxQueueThreads(queueThreads),
                                                                                           maxReservedThreads(maxReservedThreads),
                                                                                           reservedThreads(maxReservedThreads),
                                                                                           queuedThreads(ProcessScheduler::maxThreads) {
}


/*    // High is always greater than priority but low can be equal to or less than it
    int low = 0;
    int high = threadQueue.size() - 1;
    int mid = high / 2;

    // When low and high are next to eachother then low is the index of the furthest back thread of equal or less priority
    while (high - low > 1) {
        u_int8_t midPriority = queueThreadPriority[mid];

        if (midPriority > priority) {
            high = mid;
        } else {
            low = mid;
        }
        mid = (low + high) / 2;
    }
*/