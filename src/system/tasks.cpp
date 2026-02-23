#include "tasks.h"

// Task handles
TaskHandle_t gameTaskHandle = nullptr;
TaskHandle_t audioTaskHandle = nullptr;

// Shared instances
GameState gameState;
Display display;
InputManager inputManager;

// Returns the appropriate line-clear SFX event for the number of rows cleared
static SoundEvent lineClearSfx(int delta) {
    if (delta >= 4) return SoundEvent::SOUND_LINE_CLEAR_4;
    if (delta == 3) return SoundEvent::SOUND_LINE_CLEAR_3;
    if (delta == 2) return SoundEvent::SOUND_LINE_CLEAR_2;
    return SoundEvent::SOUND_LINE_CLEAR_1;
}

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

    // Start background music
    audioManager.startMusic();

    // Track line-clear & level totals for delta computation
    int lastLinesCleared = 0;
    int lastLevel        = 0;

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
                    audioManager.pauseMusic();
                    display.drawPauseOverlay();
                } else if (gameState.status == GameStatus::PLAYING) {
                    audioManager.resumeMusic();
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
                    // Capture totals BEFORE the drop
                    int linesBefore = gameState.linesCleared;
                    int levelBefore = gameState.level;

                    hardDrop(gameState);
                    audioManager.enqueueSound(SoundEvent::SOUND_PLACE);
                    needsDisplayUpdate = true;

                    int delta = gameState.linesCleared - linesBefore;
                    if (delta > 0) {
                        audioManager.enqueueSound(lineClearSfx(delta));
                        lastLinesCleared = gameState.linesCleared;
                        audioManager.setLinesCleared(gameState.linesCleared);
                    }
                    if (gameState.level > levelBefore) {
                        audioManager.enqueueSound(SoundEvent::SOUND_LEVEL_UP);
                        lastLevel = gameState.level;
                    }

                    // Check for game over
                    if (isGameOver(gameState)) {
                        audioManager.stopMusic();
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

                    int delta = gameState.linesCleared - lastLinesCleared;
                    if (delta > 0) {
                        audioManager.enqueueSound(lineClearSfx(delta));
                        lastLinesCleared = gameState.linesCleared;
                        audioManager.setLinesCleared(gameState.linesCleared);
                    }
                    if (gameState.level > lastLevel) {
                        audioManager.enqueueSound(SoundEvent::SOUND_LEVEL_UP);
                        lastLevel = gameState.level;
                    }
                    
                    // Check for game over
                    if (isGameOver(gameState)) {
                        audioManager.stopMusic();
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
