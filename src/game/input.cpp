#include "input.h"

InputManager::InputManager() {
    // Constructor - buttons will be initialized in init()
}

void InputManager::init() {
    // Configure button pins with internal pull-ups
    pinMode(BTN_PAUSE_PIN, INPUT_PULLUP);
    pinMode(BTN_ROTATE_PIN, INPUT_PULLUP);
    pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
    pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
    pinMode(BTN_HARD_DROP_PIN, INPUT_PULLUP);
    
    // Attach debounce objects to pins
    pauseButton.attach(BTN_PAUSE_PIN);
    pauseButton.interval(10);  // 10ms debounce
    
    rotateButton.attach(BTN_ROTATE_PIN);
    rotateButton.interval(10);
    
    rightButton.attach(BTN_RIGHT_PIN);
    rightButton.interval(10);
    
    leftButton.attach(BTN_LEFT_PIN);
    leftButton.interval(10);
    
    hardDropButton.attach(BTN_HARD_DROP_PIN);
    hardDropButton.interval(10);
}

void InputManager::poll() {
    // Update all button states
    pauseButton.update();
    rotateButton.update();
    rightButton.update();
    leftButton.update();
    hardDropButton.update();
}

InputState InputManager::getInputState() {
    InputState state;
    
    // Detect falling edge (button press) - buttons are active low
    state.pausePressed = pauseButton.fell();
    state.rotatePressed = rotateButton.fell();
    state.rightPressed = rightButton.fell();
    state.leftPressed = leftButton.fell();
    state.hardDropPressed = hardDropButton.fell();
    
    return state;
}
