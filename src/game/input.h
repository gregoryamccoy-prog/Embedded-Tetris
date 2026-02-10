#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <Bounce2.h>
#include "../hardware_pins.h"

// Input state structure - edge-triggered events
struct InputState {
    bool pausePressed;
    bool rotatePressed;
    bool rightPressed;
    bool leftPressed;
    bool hardDropPressed;
};

class InputManager {
private:
    Bounce pauseButton;
    Bounce rotateButton;
    Bounce rightButton;
    Bounce leftButton;
    Bounce hardDropButton;
    
public:
    InputManager();
    void init();
    void poll();
    InputState getInputState();
};

#endif // INPUT_H
