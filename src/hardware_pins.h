#ifndef HARDWARE_PINS_H
#define HARDWARE_PINS_H

// TFT Display Pins (SPI - VSPI)
// These are also defined in platformio.ini build_flags for TFT_eSPI
#define TFT_CS_PIN    5
#define TFT_DC_PIN    2
#define TFT_RST_PIN   4
#define TFT_BL_PIN    15
#define TFT_MOSI_PIN  23
#define TFT_MISO_PIN  19
#define TFT_SCLK_PIN  18

// I2S Audio Pins (MAX98357A)
#define I2S_BCLK_PIN  26
#define I2S_LRCLK_PIN 25
#define I2S_DATA_PIN  22

// Button Pins (Active Low with Internal Pull-Up)
#define BTN_PAUSE_PIN        32  // Button A - Pause/Resume
#define BTN_ROTATE_PIN       33  // Button B - Rotate (left)
#define BTN_RIGHT_PIN        27  // Button C - Translate Right
#define BTN_LEFT_PIN         14  // Button D - Translate Left
#define BTN_HARD_DROP_PIN    13  // Button E - Hard Drop

// Joystick Pins (Analog 2-Axis + Switch)
#define JOYSTICK_X_PIN       34  // ADC1 - X axis (horizontal)
#define JOYSTICK_Y_PIN       35  // ADC1 - Y axis (vertical)
#define JOYSTICK_SW_PIN      21  // Button - Active Low with Pull-Up

#endif // HARDWARE_PINS_H
