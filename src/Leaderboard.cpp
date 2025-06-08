// Leaderboard.cpp
#include "Leaderboard.h"
#include "raylib.h"
#include <string>
#include <cmath> // ✅ 包含 cmath

namespace {
    bool backPressed = false; // 模块内部状态
}

namespace Leaderboard {

    // ✅ 新增 Update 函数
    void Update() {
        backPressed = false;

        Vector2 mouse = GetMousePosition();
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float globalScale = fminf(screenW / 1200.0f, screenH / 800.0f);

        float btnWidth = 400 * globalScale;
        float btnHeight = 50 * globalScale;
        float btnX = (screenW - btnWidth) / 2.0f;
        float btnY = screenH - 120 * globalScale;

        Rectangle backBtnRect = { btnX, btnY, btnWidth, btnHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, backBtnRect)) {
            backPressed = true;
        }
    }

    void Draw(const std::vector<ScoreEntry>& scores) {
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float globalScale = fminf(screenW / 1200.0f, screenH / 800.0f);

        float fontSize = 30 * globalScale;
        float x = 150 * globalScale;
        float y = 150 * globalScale;

        DrawText("Leaderboard", x, y, (int)(fontSize * 1.2f), YELLOW);
        for (size_t i = 0; i < scores.size(); ++i) {
            DrawText(TextFormat("%d.", i + 1), x, y + 60 * globalScale + i * 40 * globalScale, fontSize, LIGHTGRAY);
            DrawText(scores[i].playerName.c_str(), x + 50 * globalScale, y + 60 * globalScale + i * 40 * globalScale, fontSize, WHITE);
            DrawText(TextFormat("%d", scores[i].score), x + 400 * globalScale, y + 60 * globalScale + i * 40 * globalScale, fontSize, GOLD);
        }

        // --- ✅ 绘制居中的 Back 按钮 ---
        float btnWidth = 400 * globalScale;
        float btnHeight = 50 * globalScale;
        float itemFontSize = 22 * globalScale;
        float btnX = (screenW - btnWidth) / 2.0f;
        float btnY = screenH - 120 * globalScale;

        Rectangle backBtnRect = { btnX, btnY, btnWidth, btnHeight };

        DrawRectangleRounded(backBtnRect, 0.2f, 8, LIGHTGRAY);
        const char* backText = "Back";
        Vector2 backTextSize = MeasureTextEx(GetFontDefault(), backText, itemFontSize, 1);
        DrawText(backText, btnX + (btnWidth - backTextSize.x) / 2, btnY + (btnHeight - backTextSize.y) / 2, itemFontSize, BLACK);
    }

    // ✅ 新增 BackButtonPressed 函数
    bool BackButtonPressed() {
        return backPressed;
    }
}