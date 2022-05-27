#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

class Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> st, ed;
public:
    Timer() {
        set();
    }

    void set() {
        st = std::chrono::high_resolution_clock::now();
    }

    long long get() {
        ed = std::chrono::high_resolution_clock::now();
        auto duration = ed - st;
        return duration.count();
    }
};

#endif