// GameApp.h
#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include "Globals.h" // 确保这里的路径是正确的

class GameApp {
public:
    GameApp();
    ~GameApp();
    void Run();

private:
    void Init();
    void Update();
    void Draw();

    // 【修正】重新声明所有在 GameApp.cpp 中被调用的函数
    void DrawBackground();
    void DrawTitle();
    void DrawPulseEffect();

    GameState currentState;
    std::vector<ScoreEntry> leaderboard;

    // 【修正】重新声明所有在 GameApp.cpp 中被使用的成员变量
    int currentTitleFrame, titleFrameCounter;
    int currentBgFrame, bgFrameCounter;

    bool shouldExit;
};