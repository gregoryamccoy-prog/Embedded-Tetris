#include <Arduino.h>
#include "config.h"
#include "hardware_pins.h"
#include "display/display.h"
#include "game/game_state.h"
#include "game/input.h"
#include "audio/audio.h"
#include "system/tasks.h"

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=================================");
    Serial.println("ESP32 Tetris Game Starting...");
    Serial.println("=================================");
    
    // Initialize random seed
    randomSeed(analogRead(0));
    
    // Initialize display
    Serial.println("Initializing display...");
    display.init();
    Serial.println("Display initialized");
    
    // Initialize input
    Serial.println("Initializing input...");
    inputManager.init();
    Serial.println("Input initialized");
    
    // Initialize audio
    Serial.println("Initializing audio...");
    if (!audioManager.init()) {
        Serial.println("WARNING: Audio initialization failed!");
    }
    
    // Initialize game state
    Serial.println("Initializing game...");
    initGame(gameState);
    Serial.println("Game initialized");
    
    // Initial full display draw
    display.fullRedraw(gameState);
    
    // Create FreeRTOS tasks
    Serial.println("Creating system tasks...");
    createSystemTasks();
    
    Serial.println("=================================");
    Serial.println("Setup complete! Game starting...");
    Serial.println("=================================");
    Serial.println();
    Serial.println("Controls:");
    Serial.println("  - GPIO32: Pause/Resume");
    Serial.println("  - GPIO33: Rotate Left");
    Serial.println("  - GPIO27: Rotate Right");
    Serial.println("  - GPIO14: Hard Drop");
    Serial.println();
}

void loop() {
    // Empty - all work done in FreeRTOS tasks
    // Keep this loop alive for watchdog
    vTaskDelay(pdMS_TO_TICKS(1000));
}
