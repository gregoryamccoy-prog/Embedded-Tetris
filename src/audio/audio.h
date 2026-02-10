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
    SOUND_LINE_CLEAR,
    SOUND_LEVEL_UP,
    SOUND_GAME_OVER,
    SOUND_NONE
};

// Audio manager class
class AudioManager {
private:
    QueueHandle_t soundQueue;
    bool initialized;
    
    void generateRotateSound(int16_t* buffer, int samples);
    void generatePlaceSound(int16_t* buffer, int samples);
    void generateLineClearSound(int16_t* buffer, int samples);
    void generateLevelUpSound(int16_t* buffer, int samples);
    void generateGameOverSound(int16_t* buffer, int samples);
    void generateTone(int16_t* buffer, int samples, float frequency, int16_t amplitude);
    void generateSilence(int16_t* buffer, int samples);
    
public:
    AudioManager();
    bool init();
    void enqueueSound(SoundEvent event);
    static void audioTask(void* pvParameters);
    void processAudio();
};

// Global audio manager instance
extern AudioManager audioManager;

#endif // AUDIO_H
