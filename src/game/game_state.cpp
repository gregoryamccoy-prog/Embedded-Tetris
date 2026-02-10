#include "game_state.h"

void initGame(GameState& s) {
    // Clear playfield
    for (int y = 0; y < PLAYFIELD_HEIGHT + PLAYFIELD_HIDDEN_ROWS; y++) {
        for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
            s.field[y][x] = 0;
        }
    }
    
    // Initialize statistics
    s.linesCleared = 0;
    s.level = 0;
    s.score = 0;
    s.status = GameStatus::PLAYING;
    
    // Create first two pieces
    s.nextPiece = createTetromino(getRandomTetrominoType());
    s.hasCurrent = false;
    spawnNewPiece(s);
}

bool checkCollision(const GameState& s, const Tetromino& piece, int x, int y, int rotation) {
    // Create temporary tetromino with new position/rotation
    Tetromino temp = piece;
    temp.x = x;
    temp.y = y;
    temp.rotation = rotation;
    
    int blockX[4], blockY[4];
    getBlockPositions(temp, blockX, blockY);
    
    for (int i = 0; i < 4; i++) {
        // Check bounds
        if (blockX[i] < 0 || blockX[i] >= PLAYFIELD_WIDTH) {
            return true;  // Collision with walls
        }
        if (blockY[i] < 0) {
            return true;  // Collision with top (during rotation)
        }
        if (blockY[i] >= PLAYFIELD_HEIGHT + PLAYFIELD_HIDDEN_ROWS) {
            return true;  // Collision with bottom
        }
        
        // Check collision with placed blocks
        if (s.field[blockY[i]][blockX[i]] != 0) {
            return true;
        }
    }
    
    return false;  // No collision
}

bool spawnNewPiece(GameState& s) {
    s.currentPiece = s.nextPiece;
    s.nextPiece = createTetromino(getRandomTetrominoType());
    s.hasCurrent = true;
    
    // Check if spawn position is valid
    if (checkCollision(s, s.currentPiece, s.currentPiece.x, s.currentPiece.y, s.currentPiece.rotation)) {
        s.status = GameStatus::GAME_OVER;
        return false;  // Game over - can't spawn
    }
    
    return true;
}

bool movePieceLeft(GameState& s) {
    if (s.status != GameStatus::PLAYING || !s.hasCurrent) return false;
    
    int newX = s.currentPiece.x - 1;
    if (!checkCollision(s, s.currentPiece, newX, s.currentPiece.y, s.currentPiece.rotation)) {
        s.currentPiece.x = newX;
        return true;
    }
    return false;
}

bool movePieceRight(GameState& s) {
    if (s.status != GameStatus::PLAYING || !s.hasCurrent) return false;
    
    int newX = s.currentPiece.x + 1;
    if (!checkCollision(s, s.currentPiece, newX, s.currentPiece.y, s.currentPiece.rotation)) {
        s.currentPiece.x = newX;
        return true;
    }
    return false;
}

bool movePieceDown(GameState& s) {
    if (s.status != GameStatus::PLAYING || !s.hasCurrent) return false;
    
    int newY = s.currentPiece.y + 1;
    if (!checkCollision(s, s.currentPiece, s.currentPiece.x, newY, s.currentPiece.rotation)) {
        s.currentPiece.y = newY;
        return true;
    }
    return false;
}

void hardDrop(GameState& s) {
    if (s.status != GameStatus::PLAYING || !s.hasCurrent) return;
    
    // Move down until collision
    while (movePieceDown(s)) {
        // Keep moving
    }
    
    // Lock the piece
    lockPiece(s);
}

bool rotatePieceLeft(GameState& s) {
    if (s.status != GameStatus::PLAYING || !s.hasCurrent) return false;
    
    int newRotation = (s.currentPiece.rotation + 3) % 4;
    
    // Try rotation at current position
    if (!checkCollision(s, s.currentPiece, s.currentPiece.x, s.currentPiece.y, newRotation)) {
        s.currentPiece.rotation = newRotation;
        return true;
    }
    
    // Try wall kick: shift right
    if (!checkCollision(s, s.currentPiece, s.currentPiece.x + 1, s.currentPiece.y, newRotation)) {
        s.currentPiece.x += 1;
        s.currentPiece.rotation = newRotation;
        return true;
    }
    
    // Try wall kick: shift left
    if (!checkCollision(s, s.currentPiece, s.currentPiece.x - 1, s.currentPiece.y, newRotation)) {
        s.currentPiece.x -= 1;
        s.currentPiece.rotation = newRotation;
        return true;
    }
    
    return false;  // Rotation failed
}

void lockPiece(GameState& s) {
    if (!s.hasCurrent) return;
    
    int blockX[4], blockY[4];
    getBlockPositions(s.currentPiece, blockX, blockY);
    
    // Place blocks in playfield
    for (int i = 0; i < 4; i++) {
        if (blockY[i] >= 0 && blockY[i] < PLAYFIELD_HEIGHT + PLAYFIELD_HIDDEN_ROWS &&
            blockX[i] >= 0 && blockX[i] < PLAYFIELD_WIDTH) {
            s.field[blockY[i]][blockX[i]] = s.currentPiece.color;
        }
    }
    
    s.hasCurrent = false;
    
    // Clear completed lines
    int linesRemoved = clearCompletedLines(s);
    if (linesRemoved > 0) {
        s.linesCleared += linesRemoved;
        
        // Update score (simple scoring)
        switch (linesRemoved) {
            case 1: s.score += 100; break;
            case 2: s.score += 300; break;
            case 3: s.score += 500; break;
            case 4: s.score += 800; break;
        }
        
        updateLevel(s);
    }
    
    // Spawn next piece
    spawnNewPiece(s);
}

int clearCompletedLines(GameState& s) {
    int linesRemoved = 0;
    
    // Check each row from bottom to top
    for (int y = PLAYFIELD_HEIGHT + PLAYFIELD_HIDDEN_ROWS - 1; y >= 0; y--) {
        bool lineFull = true;
        
        for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
            if (s.field[y][x] == 0) {
                lineFull = false;
                break;
            }
        }
        
        if (lineFull) {
            linesRemoved++;
            
            // Shift all rows above down
            for (int yy = y; yy > 0; yy--) {
                for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
                    s.field[yy][x] = s.field[yy - 1][x];
                }
            }
            
            // Clear top row
            for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
                s.field[0][x] = 0;
            }
            
            // Check this row again (since we shifted down)
            y++;
        }
    }
    
    return linesRemoved;
}

bool stepGravity(GameState& s) {
    if (s.status != GameStatus::PLAYING || !s.hasCurrent) return false;
    
    if (movePieceDown(s)) {
        return true;  // Piece moved down
    } else {
        // Piece can't move down, lock it
        lockPiece(s);
        return false;
    }
}

void updateLevel(GameState& s) {
    s.level = s.linesCleared / LINES_PER_LEVEL;
}

void togglePause(GameState& s) {
    if (s.status == GameStatus::GAME_OVER) return;
    
    if (s.status == GameStatus::PLAYING) {
        s.status = GameStatus::PAUSED;
    } else if (s.status == GameStatus::PAUSED) {
        s.status = GameStatus::PLAYING;
    }
}

bool isGameOver(const GameState& s) {
    return s.status == GameStatus::GAME_OVER;
}
