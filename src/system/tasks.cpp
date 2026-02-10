#include "tasks.h"

// Task handles
TaskHandle_t gameTaskHandle = nullptr;
TaskHandle_t audioTaskHandle = nullptr;

// Shared instances
GameState gameState;
Display display;
InputManager inputManager;

void gameTask(void* pvParameters) {
    Serial.println("Game task started on core " + String(xPortGetCoreID()));
    
    // Initialize gravity timer
    SimpleTimer gravityTimer;
    gravityTimer.start();
    
    // Initialize input poll timer
    SimpleTimer inputTimer;
    inputTimer.start();
    
    // Track if display needs update
    bool needsDisplayUpdate = false;
    
    // Game loop
    while (true) {
        // Poll input at regular intervals
        if (inputTimer.expired(INPUT_POLL_INTERVAL_MS)) {
            inputManager.poll();
            inputTimer.reset();
            
            InputState input = inputManager.getInputState();
            
            // Handle pause button
            if (input.pausePressed) {
                togglePause(gameState);
                
                if (gameState.status == GameStatus::PAUSED) {
                    display.drawPauseOverlay();
                } else if (gameState.status == GameStatus::PLAYING) {
                    // Redraw when unpausing
                    display.fullRedraw(gameState);
                    needsDisplayUpdate = false;  // Already updated
                }
            }
            
            // Handle gameplay inputs (only when playing)
            if (gameState.status == GameStatus::PLAYING) {
                // Rotate (left)
                if (input.rotatePressed) {
                    if (rotatePieceLeft(gameState)) {
                        audioManager.enqueueSound(SoundEvent::SOUND_ROTATE);
                        needsDisplayUpdate = true;
                    }
                }
                
                // Move right
                if (input.rightPressed) {
                    if (movePieceRight(gameState)) {
                        needsDisplayUpdate = true;
                    }
                }
                
                // Move left
                if (input.leftPressed) {
                    if (movePieceLeft(gameState)) {
                        needsDisplayUpdate = true;
                    }
                }
                
                // Hard drop
                if (input.hardDropPressed) {
                    hardDrop(gameState);
                    audioManager.enqueueSound(SoundEvent::SOUND_PLACE);
                    needsDisplayUpdate = true;
                    
                    // Check for line clears after drop
                    if (gameState.linesCleared > 0) {
                        int prevLines = gameState.linesCleared;
                        int prevLevel = gameState.level;
                        
                        // Line clearing already happened in hardDrop->lockPiece
                        // Just trigger sounds
                        audioManager.enqueueSound(SoundEvent::SOUND_LINE_CLEAR);
                        
                        if (gameState.level > prevLevel) {
                            audioManager.enqueueSound(SoundEvent::SOUND_LEVEL_UP);
                        }
                    }
                    
                    // Check for game over
                    if (isGameOver(gameState)) {
                        audioManager.enqueueSound(SoundEvent::SOUND_GAME_OVER);
                        display.drawGameOver(gameState);
                    }
                }
            }
        }
        
        // Handle gravity (only when playing)
        if (gameState.status == GameStatus::PLAYING) {
            unsigned long gravityInterval = getGravityInterval(gameState.level);
            
            if (gravityTimer.expired(gravityInterval)) {
                bool moved = stepGravity(gameState);
                needsDisplayUpdate = true;
                
                if (!moved) {
                    // Piece was locked
                    audioManager.enqueueSound(SoundEvent::SOUND_PLACE);
                    
                    // Check if lines were cleared in stepGravity
                    static int lastLinesCleared = 0;
                    static int lastLevel = 0;
                    
                    if (gameState.linesCleared > lastLinesCleared) {
                        audioManager.enqueueSound(SoundEvent::SOUND_LINE_CLEAR);
                        lastLinesCleared = gameState.linesCleared;
                    }
                    
                    if (gameState.level > lastLevel) {
                        audioManager.enqueueSound(SoundEvent::SOUND_LEVEL_UP);
                        lastLevel = gameState.level;
                    }
                    
                    // Check for game over
                    if (isGameOver(gameState)) {
                        audioManager.enqueueSound(SoundEvent::SOUND_GAME_OVER);
                        display.drawGameOver(gameState);
                    }
                }
                
                gravityTimer.reset();
            }
        }
        
        // Update display only when needed
        if (gameState.status == GameStatus::PLAYING && needsDisplayUpdate) {
            display.drawPlayfield(gameState);
            display.drawCurrentPiece(gameState);
            display.drawNextPiece(gameState.nextPiece);
            display.drawStats(gameState.level, gameState.linesCleared, gameState.score);
            needsDisplayUpdate = false;
        }
        
        // Small delay to prevent watchdog triggers
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void createSystemTasks() {
    // Create game task on Core 1 (APP CPU)
    xTaskCreatePinnedToCore(
        gameTask,
        "GameTask",
        GAME_TASK_STACK_SIZE,
        nullptr,
        GAME_TASK_PRIORITY,
        &gameTaskHandle,
        GAME_TASK_CORE
    );
    
    // Create audio task on Core 0 (PRO CPU)
    xTaskCreatePinnedToCore(
        AudioManager::audioTask,
        "AudioTask",
        AUDIO_TASK_STACK_SIZE,
        &audioManager,
        AUDIO_TASK_PRIORITY,
        &audioTaskHandle,
        AUDIO_TASK_CORE
    );
    
    Serial.println("System tasks created");
}
