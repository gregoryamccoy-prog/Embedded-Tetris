#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <driver/i2s.h>
#include "../config.h"
#include "../hardware_pins.h"

// Sound event types
enum class SoundEvent {
    SOUND_ROTATE,
    SOUND_PLACE,
    SOUND_LINE_CLEAR_1,   // 1 row cleared – rising sweep
    SOUND_LINE_CLEAR_2,   // 2 rows – two-note arpeggio
    SOUND_LINE_CLEAR_3,   // 3 rows – three-note arpeggio
    SOUND_LINE_CLEAR_4,   // 4 rows – "Tetris!" fanfare
    SOUND_LEVEL_UP,
    SOUND_GAME_OVER,
    SOUND_NONE
};

// A single note in the background music melody.
// frequency == 0.0 is a rest.
struct MusicNote {
    float frequency;  // Hz (0 = rest)
    int   duration;   // samples
};

// Audio manager class
class AudioManager {
private:
    QueueHandle_t soundQueue;
    bool initialized;

    // ---- background-music state ----
    bool  musicEnabled;
    bool  musicPaused;
    int   musicTrack;        // 0 = Theme A (Korobeiniki), 1 = Theme B (Romance)
    int   musicNoteIndex;    // index into active theme table
    int   musicSampleOffset; // how many samples of current note played so far
    float musicPhase;        // continuous sine phase for the current note

    // Sound-effect generators
    void generateRotateSound(int16_t* buffer, int samples);
    void generatePlaceSound(int16_t* buffer, int samples);
    void generateLineClear1Sound(int16_t* buffer, int samples);  // 1 row: rising sweep
    void generateLineClear2Sound(int16_t* buffer, int samples);  // 2 rows: 2-note arpeggio
    void generateLineClear3Sound(int16_t* buffer, int samples);  // 3 rows: 3-note arpeggio
    void generateLineClear4Sound(int16_t* buffer, int samples);  // 4 rows: fanfare + sweep
    void generateLevelUpSound(int16_t* buffer, int samples);
    void generateGameOverSound(int16_t* buffer, int samples);
    void generateTone(int16_t* buffer, int samples, float frequency, int16_t amplitude);
    void generateSilence(int16_t* buffer, int samples);

    // Plays a complete sound-effect buffer to I2S (blocking)
    void playSoundEffect(SoundEvent event);

    // Writes the next AUDIO_BUFFER_SIZE music samples to I2S
    void advanceMusicChunk();

public:
    AudioManager();
    bool init();
    void enqueueSound(SoundEvent event);

    // Background music control
    void startMusic();                 // enable and reset to beginning (Track A)
    void stopMusic();                  // disable (writes silence)
    void pauseMusic();                 // freeze position
    void resumeMusic();                // continue from frozen position
    void setLinesCleared(int total);   // call on each line clear; switches A↔B every 10 lines

    static void audioTask(void* pvParameters);
    void processAudio();
};

// Global audio manager instance
extern AudioManager audioManager;

#endif // AUDIO_H
