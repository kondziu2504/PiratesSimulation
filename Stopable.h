//
// Created by konrad on 13.01.2022.
//

#ifndef PIRATESSIMULATION_STOPABLE_H
#define PIRATESSIMULATION_STOPABLE_H

#include <condition_variable>
#include <atomic>

class Stopable {
    std::condition_variable thread_func_finished;
    std::condition_variable stop_requested_c_var;
    std::atomic<bool> stop_requested = false;
    std::atomic<bool> stopped = false;
    std::mutex stopped_mutex;

protected:
    virtual void ThreadFunc(const std::atomic<bool> &stop_requested) = 0;
    bool GetStopRequested();
    void WaitUntilStopRequested();

public:
    void Start();
    void RequestStop();
    void WaitUntilStopped();

    ~Stopable();
};


#endif //PIRATESSIMULATION_STOPABLE_H
