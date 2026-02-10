#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>
#include "../config.h"
#include "../game/game_state.h"
#include "../game/tetromino.h"

// Display management class
class Display {
private:
    TFT_eSPI tft;
    
    // Track previous state for efficient rendering
    uint16_t prevField[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
    int prevPieceX[4];
    int prevPieceY[4];
    bool hasPrevPiece;
    
    void drawCellAt(int x, int y, uint16_t color);
    void drawGridCell(int gridX, int gridY, uint16_t color);
    
public:
    Display();
    
    void init();
    void clearScreen();
    void drawPlayfield(const GameState& state);
    void drawCurrentPiece(const GameState& state);
    void drawNextPiece(const Tetromino& next);
    void drawStats(int level, int linesCleared, unsigned long score);
    void drawPauseOverlay();
    void drawGameOver(const GameState& state);
    void fullRedraw(const GameState& state);
};

#endif // DISPLAY_H
