#pragma once
#include "raylib.h"
#include <vector>
#include <string>

namespace Assets {
    extern Music bgm;
    extern Music levelMusic;
    extern Sound clickSound;
    extern std::vector<Texture2D> titleFrames;
    extern std::vector<Texture2D> bgFrames;

    extern std::vector<Texture2D> playerRunFrames;
    extern Texture2D playerJumpUpFrame;
    extern Texture2D playerJumpDownFrame;
    extern std::vector<Texture2D> playerDieFrames;
    extern std::vector<Texture2D> explosionFrames; // 【新增】声明爆炸动画帧

    extern Shader neonShader;
    extern int neonShaderColorLoc;

    extern float volume;
    extern float sfxVolume;
    extern bool isFullscreen;

    void LoadAll();
    void SaveConfig();
    void UnloadAll();
}