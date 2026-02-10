#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include "../config.h"
#include "../game/game_state.h"
#include "../display/display.h"
#include "../game/input.h"
#include "../audio/audio.h"
#include "../util/timer.h"

// Task handles
extern TaskHandle_t gameTaskHandle;
extern TaskHandle_t audioTaskHandle;

// Shared game state (accessed by game task)
extern GameState gameState;
extern Display display;
extern InputManager inputManager;

// Task functions
void gameTask(void* pvParameters);
void createSystemTasks();

#endif // TASKS_H
