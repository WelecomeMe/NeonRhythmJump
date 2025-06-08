// LevelSelect.cpp
#include "LevelSelect.h"
#include "Globals.h"
#include "raylib.h"
#include "raymath.h" 
#include <vector>
#include <cmath>

namespace {
    std::vector<LevelInfo> levels;
    int currentLevelIndex = 0;

 
    float smoothIndex = 0.0f;
    const float lerpSpeed = 8.0f; // 控制动画速度

    bool backPressed = false;
    int levelToStart = -1;
}

namespace LevelSelect {

    void Init() {
        levels = {
            {"Level 1: Luminescence", "Assets/images/thumb_level1.jpg"},
            {"Level 2: Starlight",    "Assets/images/thumb_level2.jpg"},
            {"Level 3: Nebula",       "Assets/images/thumb_level3.jpg"}
        };

        for (auto& level : levels) {
            if (FileExists(level.thumbnailPath)) {
                level.thumbnail = LoadTexture(level.thumbnailPath);
            }
            else {
                Image placeholder = GenImageColor(200, 200, DARKGRAY);
                ImageDrawText(&placeholder, "No Thumb", 10, 80, 40, RAYWHITE);
                level.thumbnail = LoadTextureFromImage(placeholder);
                UnloadImage(placeholder);
            }
        }
        // 初始化平滑索引
        currentLevelIndex = 0;
        smoothIndex = (float)currentLevelIndex;
    }

    void Unload() {
        for (auto& level : levels) {
            UnloadTexture(level.thumbnail);
        }
    }

    void Update() {
        backPressed = false;
        levelToStart = -1;

        // --- 输入处理 ---
        float wheel = GetMouseWheelMove();
        if (wheel < 0 || IsKeyPressed(KEY_RIGHT)) { // 注意：滚轮向下通常是正值，但这里反向更直观
            currentLevelIndex++;
        }
        if (wheel > 0 || IsKeyPressed(KEY_LEFT)) {
            currentLevelIndex--;
        }

        if (currentLevelIndex < 0) currentLevelIndex = (int)levels.size() - 1;
        if (currentLevelIndex >= (int)levels.size()) currentLevelIndex = 0;

        // --- 动画更新 ---
        // ✅ 使用 Lerp 函数平滑地追踪目标索引
        smoothIndex = Lerp(smoothIndex, (float)currentLevelIndex, lerpSpeed * GetFrameTime());

        // --- 点击处理 ---
        Vector2 mouse = GetMousePosition();
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float globalScale = fminf(screenW / 1200.0f, screenH / 800.0f);

        float centerCardSize = 300 * globalScale;
        Rectangle centerRect = { (screenW - centerCardSize) / 2, (screenH - centerCardSize) / 2 - 50 * globalScale, centerCardSize, centerCardSize };

        // 当动画基本稳定且鼠标点击时才触发
        if (fabsf(smoothIndex - currentLevelIndex) < 0.1f && CheckCollisionPointRec(mouse, centerRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            levelToStart = currentLevelIndex;
        }

        // --- 返回按钮 ---
        float backBtnW = 400 * globalScale;
        float backBtnH = 50 * globalScale;
        float backBtnX = (screenW - backBtnW) / 2.0f;
        float backBtnY = screenH - 120 * globalScale;
        Rectangle backBtnRect = { backBtnX, backBtnY, backBtnW, backBtnH };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, backBtnRect)) {
            backPressed = true;
        }
    }

    void Draw() {
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float globalScale = fminf(screenW / 1200.0f, screenH / 800.0f);

        // --- 绘制所有关卡卡片 ---
        for (int i = 0; i < (int)levels.size(); ++i) {
            // ✅ 使用平滑索引来计算位置、大小和透明度
            float offset = (float)i - smoothIndex;

            Vector2 pos = { screenW / 2 + offset * 400 * globalScale, screenH / 2 - 50 * globalScale };
            float scale = 1.0f - fabsf(offset) * 0.5f;
            float alpha = 1.0f - fabsf(offset) * 0.7f;

            scale = Clamp(scale, 0.0f, 1.0f);
            alpha = Clamp(alpha, 0.0f, 1.0f);

            Texture2D& tex = levels[i].thumbnail;
            float size = 300 * globalScale * scale;
            Rectangle destRect = { pos.x - size / 2, pos.y - size / 2, size, size };

            DrawTexturePro(tex, { 0, 0, (float)tex.width, (float)tex.height }, destRect, { 0,0 }, 0, Fade(WHITE, alpha));
        }

        // --- 绘制UI文本和返回按钮 ---
        float titleFontSize = 40 * globalScale;
        float levelNameFontSize = 30 * globalScale;
        float itemFontSize = 22 * globalScale;

        const char* titleText = "Select Level";
        Vector2 titleSize = MeasureTextEx(GetFontDefault(), titleText, titleFontSize, 1);
        DrawText(titleText, (screenW - titleSize.x) / 2.0f, screenH * 0.15f, titleFontSize, GOLD);

        // ✅ 根据当前选中的索引（非平滑值）显示关卡名称
        const char* levelName = levels[currentLevelIndex].name;
        Vector2 levelNameSize = MeasureTextEx(GetFontDefault(), levelName, levelNameFontSize, 1);
        DrawText(levelName, (screenW - levelNameSize.x) / 2.0f, screenH * 0.65f, levelNameFontSize, WHITE);

        float backBtnW = 400 * globalScale;
        float backBtnH = 50 * globalScale;
        float backBtnX = (screenW - backBtnW) / 2.0f;
        float backBtnY = screenH - 120 * globalScale;
        Rectangle backBtnRect = { backBtnX, backBtnY, backBtnW, backBtnH };

        DrawRectangleRounded(backBtnRect, 0.2f, 8, LIGHTGRAY);
        const char* backText = "Back to Menu";
        Vector2 backTextSize = MeasureTextEx(GetFontDefault(), backText, itemFontSize, 1);
        DrawText(backText, backBtnX + (backBtnW - backTextSize.x) / 2, backBtnY + (backBtnH - backTextSize.y) / 2, itemFontSize, BLACK);
    }

    int GetSelectedLevel() {
        return levelToStart;
    }

    bool BackButtonPressed() {
        return backPressed;
    }
}