#include "audio.h"
#include <cmath>

AudioManager audioManager;

AudioManager::AudioManager() : soundQueue(nullptr), initialized(false) {
}

bool AudioManager::init() {
    // Create sound event queue
    soundQueue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(SoundEvent));
    if (soundQueue == nullptr) {
        Serial.println("Failed to create audio queue");
        return false;
    }
    
    // Configure I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = AUDIO_BUFFER_SIZE,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    // Configure I2S pins
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRCLK_PIN,
        .data_out_num = I2S_DATA_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    // Install and configure I2S driver
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, nullptr);
    if (err != ESP_OK) {
        Serial.println("Failed to install I2S driver");
        return false;
    }
    
    err = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (err != ESP_OK) {
        Serial.println("Failed to set I2S pins");
        return false;
    }
    
    // Start with silence
    i2s_zero_dma_buffer(I2S_NUM_0);
    
    initialized = true;
    Serial.println("Audio initialized");
    return true;
}

void AudioManager::enqueueSound(SoundEvent event) {
    if (!initialized || event == SoundEvent::SOUND_NONE) return;
    
    // Non-blocking queue send
    xQueueSend(soundQueue, &event, 0);
}

void AudioManager::generateTone(int16_t* buffer, int samples, float frequency, int16_t amplitude) {
    static float phase = 0.0f;
    float phaseIncrement = 2.0f * PI * frequency / SAMPLE_RATE;
    
    for (int i = 0; i < samples; i++) {
        buffer[i] = (int16_t)(amplitude * sin(phase));
        phase += phaseIncrement;
        if (phase >= 2.0f * PI) {
            phase -= 2.0f * PI;
        }
    }
}

void AudioManager::generateSilence(int16_t* buffer, int samples) {
    memset(buffer, 0, samples * sizeof(int16_t));
}

void AudioManager::generateRotateSound(int16_t* buffer, int samples) {
    // Short high-pitched blip
    generateTone(buffer, samples, 1000.0f, SOUND_VOLUME / 2);
}

void AudioManager::generatePlaceSound(int16_t* buffer, int samples) {
    // Lower thud sound
    generateTone(buffer, samples, 400.0f, SOUND_VOLUME);
}

void AudioManager::generateLineClearSound(int16_t* buffer, int samples) {
    // Rising arpeggio (simplified - just play middle frequency)
    generateTone(buffer, samples, 800.0f, SOUND_VOLUME);
}

void AudioManager::generateLevelUpSound(int16_t* buffer, int samples) {
    // Upbeat higher tone
    generateTone(buffer, samples, 1200.0f, SOUND_VOLUME);
}

void AudioManager::generateGameOverSound(int16_t* buffer, int samples) {
    // Descending tone
    float startFreq = 800.0f;
    float endFreq = 200.0f;
    float phase = 0.0f;
    
    for (int i = 0; i < samples; i++) {
        float t = (float)i / samples;
        float freq = startFreq + (endFreq - startFreq) * t;
        float phaseIncrement = 2.0f * PI * freq / SAMPLE_RATE;
        
        buffer[i] = (int16_t)(SOUND_VOLUME * sin(phase));
        phase += phaseIncrement;
        if (phase >= 2.0f * PI) {
            phase -= 2.0f * PI;
        }
    }
}

void AudioManager::processAudio() {
    SoundEvent event;
    
    // Check for queued sound events
    while (xQueueReceive(soundQueue, &event, 0) == pdTRUE) {
        int16_t* buffer = nullptr;
        int duration = 0;
        
        // Determine sound duration
        switch (event) {
            case SoundEvent::SOUND_ROTATE:
                duration = SOUND_DURATION_ROTATE;
                break;
            case SoundEvent::SOUND_PLACE:
                duration = SOUND_DURATION_PLACE;
                break;
            case SoundEvent::SOUND_LINE_CLEAR:
                duration = SOUND_DURATION_LINE_CLEAR;
                break;
            case SoundEvent::SOUND_LEVEL_UP:
                duration = SOUND_DURATION_LEVEL_UP;
                break;
            case SoundEvent::SOUND_GAME_OVER:
                duration = SOUND_DURATION_GAME_OVER;
                break;
            default:
                continue;
        }
        
        // Allocate buffer
        buffer = (int16_t*)malloc(duration * sizeof(int16_t));
        if (buffer == nullptr) {
            Serial.println("Failed to allocate audio buffer");
            continue;
        }
        
        // Generate sound
        switch (event) {
            case SoundEvent::SOUND_ROTATE:
                generateRotateSound(buffer, duration);
                break;
            case SoundEvent::SOUND_PLACE:
                generatePlaceSound(buffer, duration);
                break;
            case SoundEvent::SOUND_LINE_CLEAR:
                generateLineClearSound(buffer, duration);
                break;
            case SoundEvent::SOUND_LEVEL_UP:
                generateLevelUpSound(buffer, duration);
                break;
            case SoundEvent::SOUND_GAME_OVER:
                generateGameOverSound(buffer, duration);
                break;
            default:
                break;
        }
        
        // Write to I2S in chunks
        size_t totalWritten = 0;
        while (totalWritten < duration) {
            size_t toWrite = min((size_t)AUDIO_BUFFER_SIZE, duration - totalWritten);
            size_t bytesWritten = 0;
            
            i2s_write(I2S_NUM_0, 
                     buffer + totalWritten, 
                     toWrite * sizeof(int16_t), 
                     &bytesWritten, 
                     portMAX_DELAY);
            
            totalWritten += bytesWritten / sizeof(int16_t);
        }
        
        free(buffer);
    }
    
    // Small delay to prevent busy-waiting
    vTaskDelay(pdMS_TO_TICKS(10));
}

void AudioManager::audioTask(void* pvParameters) {
    AudioManager* manager = (AudioManager*)pvParameters;
    
    Serial.println("Audio task started on core " + String(xPortGetCoreID()));
    
    while (true) {
        manager->processAudio();
    }
}
