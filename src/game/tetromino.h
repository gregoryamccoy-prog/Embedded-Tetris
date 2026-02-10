#ifndef TETROMINO_H
#define TETROMINO_H

#include <Arduino.h>
#include "../config.h"

// Tetromino types
enum class TetrominoType {
    I,  // Cyan - straight piece
    O,  // Yellow - square piece
    T,  // Purple - T-shaped piece
    S,  // Green - S-shaped piece
    Z,  // Red - Z-shaped piece
    J,  // Blue - J-shaped piece
    L   // Orange - L-shaped piece
};

// Tetromino structure
struct Tetromino {
    TetrominoType type;
    int x;              // Grid position X (column)
    int y;              // Grid position Y (row)
    int rotation;       // Rotation state (0-3)
    uint16_t color;     // RGB565 color
    
    // Each piece is represented as 4 blocks with relative coordinates
    // Stored as [rotation][block][x/y]
    int8_t shape[4][4][2];  // 4 rotations, 4 blocks, 2 coords (x, y)
};

// Function declarations
TetrominoType getRandomTetrominoType();
Tetromino createTetromino(TetrominoType type);
void rotateLeft(Tetromino& t);
void rotateRight(Tetromino& t);
void getBlockPositions(const Tetromino& t, int outX[4], int outY[4]);
uint16_t getTetrominoColor(TetrominoType type);

#endif // TETROMINO_H
