#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <Arduino.h>
#include "../config.h"
#include "tetromino.h"

enum class GameStatus {
    PLAYING,
    PAUSED,
    GAME_OVER
};

struct GameState {
    // Playfield grid: stores color of each cell (0 = empty)
    uint16_t field[PLAYFIELD_HEIGHT + PLAYFIELD_HIDDEN_ROWS][PLAYFIELD_WIDTH];
    
    // Current piece
    Tetromino currentPiece;
    bool hasCurrent;
    
    // Next piece
    Tetromino nextPiece;
    
    // Game statistics
    int linesCleared;
    int level;
    GameStatus status;
    
    // Score (optional but useful)
    unsigned long score;
};

// Game state management functions
void initGame(GameState& s);
bool spawnNewPiece(GameState& s);
bool checkCollision(const GameState& s, const Tetromino& piece, int x, int y, int rotation);
bool movePieceLeft(GameState& s);
bool movePieceRight(GameState& s);
bool movePieceDown(GameState& s);
void hardDrop(GameState& s);
bool rotatePieceLeft(GameState& s);
void lockPiece(GameState& s);
int clearCompletedLines(GameState& s);
bool stepGravity(GameState& s);
void updateLevel(GameState& s);
void togglePause(GameState& s);
bool isGameOver(const GameState& s);

#endif // GAME_STATE_H
