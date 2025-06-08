// VictoryScreen.cpp
#include "VictoryScreen.h"
#include "raylib.h"
#include <string>

namespace {
    int deaths = 0;
    VictoryScreen::Option clickedOption = VictoryScreen::NONE;
}

namespace VictoryScreen {
    void Init(int deathCount) {
        deaths = deathCount;
        clickedOption = NONE;
    }

    void Update() {
        clickedOption = NONE;
        Vector2 mouse = GetMousePosition();

        float btnWidth = 300;
        float btnHeight = 50;
        float centerX = (GetScreenWidth() - btnWidth) / 2.0f;

        Rectangle restartBtn = { centerX, GetScreenHeight() * 0.6f, btnWidth, btnHeight };
        Rectangle menuBtn = { centerX, GetScreenHeight() * 0.75f, btnWidth, btnHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, restartBtn)) clickedOption = RESTART_LEVEL;
            if (CheckCollisionPointRec(mouse, menuBtn)) clickedOption = BACK_TO_MENU;
        }
    }

    void Draw() {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

        const char* title = "LEVEL COMPLETE!";
        int titleSize = 80;
        DrawText(title, (GetScreenWidth() - MeasureText(title, titleSize)) / 2, GetScreenHeight() * 0.2f, titleSize, GOLD);

        std::string deathText = "Deaths: " + std::to_string(deaths);
        int deathTextSize = 30;
        DrawText(deathText.c_str(), (GetScreenWidth() - MeasureText(deathText.c_str(), deathTextSize)) / 2, GetScreenHeight() * 0.4f, deathTextSize, RAYWHITE);

        // ªÊ÷∆∞¥≈•
        Rectangle restartBtn = { (GetScreenWidth() - 300.0f) / 2.0f, GetScreenHeight() * 0.6f, 300, 50 };
        Rectangle menuBtn = { (GetScreenWidth() - 300.0f) / 2.0f, GetScreenHeight() * 0.75f, 300, 50 };

        DrawRectangleRounded(restartBtn, 0.2f, 8, SKYBLUE);
        DrawText("Restart Level", restartBtn.x + (restartBtn.width - MeasureText("Restart Level", 20)) / 2, restartBtn.y + 15, 20, BLACK);

        DrawRectangleRounded(menuBtn, 0.2f, 8, LIGHTGRAY);
        DrawText("Back to Menu", menuBtn.x + (menuBtn.width - MeasureText("Back to Menu", 20)) / 2, menuBtn.y + 15, 20, BLACK);
    }

    Option GetClickedOption() {
        return clickedOption;
    }
}