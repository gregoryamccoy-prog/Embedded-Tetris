#include "display.h"

Display::Display() : tft() {
    hasPrevPiece = false;
    hasDrawnNextPiece = false;
    prevLevel = -1;
    prevLinesCleared = -1;
    prevScore = 0;
    
    // Initialize previous field state
    for (int y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
            prevField[y][x] = 0;
        }
    }
}

void Display::init() {
    tft.init();
    tft.setRotation(0);  // Portrait mode
    tft.fillScreen(COLOR_BACKGROUND);
    
    // Draw static UI elements
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    
    // Title
    tft.setCursor(10, 5);
    tft.setTextSize(2);
    tft.print("TETRIS");
    
    // Draw playfield border
    int borderX = PLAYFIELD_OFFSET_X - 2;
    int borderY = PLAYFIELD_OFFSET_Y - 2;
    int borderW = PLAYFIELD_WIDTH * CELL_SIZE + 4;
    int borderH = PLAYFIELD_HEIGHT * CELL_SIZE + 4;
    tft.drawRect(borderX, borderY, borderW, borderH, COLOR_WHITE);
    
    // UI Labels
    tft.setTextSize(1);
    tft.setCursor(UI_PANEL_X, UI_PANEL_Y - 15);
    tft.print("NEXT:");
    
    tft.setCursor(UI_PANEL_X, UI_PANEL_Y + 60);
    tft.print("LEVEL:");
    
    tft.setCursor(UI_PANEL_X, UI_PANEL_Y + 90);
    tft.print("LINES:");
    
    tft.setCursor(UI_PANEL_X, UI_PANEL_Y + 120);
    tft.print("SCORE:");
}

void Display::clearScreen() {
    tft.fillScreen(COLOR_BACKGROUND);
}

void Display::drawCellAt(int x, int y, uint16_t color) {
    if (color == 0) {
        tft.fillRect(x, y, CELL_SIZE, CELL_SIZE, COLOR_BACKGROUND);
    } else {
        tft.fillRect(x, y, CELL_SIZE, CELL_SIZE, color);
        // Draw a slight border for 3D effect
        tft.drawRect(x, y, CELL_SIZE, CELL_SIZE, COLOR_WHITE);
    }
}

void Display::drawGridCell(int gridX, int gridY, uint16_t color) {
    int pixelX = PLAYFIELD_OFFSET_X + (gridX * CELL_SIZE);
    int pixelY = PLAYFIELD_OFFSET_Y + (gridY * CELL_SIZE);
    drawCellAt(pixelX, pixelY, color);
}

void Display::drawPlayfield(const GameState& state) {
    // Only draw visible rows (not hidden rows)
    for (int y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
            int actualY = y + PLAYFIELD_HIDDEN_ROWS;
            uint16_t color = state.field[actualY][x];
            
            // Only redraw if changed
            if (color != prevField[y][x]) {
                drawGridCell(x, y, color);
                prevField[y][x] = color;
            }
        }
    }
}

void Display::drawCurrentPiece(const GameState& state) {
    // Clear previous piece position
    if (hasPrevPiece) {
        for (int i = 0; i < 4; i++) {
            int gridY = prevPieceY[i] - PLAYFIELD_HIDDEN_ROWS;
            if (gridY >= 0 && gridY < PLAYFIELD_HEIGHT) {
                // Restore background (from playfield)
                int actualY = prevPieceY[i];
                uint16_t bgColor = 0;
                if (actualY >= 0 && actualY < PLAYFIELD_HEIGHT + PLAYFIELD_HIDDEN_ROWS) {
                    bgColor = state.field[actualY][prevPieceX[i]];
                }
                drawGridCell(prevPieceX[i], gridY, bgColor);
            }
        }
    }
    
    // Draw current piece
    if (state.hasCurrent) {
        int blockX[4], blockY[4];
        getBlockPositions(state.currentPiece, blockX, blockY);
        
        for (int i = 0; i < 4; i++) {
            int gridY = blockY[i] - PLAYFIELD_HIDDEN_ROWS;
            if (gridY >= 0 && gridY < PLAYFIELD_HEIGHT &&
                blockX[i] >= 0 && blockX[i] < PLAYFIELD_WIDTH) {
                drawGridCell(blockX[i], gridY, state.currentPiece.color);
            }
            
            // Save for next frame
            prevPieceX[i] = blockX[i];
            prevPieceY[i] = blockY[i];
        }
        hasPrevPiece = true;
    } else {
        hasPrevPiece = false;
    }
}

void Display::drawNextPiece(const Tetromino& next) {
    // Only redraw if the next piece changed
    if (hasDrawnNextPiece && 
        next.type == prevNextPieceType && 
        next.rotation == prevNextPieceRotation) {
        return;  // No change, skip redraw
    }
    
    // Clear next piece area
    int previewX = UI_PANEL_X;
    int previewY = UI_PANEL_Y;
    tft.fillRect(previewX, previewY, NEXT_PIECE_CELL_SIZE * 4, NEXT_PIECE_CELL_SIZE * 4, COLOR_BACKGROUND);
    
    // Draw next piece
    int blockX[4], blockY[4];
    getBlockPositions(next, blockX, blockY);
    
    // Find bounds for centering
    int minX = blockX[0], maxX = blockX[0];
    int minY = blockY[0], maxY = blockY[0];
    for (int i = 1; i < 4; i++) {
        if (blockX[i] < minX) minX = blockX[i];
        if (blockX[i] > maxX) maxX = blockX[i];
        if (blockY[i] < minY) minY = blockY[i];
        if (blockY[i] > maxY) maxY = blockY[i];
    }
    
    int offsetX = -(minX + maxX) / 2;
    int offsetY = -(minY + maxY) / 2;
    
    for (int i = 0; i < 4; i++) {
        int px = previewX + (blockX[i] + offsetX + 2) * NEXT_PIECE_CELL_SIZE;
        int py = previewY + (blockY[i] + offsetY + 2) * NEXT_PIECE_CELL_SIZE;
        
        tft.fillRect(px, py, NEXT_PIECE_CELL_SIZE - 1, NEXT_PIECE_CELL_SIZE - 1, next.color);
    }
    
    // Remember what we drew
    prevNextPieceType = next.type;
    prevNextPieceRotation = next.rotation;
    hasDrawnNextPiece = true;
}

void Display::drawStats(int level, int linesCleared, unsigned long score) {
    tft.setTextColor(COLOR_WHITE, COLOR_BACKGROUND);
    tft.setTextSize(1);
    
    // Only update level if changed
    if (level != prevLevel) {
        tft.setCursor(UI_PANEL_X, UI_PANEL_Y + 75);
        tft.fillRect(UI_PANEL_X, UI_PANEL_Y + 75, 60, 10, COLOR_BACKGROUND);
        tft.print(level);
        prevLevel = level;
    }
    
    // Only update lines if changed
    if (linesCleared != prevLinesCleared) {
        tft.setCursor(UI_PANEL_X, UI_PANEL_Y + 105);
        tft.fillRect(UI_PANEL_X, UI_PANEL_Y + 105, 60, 10, COLOR_BACKGROUND);
        tft.print(linesCleared);
        prevLinesCleared = linesCleared;
    }
    
    // Only update score if changed
    if (score != prevScore) {
        tft.setCursor(UI_PANEL_X, UI_PANEL_Y + 135);
        tft.fillRect(UI_PANEL_X, UI_PANEL_Y + 135, 60, 10, COLOR_BACKGROUND);
        tft.print(score);
        prevScore = score;
    }
}

void Display::drawPauseOverlay() {
    int boxX = SCREEN_WIDTH / 2 - 40;
    int boxY = SCREEN_HEIGHT / 2 - 20;
    
    tft.fillRect(boxX, boxY, 80, 40, COLOR_BLACK);
    tft.drawRect(boxX, boxY, 80, 40, COLOR_WHITE);
    
    tft.setTextColor(COLOR_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(boxX + 10, boxY + 12);
    tft.print("PAUSED");
}

void Display::drawGameOver(const GameState& state) {
    int boxX = SCREEN_WIDTH / 2 - 60;
    int boxY = SCREEN_HEIGHT / 2 - 40;
    
    tft.fillRect(boxX, boxY, 120, 80, COLOR_RED);
    tft.drawRect(boxX, boxY, 120, 80, COLOR_WHITE);
    
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(2);
    tft.setCursor(boxX + 10, boxY + 10);
    tft.print("GAME");
    tft.setCursor(boxX + 15, boxY + 30);
    tft.print("OVER");
    
    tft.setTextSize(1);
    tft.setCursor(boxX + 5, boxY + 55);
    tft.print("Level: ");
    tft.print(state.level);
    tft.setCursor(boxX + 5, boxY + 68);
    tft.print("Lines: ");
    tft.print(state.linesCleared);
}

void Display::fullRedraw(const GameState& state) {
    // Force full redraw by marking everything as changed
    for (int y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
            prevField[y][x] = 0xFFFF;  // Invalid color
        }
    }
    hasPrevPiece = false;
    hasDrawnNextPiece = false;
    prevLevel = -1;
    prevLinesCleared = -1;
    prevScore = 0;
    
    drawPlayfield(state);
    drawCurrentPiece(state);
    drawNextPiece(state.nextPiece);
    drawStats(state.level, state.linesCleared, state.score);
}
