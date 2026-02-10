#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// GAME CONFIGURATION
// ============================================================================

// Playfield dimensions
constexpr int PLAYFIELD_WIDTH = 10;
constexpr int PLAYFIELD_HEIGHT = 20;
constexpr int PLAYFIELD_HIDDEN_ROWS = 2;  // Extra rows at top for spawning

// Level progression
constexpr int LINES_PER_LEVEL = 20;

// Gravity timing (milliseconds per drop at each level)
// Decreases as level increases for faster gameplay
inline unsigned long getGravityInterval(int level) {
    const unsigned long baseInterval = 800;  // Level 0
    const unsigned long minInterval = 100;   // Minimum speed cap
    const unsigned long decrease = 50;       // Decrease per level
    
    unsigned long interval = baseInterval - (level * decrease);
    return (interval < minInterval) ? minInterval : interval;
}

// Input polling
constexpr unsigned long INPUT_POLL_INTERVAL_MS = 10;  // 100 Hz

// ============================================================================
// GRAPHICS CONFIGURATION
// ============================================================================

// Display dimensions
constexpr int SCREEN_WIDTH = 240;
constexpr int SCREEN_HEIGHT = 320;

// Cell rendering
constexpr int CELL_SIZE = 12;  // Pixels per grid cell

// Playfield position (centered vertically, left-aligned)
constexpr int PLAYFIELD_OFFSET_X = 10;
constexpr int PLAYFIELD_OFFSET_Y = 30;

// UI Panel position (right side)
constexpr int UI_PANEL_X = PLAYFIELD_OFFSET_X + (PLAYFIELD_WIDTH * CELL_SIZE) + 15;
constexpr int UI_PANEL_Y = 40;

// Next piece preview
constexpr int NEXT_PIECE_CELL_SIZE = 8;

// RGB565 Colors
constexpr uint16_t COLOR_BLACK = 0x0000;
constexpr uint16_t COLOR_WHITE = 0xFFFF;
constexpr uint16_t COLOR_BACKGROUND = 0x0841;  // Dark blue
constexpr uint16_t COLOR_GRID = 0x2104;        // Darker blue for grid lines

// Tetromino colors (RGB565)
constexpr uint16_t COLOR_CYAN = 0x07FF;    // I piece
constexpr uint16_t COLOR_YELLOW = 0xFFE0;  // O piece
constexpr uint16_t COLOR_PURPLE = 0xF81F;  // T piece
constexpr uint16_t COLOR_GREEN = 0x07E0;   // S piece
constexpr uint16_t COLOR_RED = 0xF800;     // Z piece
constexpr uint16_t COLOR_BLUE = 0x001F;    // J piece
constexpr uint16_t COLOR_ORANGE = 0xFD20;  // L piece

// ============================================================================
// AUDIO CONFIGURATION
// ============================================================================

// I2S Configuration
constexpr int SAMPLE_RATE = 22050;         // Hz
constexpr int AUDIO_BUFFER_SIZE = 512;     // Samples per buffer
constexpr int AUDIO_QUEUE_SIZE = 10;       // Max queued sound events

// Sound durations (in samples)
constexpr int SOUND_DURATION_ROTATE = SAMPLE_RATE / 20;      // ~50ms
constexpr int SOUND_DURATION_PLACE = SAMPLE_RATE / 10;       // ~100ms
constexpr int SOUND_DURATION_LINE_CLEAR = SAMPLE_RATE / 5;   // ~200ms
constexpr int SOUND_DURATION_LEVEL_UP = SAMPLE_RATE / 3;     // ~333ms
constexpr int SOUND_DURATION_GAME_OVER = SAMPLE_RATE / 2;    // ~500ms

// Sound volume (amplitude, 0-32767)
constexpr int16_t SOUND_VOLUME = 16000;

// ============================================================================
// FREERTOS CONFIGURATION
// ============================================================================

// Task stack sizes (bytes)
constexpr int GAME_TASK_STACK_SIZE = 4096;
constexpr int AUDIO_TASK_STACK_SIZE = 4096;

// Task priorities
constexpr int GAME_TASK_PRIORITY = 2;
constexpr int AUDIO_TASK_PRIORITY = 3;  // Higher priority for audio

// Task core assignments
constexpr int GAME_TASK_CORE = 1;   // APP CPU
constexpr int AUDIO_TASK_CORE = 0;  // PRO CPU

#endif // CONFIG_H
