#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class SimpleTimer {
private:
    unsigned long startTime;
    bool running;

public:
    SimpleTimer() : startTime(0), running(false) {}
    
    void start() {
        startTime = millis();
        running = true;
    }
    
    void reset() {
        start();
    }
    
    bool expired(unsigned long intervalMs) {
        if (!running) return false;
        return (millis() - startTime) >= intervalMs;
    }
    
    unsigned long elapsed() const {
        if (!running) return 0;
        return millis() - startTime;
    }
    
    void stop() {
        running = false;
    }
    
    bool isRunning() const {
        return running;
    }
};

#endif // TIMER_H
