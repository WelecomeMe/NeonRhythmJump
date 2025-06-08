// Settings.cpp
#include "Settings.h"
#include "Assets.h"
#include "Globals.h"
#include "raylib.h"
#include "raymath.h" // ✅ 添加这个头文件以使用 Clamp 函数
#include <cmath>

namespace {
    bool draggingVolume = false;
    bool draggingSFX = false;
    bool backPressed = false;
}

namespace SettingsUI {
    void Update() {
        backPressed = false;
        Vector2 mouse = GetMousePosition();
        float globalScale = fminf((float)GetScreenWidth() / 1200.0f, (float)GetScreenHeight() / 800.0f);
        float baseBtnWidth = 400 * globalScale;
        float baseBtnHeight = 50 * globalScale;
        float xOffset = (GetScreenWidth() - baseBtnWidth) / 2.0f;

        Rectangle volumeBar = { xOffset, GetScreenHeight() * 0.35f, baseBtnWidth, 20 * globalScale };
        Rectangle sfxBar = { xOffset, GetScreenHeight() * 0.5f, baseBtnWidth, 20 * globalScale };
        Rectangle fsBtn = { xOffset, GetScreenHeight() * 0.65f, baseBtnWidth, baseBtnHeight };
        Rectangle backBtn = { xOffset, GetScreenHeight() - 80 * globalScale - baseBtnHeight, baseBtnWidth, baseBtnHeight };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, volumeBar)) draggingVolume = true;
        if (draggingVolume) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                // ✅ 现在 Clamp 函数可以被正确找到了
                Assets::volume = Clamp((mouse.x - volumeBar.x) / volumeBar.width, 0.0f, 1.0f);
                SetMusicVolume(Assets::bgm, Assets::volume);
            }
            else draggingVolume = false;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, sfxBar)) draggingSFX = true;
        if (draggingSFX) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                // ✅ 现在 Clamp 函数可以被正确找到了
                Assets::sfxVolume = Clamp((mouse.x - sfxBar.x) / sfxBar.width, 0.0f, 1.0f);
            }
            else draggingSFX = false;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, fsBtn)) {
            ToggleFullscreen();
            Assets::isFullscreen = !Assets::isFullscreen;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, backBtn)) {
            backPressed = true;
        }
    }

    void Draw() {
        // ... Draw 函数没有使用 Clamp，所以保持不变 ...
        float globalScale = fminf((float)GetScreenWidth() / 1200.0f, (float)GetScreenHeight() / 800.0f);
        float baseBtnWidth = 400 * globalScale;
        float baseBtnHeight = 50 * globalScale;
        float fontSize = 24 * globalScale;
        float xOffset = (GetScreenWidth() - baseBtnWidth) / 2.0f;

        Rectangle volumeBar = { xOffset, GetScreenHeight() * 0.35f, baseBtnWidth, 20 * globalScale };
        Rectangle sfxBar = { xOffset, GetScreenHeight() * 0.5f, baseBtnWidth, 20 * globalScale };
        Rectangle fsBtn = { xOffset, GetScreenHeight() * 0.65f, baseBtnWidth, baseBtnHeight };
        Rectangle backBtn = { xOffset, GetScreenHeight() - 80 * globalScale - baseBtnHeight, baseBtnWidth, baseBtnHeight };

        DrawText("Settings", xOffset, GetScreenHeight() * 0.15f, 32 * globalScale, ORANGE);

        DrawText("Music Volume", volumeBar.x, volumeBar.y - 30 * globalScale, fontSize, WHITE);
        DrawRectangleRec(volumeBar, Fade(GRAY, 0.5f));
        DrawRectangleRec({ volumeBar.x, volumeBar.y, Assets::volume * volumeBar.width, volumeBar.height }, ORANGE);

        DrawText("SFX Volume", sfxBar.x, sfxBar.y - 30 * globalScale, fontSize, WHITE);
        DrawRectangleRec(sfxBar, Fade(GRAY, 0.5f));
        DrawRectangleRec({ sfxBar.x, sfxBar.y, Assets::sfxVolume * sfxBar.width, sfxBar.height }, ORANGE);

        DrawRectangleRounded(fsBtn, 0.2f, 8, DARKGRAY);
        const char* fsText = Assets::isFullscreen ? "Windowed Mode" : "Fullscreen Mode";
        Vector2 fsTextSize = MeasureTextEx(GetFontDefault(), fsText, fontSize, 1);
        DrawText(fsText, fsBtn.x + (fsBtn.width - fsTextSize.x) / 2, fsBtn.y + (fsBtn.height - fsTextSize.y) / 2, fontSize, WHITE);

        DrawRectangleRounded(backBtn, 0.2f, 8, LIGHTGRAY);
        const char* backText = "Back";
        Vector2 backTextSize = MeasureTextEx(GetFontDefault(), backText, fontSize, 1);
        DrawText(backText, backBtn.x + (backBtn.width - backTextSize.x) / 2, backBtn.y + (backBtn.height - backTextSize.y) / 2, fontSize, BLACK);
    }

    bool BackButtonPressed() {
        return backPressed;
    }
}