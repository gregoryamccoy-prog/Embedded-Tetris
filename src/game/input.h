#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <Bounce2.h>
#include "../hardware_pins.h"
#include "../config.h"

// Input state structure - edge-triggered events
struct InputState {
    bool pausePressed;
    bool rotatePressed;
    bool rightPressed;
    bool leftPressed;
    bool hardDropPressed;
};

// Joystick axis zones
enum class JoystickZone {
    CENTER,
    SMALL_DEFLECTION,
    LARGE_DEFLECTION
};

class InputManager {
private:
    // Button debouncers
    Bounce pauseButton;
    Bounce rotateButton;
    Bounce rightButton;
    Bounce leftButton;
    Bounce hardDropButton;
    Bounce joystickSwButton;
    
    // Joystick analog state
    int joystickXRaw[JOYSTICK_SMOOTHING_SAMPLES];
    int joystickYRaw[JOYSTICK_SMOOTHING_SAMPLES];
    int smoothingIndex;
    
    int joystickXSmooth;
    int joystickYSmooth;
    
    // Calibrated center values
    int joystickXCenter;
    int joystickYCenter;
    
    // Joystick zone tracking for edge detection
    JoystickZone lastXZone;
    JoystickZone lastYZone;
    
    // Track Y-axis value to detect direction changes
    int lastYValue;
    
    // Auto-repeat timing for large deflections
    unsigned long lastHorizontalRepeatTime;
    
    // Y-axis lockout to prevent bounce between up/down
    unsigned long yAxisLockoutUntil;
    
    // Helper methods
    void calibrateJoystick();
    void readJoystickAnalog();
    int getSmoothedValue(const int* samples);
    JoystickZone getZone(int value, int centerValue);
    bool isInDeadzone(int value, int centerValue);
    
public:
    InputManager();
    void init();
    void poll();
    InputState getInputState();
};

#endif // INPUT_H
