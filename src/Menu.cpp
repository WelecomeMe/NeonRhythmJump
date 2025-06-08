// Menu.cpp
#include "Menu.h"
#include "Globals.h"
#include "raylib.h"
#include <vector>
#include <cmath>

namespace {
    std::vector<MenuButton> buttons;
}

namespace Menu {
    void Init() {
        buttons = {
            {"Start Game", SKYBLUE},
            {"Credits", VIOLET},
            {"Statistics", YELLOW},
            {"Settings", ORANGE},
            {"Quit", RED}
        };
    }

    void Update() {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();
        float globalScale = fminf((float)GetScreenWidth() / 1200.0f, (float)GetScreenHeight() / 800.0f);
        float baseBtnWidth = 300 * globalScale;
        float baseBtnHeight = 60 * globalScale;
        float spacing = 20 * globalScale;
        float xOffset = GetScreenWidth() - baseBtnWidth - 100 * globalScale;
        float yStart = GetScreenHeight() / 2.0f - (baseBtnHeight + spacing) * buttons.size() / 2.0f;

        for (int i = 0; i < buttons.size(); ++i) {
            auto& btn = buttons[i];
            if (btn.animOffset < 0) btn.animOffset += 800 * dt;
            if (btn.animOffset > 0) btn.animOffset = 0;

            Rectangle rect = { xOffset + btn.animOffset * globalScale, yStart + i * (baseBtnHeight + spacing), baseBtnWidth, baseBtnHeight * btn.currentScale };
            bool hovered = CheckCollisionPointRec(mouse, rect);
            btn.targetScale = hovered ? 1.2f + 0.05f * sinf(GetTime() * 6.0f) : 1.0f;

            if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                btn.clicked = true;
                btn.clickTime = 0.0f;
            }
            if (btn.clicked) {
                btn.clickTime += dt;
                btn.targetScale = 0.9f;
            }

            btn.pulseTime += dt;
            btn.targetScale += 0.01f * sinf(btn.pulseTime * 3.0f);
            btn.currentScale += (btn.targetScale - btn.currentScale) * 0.1f;
        }
    }

    void Draw() {
        float globalScale = fminf((float)GetScreenWidth() / 1200.0f, (float)GetScreenHeight() / 800.0f);
        float baseBtnWidth = 300 * globalScale;
        float baseBtnHeight = 60 * globalScale;
        float spacing = 20 * globalScale;
        float fontSize = 24 * globalScale;
        float xOffset = GetScreenWidth() - baseBtnWidth - 100 * globalScale;
        float yStart = GetScreenHeight() / 2.0f - (baseBtnHeight + spacing) * buttons.size() / 2.0f;

        for (int i = 0; i < buttons.size(); ++i) {
            MenuButton& btn = buttons[i];
            Rectangle rect = { xOffset + btn.animOffset * globalScale, yStart + i * (baseBtnHeight + spacing), baseBtnWidth, baseBtnHeight * btn.currentScale };
            float h = baseBtnHeight * btn.currentScale;
            Color color = btn.glowColor;

            for (int j = 1; j <= 3; j++) {
                Rectangle glowRect = { rect.x - j * 2 * globalScale, rect.y - j * 2 * globalScale, rect.width + j * 4 * globalScale, rect.height + j * 4 * globalScale };
                DrawRectangleRounded(glowRect, 0.2f, 8, Fade(color, 0.1f * (4 - j)));
            }
            for (int j = 0; j < 5; j++) {
                float t = GetTime() * 4.0f + j;
                float offset = sinf(t) * 6.0f * globalScale;
                DrawLine(rect.x + offset, rect.y, rect.x + offset, rect.y + rect.height, Fade(color, 0.2f));
            }
            DrawRectangleRounded(rect, 0.2f, 8, Fade(color, 0.5f));
            DrawRectangleLinesEx(rect, 2 * globalScale, color);
            DrawText(btn.label, rect.x + 20 * globalScale, rect.y + h / 2 - fontSize / 2, (int)fontSize, WHITE);
        }
    }

    int GetClickedButton() {
        for (int i = 0; i < buttons.size(); ++i) {
            if (buttons[i].clicked && buttons[i].clickTime > 0.15f) {
                buttons[i].clicked = false;
                return i;
            }
        }
        return -1;
    }
}