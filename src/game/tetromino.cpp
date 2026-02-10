#include "tetromino.h"

// I-piece (Cyan) - Straight line
static const int8_t SHAPE_I[4][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},   // Rotation 0: horizontal
    {{0, -1}, {0, 0}, {0, 1}, {0, 2}},   // Rotation 1: vertical
    {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},   // Rotation 2: horizontal
    {{0, -1}, {0, 0}, {0, 1}, {0, 2}}    // Rotation 3: vertical
};

// O-piece (Yellow) - Square (no rotation change)
static const int8_t SHAPE_O[4][4][2] = {
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
};

// T-piece (Purple) - T-shape
static const int8_t SHAPE_T[4][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {0, 1}},   // Rotation 0: T pointing up
    {{0, -1}, {0, 0}, {0, 1}, {1, 0}},   // Rotation 1: T pointing right
    {{-1, 0}, {0, 0}, {1, 0}, {0, -1}},  // Rotation 2: T pointing down
    {{0, -1}, {0, 0}, {0, 1}, {-1, 0}}   // Rotation 3: T pointing left
};

// S-piece (Green) - S-shape
static const int8_t SHAPE_S[4][4][2] = {
    {{-1, 1}, {0, 1}, {0, 0}, {1, 0}},   // Rotation 0
    {{0, -1}, {0, 0}, {1, 0}, {1, 1}},   // Rotation 1
    {{-1, 1}, {0, 1}, {0, 0}, {1, 0}},   // Rotation 2
    {{0, -1}, {0, 0}, {1, 0}, {1, 1}}    // Rotation 3
};

// Z-piece (Red) - Z-shape
static const int8_t SHAPE_Z[4][4][2] = {
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},   // Rotation 0
    {{1, -1}, {1, 0}, {0, 0}, {0, 1}},   // Rotation 1
    {{-1, 0}, {0, 0}, {0, 1}, {1, 1}},   // Rotation 2
    {{1, -1}, {1, 0}, {0, 0}, {0, 1}}    // Rotation 3
};

// J-piece (Blue) - J-shape
static const int8_t SHAPE_J[4][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}},  // Rotation 0
    {{0, -1}, {0, 0}, {0, 1}, {1, -1}},  // Rotation 1
    {{-1, 0}, {0, 0}, {1, 0}, {1, -1}},  // Rotation 2
    {{0, -1}, {0, 0}, {0, 1}, {-1, 1}}   // Rotation 3
};

// L-piece (Orange) - L-shape
static const int8_t SHAPE_L[4][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {1, 1}},   // Rotation 0
    {{0, -1}, {0, 0}, {0, 1}, {1, 1}},   // Rotation 1
    {{-1, 0}, {0, 0}, {1, 0}, {-1, -1}}, // Rotation 2
    {{0, -1}, {0, 0}, {0, 1}, {-1, -1}}  // Rotation 3
};

TetrominoType getRandomTetrominoType() {
    return static_cast<TetrominoType>(random(0, 7));
}

uint16_t getTetrominoColor(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return COLOR_CYAN;
        case TetrominoType::O: return COLOR_YELLOW;
        case TetrominoType::T: return COLOR_PURPLE;
        case TetrominoType::S: return COLOR_GREEN;
        case TetrominoType::Z: return COLOR_RED;
        case TetrominoType::J: return COLOR_BLUE;
        case TetrominoType::L: return COLOR_ORANGE;
        default: return COLOR_WHITE;
    }
}

Tetromino createTetromino(TetrominoType type) {
    Tetromino t;
    t.type = type;
    t.x = PLAYFIELD_WIDTH / 2;  // Center of playfield
    t.y = 0;                     // Top of playfield
    t.rotation = 0;
    t.color = getTetrominoColor(type);
    
    // Copy shape data
    const int8_t (*shapeData)[4][2];
    switch (type) {
        case TetrominoType::I: shapeData = SHAPE_I; break;
        case TetrominoType::O: shapeData = SHAPE_O; break;
        case TetrominoType::T: shapeData = SHAPE_T; break;
        case TetrominoType::S: shapeData = SHAPE_S; break;
        case TetrominoType::Z: shapeData = SHAPE_Z; break;
        case TetrominoType::J: shapeData = SHAPE_J; break;
        case TetrominoType::L: shapeData = SHAPE_L; break;
        default: shapeData = SHAPE_I; break;
    }
    
    for (int r = 0; r < 4; r++) {
        for (int b = 0; b < 4; b++) {
            t.shape[r][b][0] = shapeData[r][b][0];
            t.shape[r][b][1] = shapeData[r][b][1];
        }
    }
    
    return t;
}

void rotateLeft(Tetromino& t) {
    t.rotation = (t.rotation + 3) % 4;  // -1 mod 4 = +3 mod 4
}

void rotateRight(Tetromino& t) {
    t.rotation = (t.rotation + 1) % 4;
}

void getBlockPositions(const Tetromino& t, int outX[4], int outY[4]) {
    for (int i = 0; i < 4; i++) {
        outX[i] = t.x + t.shape[t.rotation][i][0];
        outY[i] = t.y + t.shape[t.rotation][i][1];
    }
}
