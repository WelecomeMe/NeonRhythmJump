// Credits.cpp
#include "Credits.h"
#include "raylib.h"
#include <cmath>

namespace {
    bool backPressed = false; // 模块内部状态，用于记录按钮是否被按下
}

namespace Credits {

    // ✅ 新增 Update 函数
    void Update() {
        backPressed = false; // 每帧开始时重置状态

        Vector2 mouse = GetMousePosition();
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float globalScale = fminf(screenW / 1200.0f, screenH / 800.0f);

        float btnWidth = 400 * globalScale;
        float btnHeight = 50 * globalScale;
        float btnX = (screenW - btnWidth) / 2.0f;
        float btnY = screenH - 120 * globalScale; // 按钮位置

        Rectangle backBtnRect = { btnX, btnY, btnWidth, btnHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, backBtnRect)) {
            backPressed = true;
        }
    }

    void Draw() {
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float globalScale = fminf(screenW / 1200.0f, screenH / 800.0f);

        float titleFontSize = 40 * globalScale;
        float headerFontSize = 26 * globalScale;
        float itemFontSize = 22 * globalScale;

        float startX = screenW * 0.2f;
        float startY = screenH * 0.2f;
        float lineSpacing = 40 * globalScale;
        float sectionSpacing = 60 * globalScale;

        // ... (绘制 Credits 名单的代码保持不变) ...
        DrawText("Credits", startX, startY, titleFontSize, GOLD);
        float currentY = startY + sectionSpacing * 2;
        DrawText("Main Members (Core Development)", startX, currentY, headerFontSize, SKYBLUE);
        currentY += lineSpacing * 1.5f;
        DrawText("A: Lead Developer (Ding)", startX + 20 * globalScale, currentY, itemFontSize, RAYWHITE);
        currentY += lineSpacing;
        DrawText("B: Lead Designer (Jiang)", startX + 20 * globalScale, currentY, itemFontSize, RAYWHITE);
        currentY += sectionSpacing * 1.5f;
        DrawText("Support Members (Collaboration)", startX, currentY, headerFontSize, GREEN);
        currentY += lineSpacing * 1.5f;
        DrawText("C: QA & Feedback (Wang)", startX + 20 * globalScale, currentY, itemFontSize, RAYWHITE);
        currentY += lineSpacing;
        DrawText("D: Documentation (Gu)", startX + 20 * globalScale, currentY, itemFontSize, RAYWHITE);


        // --- ✅ 绘制居中的 Back 按钮，替换原来的 ESC 提示 ---
        float btnWidth = 400 * globalScale;
        float btnHeight = 50 * globalScale;
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