#include "input.h"

InputManager::InputManager() 
    : smoothingIndex(0)
    , joystickXSmooth(JOYSTICK_CENTER_VALUE)
    , joystickYSmooth(JOYSTICK_CENTER_VALUE)
    , joystickXCenter(JOYSTICK_CENTER_VALUE)
    , joystickYCenter(JOYSTICK_CENTER_VALUE)
    , lastXZone(JoystickZone::CENTER)
    , lastYZone(JoystickZone::CENTER)
    , lastYValue(JOYSTICK_CENTER_VALUE)
    , lastHorizontalRepeatTime(0)
    , yAxisLockoutUntil(0)
{
    // Initialize smoothing arrays with center values
    for (int i = 0; i < JOYSTICK_SMOOTHING_SAMPLES; i++) {
        joystickXRaw[i] = JOYSTICK_CENTER_VALUE;
        joystickYRaw[i] = JOYSTICK_CENTER_VALUE;
    }
}

void InputManager::init() {
    // Configure button pins with internal pull-ups
    pinMode(BTN_PAUSE_PIN, INPUT_PULLUP);
    pinMode(BTN_ROTATE_PIN, INPUT_PULLUP);
    pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
    pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
    pinMode(BTN_HARD_DROP_PIN, INPUT_PULLUP);
    pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
    
    // Configure joystick analog pins (ADC1)
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    
    // Set ADC resolution to 12-bit (0-4095)
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);  // Full range 0-3.3V
    
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
    
    joystickSwButton.attach(JOYSTICK_SW_PIN);
    joystickSwButton.interval(10);
    
    // Calibrate joystick center position
    calibrateJoystick();
}

void InputManager::calibrateJoystick() {
    // Give user time to release joystick before calibration
    Serial.println("Calibrating joystick - please do not touch...");
    delay(500);  // Wait 500ms for joystick to settle
    
    // Read joystick position multiple times and average to get true center
    const int calibrationSamples = 20;  // More samples for better accuracy
    long xSum = 0;
    long ySum = 0;
    
    // Take multiple readings with small delay
    // Read Y-axis as-is (no inversion)
    for (int i = 0; i < calibrationSamples; i++) {
        xSum += analogRead(JOYSTICK_X_PIN);
        ySum += analogRead(JOYSTICK_Y_PIN);
        delay(10);  // 10ms delay between readings for stability
    }
    
    // Calculate average center values
    joystickXCenter = xSum / calibrationSamples;
    joystickYCenter = ySum / calibrationSamples;
    
    // Initialize smoothing arrays with calibrated center
    for (int i = 0; i < JOYSTICK_SMOOTHING_SAMPLES; i++) {
        joystickXRaw[i] = joystickXCenter;
        joystickYRaw[i] = joystickYCenter;
    }
    joystickXSmooth = joystickXCenter;
    joystickYSmooth = joystickYCenter;
    lastYValue = joystickYCenter;
    
    Serial.print("Calibration complete - X center: ");
    Serial.print(joystickXCenter);
    Serial.print(", Y center: ");
    Serial.println(joystickYCenter);
}

void InputManager::poll() {
    // Update all button states
    pauseButton.update();
    rotateButton.update();
    rightButton.update();
    leftButton.update();
    hardDropButton.update();
    joystickSwButton.update();
    
    // Update joystick analog readings
    readJoystickAnalog();
}

void InputManager::readJoystickAnalog() {
    // Read raw analog values (no Y-axis inversion)
    joystickXRaw[smoothingIndex] = analogRead(JOYSTICK_X_PIN);
    joystickYRaw[smoothingIndex] = analogRead(JOYSTICK_Y_PIN);
    
    // Move to next smoothing sample index
    smoothingIndex = (smoothingIndex + 1) % JOYSTICK_SMOOTHING_SAMPLES;
    
    // Calculate smoothed values
    joystickXSmooth = getSmoothedValue(joystickXRaw);
    joystickYSmooth = getSmoothedValue(joystickYRaw);
}

int InputManager::getSmoothedValue(const int* samples) {
    long sum = 0;
    for (int i = 0; i < JOYSTICK_SMOOTHING_SAMPLES; i++) {
        sum += samples[i];
    }
    return sum / JOYSTICK_SMOOTHING_SAMPLES;
}

JoystickZone InputManager::getZone(int value, int centerValue) {
    int diff = abs(value - centerValue);
    
    if (diff < JOYSTICK_DEADZONE) {
        return JoystickZone::CENTER;
    } else if (diff < JOYSTICK_LARGE_THRESHOLD) {
        return JoystickZone::SMALL_DEFLECTION;
    } else {
        return JoystickZone::LARGE_DEFLECTION;
    }
}

bool InputManager::isInDeadzone(int value, int centerValue) {
    return abs(value - centerValue) < JOYSTICK_DEADZONE;
}

InputState InputManager::getInputState() {
    InputState state;
    
    // Initialize all to false
    state.pausePressed = false;
    state.rotatePressed = false;
    state.rightPressed = false;
    state.leftPressed = false;
    state.hardDropPressed = false;
    
    // ========================================================================
    // BUTTON INPUT (edge-triggered)
    // ========================================================================
    
    // Detect falling edge (button press) - buttons are active low
    if (pauseButton.fell()) state.pausePressed = true;
    if (rotateButton.fell()) state.rotatePressed = true;
    if (rightButton.fell()) state.rightPressed = true;
    if (leftButton.fell()) state.leftPressed = true;
    if (hardDropButton.fell()) state.hardDropPressed = true;
    
    // ========================================================================
    // JOYSTICK INPUT (analog axes)
    // ========================================================================
    
    // Get current zones using calibrated center values
    JoystickZone currentXZone = getZone(joystickXSmooth, joystickXCenter);
    JoystickZone currentYZone = getZone(joystickYSmooth, joystickYCenter);
    
    unsigned long currentTime = millis();
    
    // Debug - continuous Y monitoring
    static unsigned long lastDebug = 0;
    if (currentTime - lastDebug > 100) {
        int yDiff = joystickYSmooth - joystickYCenter;
        int xDiff = joystickXSmooth - joystickXCenter;
        Serial.print("X: ");
        Serial.print(joystickXSmooth);
        Serial.print(" Y: ");
        Serial.print(joystickYSmooth);
        Serial.print(" | X_CTR: ");
        Serial.print(joystickXCenter);
        Serial.print(" Y_CTR: ");
        Serial.print(joystickYCenter);
        Serial.print(" | X_DIFF: ");
        Serial.print(xDiff);
        Serial.print(" Y_DIFF: ");
        Serial.print(yDiff);
        Serial.print(" | Y_ZONE: ");
        Serial.println(currentYZone == JoystickZone::CENTER ? "C" : 
                      currentYZone == JoystickZone::SMALL_DEFLECTION ? "S" : "L");
        lastDebug = currentTime;
    }
    
    // ------------------------------------------------------------------------
    // HORIZONTAL MOVEMENT (X-axis)
    // ------------------------------------------------------------------------
    
    // Small deflection: trigger on entry into zone
    if (currentXZone == JoystickZone::SMALL_DEFLECTION && 
        lastXZone == JoystickZone::CENTER) {
        
        // Determine direction from current position relative to center
        if (joystickXSmooth < joystickXCenter) {
            state.leftPressed = true;
        } else {
            state.rightPressed = true;
        }
        
        lastHorizontalRepeatTime = currentTime;
    }
    
    // Large deflection: trigger on entry + auto-repeat
    if (currentXZone == JoystickZone::LARGE_DEFLECTION) {
        // Trigger on entry from smaller zone
        if (lastXZone != JoystickZone::LARGE_DEFLECTION) {
            
            if (joystickXSmooth < joystickXCenter) {
                state.leftPressed = true;
            } else {
                state.rightPressed = true;
            }
            
            lastHorizontalRepeatTime = currentTime;
        }
        // Auto-repeat after interval
        else if (currentTime - lastHorizontalRepeatTime >= JOYSTICK_REPEAT_INTERVAL_MS) {
            if (joystickXSmooth < joystickXCenter) {
                state.leftPressed = true;
            } else {
                state.rightPressed = true;
            }
            
            lastHorizontalRepeatTime = currentTime;
        }
    }
    
    // ------------------------------------------------------------------------
    // VERTICAL MOVEMENT (Y-axis) - STRICT EDGE DETECTION
    // ------------------------------------------------------------------------
    
    // Only process Y-axis if lockout period has expired
    if (currentTime >= yAxisLockoutUntil) {
        
        // Only trigger on LARGE deflection (require strong intentional movement)
        if (currentYZone == JoystickZone::LARGE_DEFLECTION && 
            lastYZone != JoystickZone::LARGE_DEFLECTION) {
            
            // Calculate direction and magnitude
            int yDiff = joystickYSmooth - joystickYCenter;
            int yDiffAbs = abs(yDiff);
            int xDiff = abs(joystickXSmooth - joystickXCenter);
            
            // Only trigger Y-axis action if Y movement is dominant
            // Y must be larger than X movement to avoid crosstalk
            if (yDiffAbs > xDiff) {
                // Check direction: positive yDiff = up, negative yDiff = down
                if (yDiff > 0) {
                    // Up = Rotate
                    state.rotatePressed = true;
                    yAxisLockoutUntil = currentTime + JOYSTICK_Y_LOCKOUT_MS;
                    Serial.println(">>> ROTATE (up)");
                } else {
                    // Down = Hard Drop
                    state.hardDropPressed = true;
                    yAxisLockoutUntil = currentTime + JOYSTICK_Y_LOCKOUT_MS;
                    Serial.println(">>> HARD DROP (down)");
                }
            }
        }
    }
    
    // Update tracked state for next poll
    lastXZone = currentXZone;
    lastYZone = currentYZone;
    lastYValue = joystickYSmooth;
    
    return state;
}
