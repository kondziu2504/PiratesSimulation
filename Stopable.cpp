//
// Created by konrad on 13.01.2022.
//

#include "Stopable.h"
#include <thread>

using namespace std;

void Stopable::Start() {
    thread _thread([this](){
        ThreadFunc(stop_requested);
        lock_guard<mutex> guard(stopped_mutex);
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
    unique_lock<mutex> lock(stopped_mutex);
    if(stopped)
        return;
    thread_func_finished.wait(lock, [this](){return stopped.load();});
}

void Stopable::WaitUntilStopRequested() {
    if(stop_requested)
        return;
    mutex c_var_mutex;
    unique_lock<mutex> lock(c_var_mutex);
    stop_requested_c_var.wait(lock, [this](){return stop_requested.load();});
}

bool Stopable::GetStopRequested() {
    return stop_requested;
}

Stopable::~Stopable() {
    RequestStop();
    WaitUntilStopped();
}
