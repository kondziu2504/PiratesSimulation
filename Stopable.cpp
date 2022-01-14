//
// Created by konrad on 13.01.2022.
//

#include "Stopable.h"
#include <thread>

void Stopable::Start() {
    std::thread _thread([this](){
        ThreadFunc(stop_requested);
        stopped = true;
        thread_func_finished.notify_all();
    });
    _thread.detach();
}

void Stopable::RequestStop() {
    stop_requested = true;
    stop_requested_c_var.notify_all();
}

void Stopable::WaitUntilStopped() {
    if(stopped)
        return;
    std::mutex c_var_mutex;
    std::unique_lock<std::mutex> lock(c_var_mutex);
    thread_func_finished.wait(lock, [this](){return stopped.load();});
}

void Stopable::WaitUntilStopRequested() {
    if(stop_requested)
        return;
    std::mutex c_var_mutex;
    std::unique_lock<std::mutex> lock(c_var_mutex);
    stop_requested_c_var.wait(lock, [this](){return stop_requested.load();});
}

bool Stopable::GetStopRequested() {
    return stop_requested;
}

Stopable::~Stopable() {
    RequestStop();
    WaitUntilStopped();
}
