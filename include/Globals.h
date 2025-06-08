#pragma once
#include <string>
#include <vector>
#include "raylib.h"

enum GameState {
    MENU,
    LEVEL_SELECT,
    PLAYING,
    SETTINGS,
    STATS,
    CREDITS,
    VICTORY
};

enum class ObstacleType {
    NONE,
    STATIC_PLATFORM,
    MOVING_PLATFORM,
    SPIKE,
    EXPLODING_PLATFORM,
    AUTO_RUN_START,     
    AUTO_RUN_END,       
    BOOST_GATE,         
    NEON_WALL,          
    SYNC_PLATFORM       
};

struct MenuButton {
    const char* label;
    Color glowColor;
    float targetScale = 1.0f;
    float currentScale = 0.0f;
    float pulseTime = 0.0f;
    float animOffset = -400.0f;
    bool clicked = false;
    float clickTime = 0.0f;
};

struct ScoreEntry {
    std::string playerName;
    int score;
};

struct LevelInfo {
    const char* name;
    const char* thumbnailPath;
    Texture2D thumbnail;
};

struct LevelEvent {
    float time;
    ObstacleType type;
    Vector4 position;
};


