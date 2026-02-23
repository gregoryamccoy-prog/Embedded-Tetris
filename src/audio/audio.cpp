#include "audio.h"
#include <cmath>

// ============================================================================
// Tetris Theme A (Korobeiniki) – gameboy version, 149 BPM
// Transcription: 16 measures, key A-minor / C-major, AABB structure.
// ============================================================================

// Convenience aliases for Theme A note durations (defined in config.h)
#define Q  MUSIC_NOTE_Q
#define E  MUSIC_NOTE_E
#define DQ MUSIC_NOTE_DQ
#define H  MUSIC_NOTE_H

// Note frequencies (Hz) – shared between both themes
static constexpr float G4  = 392.00f;
static constexpr float nA4 = 440.00f;
static constexpr float B4  = 493.88f;
static constexpr float C5  = 523.25f;
static constexpr float D5  = 587.33f;
static constexpr float E5  = 659.25f;
static constexpr float F5  = 698.46f;
static constexpr float G5  = 783.99f;
static constexpr float nA5 = 880.00f;
static constexpr float REST = 0.0f;

// Full melody table – 77 notes, loops seamlessly
static const MusicNote TETRIS_THEME_A[] = {
    // ---- Part A, measures 1-4 ----
    {E5,Q},  {B4,E},  {C5,E},  {D5,Q}, {C5,E}, {B4,E},   // M1
    {nA4,Q}, {nA4,E}, {C5,E},  {E5,Q}, {D5,E}, {C5,E},   // M2
    {B4,DQ}, {C5,E},  {D5,Q},  {E5,Q},                   // M3
    {C5,Q},  {nA4,Q}, {nA4,H},                           // M4

    // ---- Part A, measures 5-8 (repeat) ----
    {E5,Q},  {B4,E},  {C5,E},  {D5,Q}, {C5,E}, {B4,E},   // M5
    {nA4,Q}, {nA4,E}, {C5,E},  {E5,Q}, {D5,E}, {C5,E},   // M6
    {B4,DQ}, {C5,E},  {D5,Q},  {E5,Q},                   // M7
    {C5,Q},  {nA4,Q}, {nA4,H},                           // M8

    // ---- Part B, measures 9-12 ----
    {D5,Q},  {D5,E},  {F5,E},  {nA5,Q}, {G5,E}, {F5,E}, // M9
    {E5,DQ}, {C5,E},  {E5,Q},  {D5,E},  {C5,E},         // M10
    {B4,Q},  {B4,E},  {C5,E},  {D5,Q},  {E5,Q},         // M11
    {C5,Q},  {nA4,Q}, {nA4,Q}, {REST,Q},                // M12

    // ---- Part B, measures 13-16 (repeat) ----
    {D5,Q},  {D5,E},  {F5,E},  {nA5,Q}, {G5,E}, {F5,E}, // M13
    {E5,DQ}, {C5,E},  {E5,Q},  {D5,E},  {C5,E},         // M14
    {B4,Q},  {B4,E},  {C5,E},  {D5,Q},  {E5,Q},         // M15
    {C5,Q},  {nA4,Q}, {nA4,H},                          // M16
};

static constexpr int TETRIS_THEME_A_SIZE =
    (int)(sizeof(TETRIS_THEME_A) / sizeof(TETRIS_THEME_A[0]));

#undef Q
#undef E
#undef DQ
#undef H

// ============================================================================
// Tetris Theme C – Gameboy arr. of Bach BWV 814 (French Suite No.3 Menuet)
// Key: A major / F# minor  |  3/4  |  ~100 BPM  |  27 measures  |  AAB form
// Section A = 8 bars (repeated), Section B = 11 bars.
// ============================================================================

// Extra frequency constants needed for A major (not used in Theme A)
static constexpr float Cs5 = 554.37f;  // C#5
static constexpr float Fs5 = 739.99f;  // F#5
static constexpr float Gs5 = 830.61f;  // G#5

#define QB  MUSIC_B_NOTE_Q    // quarter   (~600 ms at 100 BPM)
#define HB  MUSIC_B_NOTE_H    // half
#define DHB MUSIC_B_NOTE_DH   // dotted half = full 3/4 bar

static const MusicNote TETRIS_THEME_B[] = {
    // ---- Section A (measures 1-8) ----
    {nA4,QB}, {B4,QB},  {Cs5,QB},  // M1  A-major scale up
    {D5,HB},  {Cs5,QB},            // M2  step back
    {B4,QB},  {Cs5,QB}, {D5,QB},   // M3  stepwise up
    {E5,DHB},                      // M4  dominant, held
    {Cs5,QB}, {B4,QB},  {nA4,QB},  // M5  scale back down
    {Gs5,QB}, {Fs5,QB}, {E5,QB},   // M6  descend through raised 7th
    {D5,QB},  {Cs5,QB}, {B4,QB},   // M7  continue descent
    {nA4,DHB},                     // M8  tonic, full bar

    // ---- Section A repeat (measures 9-16) ----
    {nA4,QB}, {B4,QB},  {Cs5,QB},  // M9
    {D5,HB},  {Cs5,QB},            // M10
    {B4,QB},  {Cs5,QB}, {D5,QB},   // M11
    {E5,DHB},                      // M12
    {Cs5,QB}, {B4,QB},  {nA4,QB},  // M13
    {Gs5,QB}, {Fs5,QB}, {E5,QB},   // M14
    {D5,QB},  {Cs5,QB}, {B4,QB},   // M15
    {nA4,DHB},                     // M16

    // ---- Section B (measures 17-27) ----
    {Fs5,QB}, {E5,QB},  {D5,QB},   // M17 higher register entry
    {Cs5,HB}, {B4,QB},             // M18
    {Gs5,QB}, {Fs5,QB}, {E5,QB},   // M19 raised 6th region
    {D5,DHB},                      // M20
    {E5,QB},  {Fs5,QB}, {Gs5,QB},  // M21 ascending toward climax
    {nA5,DHB},                     // M22 peak – A5 octave
    {Gs5,QB}, {Fs5,QB}, {E5,QB},   // M23 come down
    {D5,HB},  {Cs5,QB},            // M24
    {B4,QB},  {Cs5,QB}, {D5,QB},   // M25
    {E5,HB},  {Cs5,QB},            // M26
    {nA4,DHB},                     // M27 resolve to tonic, loop
};

static constexpr int TETRIS_THEME_B_SIZE =
    (int)(sizeof(TETRIS_THEME_B) / sizeof(TETRIS_THEME_B[0]));

#undef QB
#undef HB
#undef DHB

// ============================================================================
// AudioManager implementation
// ============================================================================

AudioManager audioManager;

AudioManager::AudioManager()
    : soundQueue(nullptr),
      initialized(false),
      musicEnabled(false),
      musicPaused(false),
      musicTrack(0),
      musicNoteIndex(0),
      musicSampleOffset(0),
      musicPhase(0.0f) {
}

bool AudioManager::init() {
    soundQueue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(SoundEvent));
    if (soundQueue == nullptr) {
        Serial.println("Failed to create audio queue");
        return false;
    }

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

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRCLK_PIN,
        .data_out_num = I2S_DATA_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

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

    i2s_zero_dma_buffer(I2S_NUM_0);

    initialized = true;
    Serial.println("Audio initialized");
    return true;
}

void AudioManager::enqueueSound(SoundEvent event) {
    if (!initialized || event == SoundEvent::SOUND_NONE) return;
    xQueueSend(soundQueue, &event, 0);
}

// ---- Music control --------------------------------------------------------

void AudioManager::startMusic() {
    musicTrack        = 0;
    musicNoteIndex    = 0;
    musicSampleOffset = 0;
    musicPhase        = 0.0f;
    musicPaused       = false;
    musicEnabled      = true;
}

void AudioManager::stopMusic() {
    musicEnabled = false;
    musicPaused  = false;
}

void AudioManager::pauseMusic() {
    musicPaused = true;
}

void AudioManager::resumeMusic() {
    musicPaused = false;
}

// Switch between Theme A and Theme B every MUSIC_TRACK_SWITCH_LINES lines.
// Called from the game task whenever linesCleared changes.
void AudioManager::setLinesCleared(int total) {
    if (!musicEnabled) return;
    int newTrack = (total / MUSIC_TRACK_SWITCH_LINES) % 2;
    if (newTrack != musicTrack) {
        musicTrack        = newTrack;
        musicNoteIndex    = 0;
        musicSampleOffset = 0;
        musicPhase        = 0.0f;
        Serial.println(newTrack == 0 ? "Music: Theme A" : "Music: Theme B");
    }
}

// ---- Tone / silence generators --------------------------------------------

void AudioManager::generateTone(int16_t* buffer, int samples,
                                 float frequency, int16_t amplitude) {
    static float phase = 0.0f;
    float phaseInc = 2.0f * PI * frequency / SAMPLE_RATE;
    for (int i = 0; i < samples; i++) {
        buffer[i]  = (int16_t)(amplitude * sinf(phase));
        phase     += phaseInc;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}

void AudioManager::generateSilence(int16_t* buffer, int samples) {
    memset(buffer, 0, samples * sizeof(int16_t));
}

// ---- Static sample-filling helper ----------------------------------------
// Writes a pure sine tone into buf[start..end); phase always starts at 0.
static void fillSegment(int16_t* buf, int start, int end,
                         float freq, int16_t amp) {
    float phase    = 0.0f;
    float phaseInc = 2.0f * PI * freq / SAMPLE_RATE;
    for (int i = start; i < end; i++) {
        buf[i]  = (int16_t)(amp * sinf(phase));
        phase  += phaseInc;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}

void AudioManager::generateRotateSound(int16_t* buffer, int samples) {
    generateTone(buffer, samples, 1000.0f, SOUND_VOLUME / 2);
}

void AudioManager::generatePlaceSound(int16_t* buffer, int samples) {
    generateTone(buffer, samples, 400.0f, SOUND_VOLUME);
}

// 1 row cleared – smooth rising frequency sweep (400 → 1100 Hz)
void AudioManager::generateLineClear1Sound(int16_t* buffer, int samples) {
    float phase = 0.0f;
    for (int i = 0; i < samples; i++) {
        float t    = (float)i / samples;
        float freq = 400.0f + 700.0f * t;
        buffer[i]  = (int16_t)(SOUND_VOLUME * sinf(phase));
        phase     += 2.0f * PI * freq / SAMPLE_RATE;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}

// 2 rows cleared – two-note ascending arpeggio  E5 → A5  (with gap)
void AudioManager::generateLineClear2Sound(int16_t* buffer, int samples) {
    //  0..130ms  E5 (659 Hz)
    //  130..150ms  silence
    //  150ms..end  A5 (880 Hz)
    const int e1 = SAMPLE_RATE * 130 / 1000;
    const int s2 = SAMPLE_RATE * 150 / 1000;
    fillSegment(buffer, 0,  e1,    659.25f, SOUND_VOLUME);
    memset(buffer + e1, 0, (s2 - e1) * sizeof(int16_t));
    fillSegment(buffer, s2, samples, 880.00f, SOUND_VOLUME);
}

// 3 rows cleared – three-note ascending arpeggio  C5 → E5 → A5  (with gaps)
void AudioManager::generateLineClear3Sound(int16_t* buffer, int samples) {
    //  0..100ms    C5 (523 Hz)
    //  100..115ms  silence
    //  115..215ms  E5 (659 Hz)
    //  215..230ms  silence
    //  230ms..end  A5 (880 Hz)  — the longest note, rings out
    const int e1 = SAMPLE_RATE * 100 / 1000;
    const int s2 = SAMPLE_RATE * 115 / 1000;
    const int e2 = SAMPLE_RATE * 215 / 1000;
    const int s3 = SAMPLE_RATE * 230 / 1000;
    fillSegment(buffer, 0,  e1,      523.25f, SOUND_VOLUME);
    memset(buffer + e1, 0, (s2 - e1) * sizeof(int16_t));
    fillSegment(buffer, s2, e2,      659.25f, SOUND_VOLUME);
    memset(buffer + e2, 0, (s3 - e2) * sizeof(int16_t));
    fillSegment(buffer, s3, samples, 880.00f, SOUND_VOLUME);
}

// 4 rows cleared – "Tetris!" full fanfare:  C5 → E5 → G5 → C6 (boosted),
//                                           then a rapid rising sweep to 2 kHz
void AudioManager::generateLineClear4Sound(int16_t* buffer, int samples) {
    //  0..80ms     C5 (523 Hz)
    //  80..90ms    silence
    //  90..170ms   E5 (659 Hz)
    //  170..180ms  silence
    //  180..260ms  G5 (784 Hz)
    //  260..270ms  silence
    //  270..490ms  C6 (1047 Hz)  louder
    //  490..500ms  silence
    //  500ms..end  sweep C6 → 2000 Hz  (louder)
    const int16_t bigVol = (int16_t)min((int)SOUND_VOLUME * 5 / 4, 32767);
    const int e1 = SAMPLE_RATE * 80  / 1000;
    const int s2 = SAMPLE_RATE * 90  / 1000;
    const int e2 = SAMPLE_RATE * 170 / 1000;
    const int s3 = SAMPLE_RATE * 180 / 1000;
    const int e3 = SAMPLE_RATE * 260 / 1000;
    const int s4 = SAMPLE_RATE * 270 / 1000;
    const int e4 = SAMPLE_RATE * 490 / 1000;
    const int s5 = SAMPLE_RATE * 500 / 1000;
    fillSegment(buffer, 0,   e1,      523.25f, SOUND_VOLUME);
    memset(buffer + e1, 0, (s2 - e1) * sizeof(int16_t));
    fillSegment(buffer, s2,  e2,      659.25f, SOUND_VOLUME);
    memset(buffer + e2, 0, (s3 - e2) * sizeof(int16_t));
    fillSegment(buffer, s3,  e3,      783.99f, SOUND_VOLUME);
    memset(buffer + e3, 0, (s4 - e3) * sizeof(int16_t));
    fillSegment(buffer, s4,  e4,     1046.50f, bigVol);
    memset(buffer + e4, 0, (s5 - e4) * sizeof(int16_t));
    // Rising sweep for remaining samples
    float phase = 0.0f;
    for (int i = s5; i < samples; i++) {
        float t    = (float)(i - s5) / (samples - s5);
        float freq = 1046.50f + 953.50f * t;   // 1047 Hz → 2000 Hz
        buffer[i]  = (int16_t)(bigVol * sinf(phase));
        phase     += 2.0f * PI * freq / SAMPLE_RATE;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}

void AudioManager::generateLevelUpSound(int16_t* buffer, int samples) {
    generateTone(buffer, samples, 1200.0f, SOUND_VOLUME);
}

void AudioManager::generateGameOverSound(int16_t* buffer, int samples) {
    float startFreq = 800.0f;
    float endFreq   = 200.0f;
    float phase     = 0.0f;
    for (int i = 0; i < samples; i++) {
        float t    = (float)i / samples;
        float freq = startFreq + (endFreq - startFreq) * t;
        float phaseInc = 2.0f * PI * freq / SAMPLE_RATE;
        buffer[i]  = (int16_t)(SOUND_VOLUME * sinf(phase));
        phase     += phaseInc;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}

// ---- Sound-effect playback (extracted helper) -----------------------------

void AudioManager::playSoundEffect(SoundEvent event) {
    int duration = 0;
    switch (event) {
        case SoundEvent::SOUND_ROTATE:       duration = SOUND_DURATION_ROTATE;       break;
        case SoundEvent::SOUND_PLACE:        duration = SOUND_DURATION_PLACE;        break;
        case SoundEvent::SOUND_LINE_CLEAR_1: duration = SOUND_DURATION_LINE_CLEAR_1; break;
        case SoundEvent::SOUND_LINE_CLEAR_2: duration = SOUND_DURATION_LINE_CLEAR_2; break;
        case SoundEvent::SOUND_LINE_CLEAR_3: duration = SOUND_DURATION_LINE_CLEAR_3; break;
        case SoundEvent::SOUND_LINE_CLEAR_4: duration = SOUND_DURATION_LINE_CLEAR_4; break;
        case SoundEvent::SOUND_LEVEL_UP:     duration = SOUND_DURATION_LEVEL_UP;     break;
        case SoundEvent::SOUND_GAME_OVER:    duration = SOUND_DURATION_GAME_OVER;    break;
        default: return;
    }

    int16_t* buffer = (int16_t*)malloc(duration * sizeof(int16_t));
    if (!buffer) {
        Serial.println("Failed to allocate SFX buffer");
        return;
    }

    switch (event) {
        case SoundEvent::SOUND_ROTATE:       generateRotateSound(buffer, duration);      break;
        case SoundEvent::SOUND_PLACE:        generatePlaceSound(buffer, duration);       break;
        case SoundEvent::SOUND_LINE_CLEAR_1: generateLineClear1Sound(buffer, duration);  break;
        case SoundEvent::SOUND_LINE_CLEAR_2: generateLineClear2Sound(buffer, duration);  break;
        case SoundEvent::SOUND_LINE_CLEAR_3: generateLineClear3Sound(buffer, duration);  break;
        case SoundEvent::SOUND_LINE_CLEAR_4: generateLineClear4Sound(buffer, duration);  break;
        case SoundEvent::SOUND_LEVEL_UP:     generateLevelUpSound(buffer, duration);     break;
        case SoundEvent::SOUND_GAME_OVER:    generateGameOverSound(buffer, duration);    break;
        default: break;
    }

    size_t totalWritten = 0;
    while (totalWritten < (size_t)duration) {
        size_t toWrite = min((size_t)AUDIO_BUFFER_SIZE, (size_t)duration - totalWritten);
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

// ---- Background music chunk writer ----------------------------------------
//
// Advances the melody by exactly AUDIO_BUFFER_SIZE samples and writes them
// to I2S.  The last NOTE_GAP_RATIO fraction of every note is silenced to
// articulate individual pitches cleanly.

void AudioManager::advanceMusicChunk() {
    static int16_t buf[AUDIO_BUFFER_SIZE];

    // Select the active theme
    const MusicNote* theme    = (musicTrack == 0) ? TETRIS_THEME_A : TETRIS_THEME_B;
    const int        themeSize = (musicTrack == 0) ? TETRIS_THEME_A_SIZE : TETRIS_THEME_B_SIZE;

    int pos = 0;
    while (pos < AUDIO_BUFFER_SIZE) {
        const MusicNote& note = theme[musicNoteIndex];

        // How many more samples remain in this note?
        int samplesLeft  = note.duration - musicSampleOffset;
        int toGenerate   = min(AUDIO_BUFFER_SIZE - pos, samplesLeft);

        // Silence threshold for articulation gap at end of note
        int toneEnd = (int)(note.duration * (1.0f - NOTE_GAP_RATIO));

        for (int i = 0; i < toGenerate; i++) {
            int offset = musicSampleOffset + i;
            if (note.frequency > 0.0f && offset < toneEnd) {
                buf[pos + i]  = (int16_t)(MUSIC_VOLUME * sinf(musicPhase));
                musicPhase   += 2.0f * PI * note.frequency / SAMPLE_RATE;
                if (musicPhase >= 2.0f * PI) musicPhase -= 2.0f * PI;
            } else {
                buf[pos + i] = 0;
            }
        }

        pos               += toGenerate;
        musicSampleOffset += toGenerate;

        // Advance to the next note when the current one is exhausted
        if (musicSampleOffset >= note.duration) {
            musicSampleOffset = 0;
            musicPhase        = 0.0f;   // reset phase at note boundary
            musicNoteIndex    = (musicNoteIndex + 1) % themeSize;
        }
    }

    size_t bytesWritten = 0;
    i2s_write(I2S_NUM_0, buf, AUDIO_BUFFER_SIZE * sizeof(int16_t),
              &bytesWritten, portMAX_DELAY);
}

// ---- Main audio processing loop -------------------------------------------

void AudioManager::processAudio() {
    if (!initialized) {
        vTaskDelay(pdMS_TO_TICKS(10));
        return;
    }

    // Handle one pending sound effect (non-blocking queue peek)
    SoundEvent event = SoundEvent::SOUND_NONE;
    if (xQueueReceive(soundQueue, &event, 0) == pdTRUE &&
        event != SoundEvent::SOUND_NONE) {
        playSoundEffect(event);
    }

    // Always advance background music (or write silence to keep I2S alive)
    if (musicEnabled && !musicPaused) {
        advanceMusicChunk();
    } else {
        // Write silence so the I2S DMA pipeline stays primed
        static int16_t silence[AUDIO_BUFFER_SIZE];
        memset(silence, 0, sizeof(silence));
        size_t bytesWritten = 0;
        i2s_write(I2S_NUM_0, silence, sizeof(silence), &bytesWritten, portMAX_DELAY);
    }
}

void AudioManager::audioTask(void* pvParameters) {
    AudioManager* manager = (AudioManager*)pvParameters;
    Serial.println("Audio task started on core " + String(xPortGetCoreID()));
    while (true) {
        manager->processAudio();
    }
}
