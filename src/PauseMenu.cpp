// PauseMenu.cpp
#include "PauseMenu.h"
#include "Assets.h"
#include "raylib.h"
#include "raymath.h"
#include <string>

namespace {
    // 模块内部状态
    bool resume = false;
    bool restart = false;
    bool exitToMenu = false;

    // --- 为了确保点击和绘制一致，将UI元素矩形定义在这里 ---
    Rectangle resumeBtnRect;
    Rectangle restartBtnRect;
    Rectangle exitBtnRect;
    Rectangle masterVolBar;
    Rectangle sfxVolBar;

    // 辅助函数
    void DrawGlowRectangleLines(Rectangle rect, float thick, Color color, float glowIntensity) {
        for (int i = 1; i <= 5; ++i) {
            DrawRectangleLinesEx({ rect.x - i, rect.y - i, rect.width + i * 2, rect.height + i * 2 }, thick + i, Fade(color, glowIntensity * (0.5f - i * 0.08f)));
        }
        DrawRectangleLinesEx(rect, thick, color);
    }
}

namespace PauseMenu {

    void Init() {
        resume = false;
        restart = false;
        exitToMenu = false;
    }

    void Update() {
        // 每帧开始时重置状态
        resume = false;
        restart = false;
        exitToMenu = false;

        Vector2 mousePos = GetMousePosition();

        // --- 统一计算所有UI元素的位置和大小 ---
        float scrW = (float)GetScreenWidth();
        float scrH = (float)GetScreenHeight();
        float menuWidth = 800;
        float menuHeight = 500;
        Rectangle menuRect = { (scrW - menuWidth) / 2, (scrH - menuHeight) / 2, menuWidth, menuHeight };

        // 按钮位置
        exitBtnRect = { menuRect.x + 100, menuRect.y + 350, 80, 80 };
        resumeBtnRect = { menuRect.x + menuWidth / 2 - 40, menuRect.y + 350, 80, 80 };
        restartBtnRect = { menuRect.x + menuWidth - 180, menuRect.y + 350, 80, 80 };

        // 音量条位置
        masterVolBar = { menuRect.x + 450, menuRect.y + 110, 250, 10 };
        sfxVolBar = { menuRect.x + 450, menuRect.y + 210, 250, 10 };


        // --- 按钮点击逻辑 ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, resumeBtnRect)) resume = true;
            if (CheckCollisionPointRec(mousePos, restartBtnRect)) restart = true;
            if (CheckCollisionPointRec(mousePos, exitBtnRect)) exitToMenu = true;
        }

        // --- 音量滑块逻辑 ---
        // 主音量
        if (CheckCollisionPointRec(mousePos, masterVolBar) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Assets::volume = Clamp((mousePos.x - masterVolBar.x) / masterVolBar.width, 0.0f, 1.0f);
            SetMusicVolume(Assets::bgm, Assets::volume);
        }
        // 游戏音效
        if (CheckCollisionPointRec(mousePos, sfxVolBar) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Assets::sfxVolume = Clamp((mousePos.x - sfxVolBar.x) / sfxVolBar.width, 0.0f, 1.0f);
        }
    }

    void Draw() {
        // 绘制半透明背景遮罩
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

        // 绘制菜单边框
        float menuWidth = 800;
        float menuHeight = 500;
        Rectangle menuRect = { (GetScreenWidth() - menuWidth) / 2, (GetScreenHeight() - menuHeight) / 2, menuWidth, menuHeight };
        DrawGlowRectangleLines(menuRect, 3.0f, WHITE, 0.5f);
        DrawText("PAUSED", menuRect.x + 50, menuRect.y + 50, 60, WHITE);

        // --- ↓↓↓ 这里是关键的对齐修改！ ↓↓↓ ---
        const int fontSize = 16;
        const int iconSize = 40;

        // 退出按钮
        DrawGlowRectangleLines(exitBtnRect, 2.0, WHITE, 0.5);
        DrawText("X", exitBtnRect.x + (exitBtnRect.width - MeasureText("X", iconSize)) / 2, exitBtnRect.y + 12, iconSize, WHITE);
        DrawText("EXIT", exitBtnRect.x + (exitBtnRect.width - MeasureText("EXIT", fontSize)) / 2, exitBtnRect.y + 55, fontSize, WHITE);

        // 继续按钮
        DrawGlowRectangleLines(resumeBtnRect, 2.0, WHITE, 0.5);
        DrawTriangle({ resumeBtnRect.x + 30, resumeBtnRect.y + 20 }, { resumeBtnRect.x + 30, resumeBtnRect.y + 50 }, { resumeBtnRect.x + 60, resumeBtnRect.y + 35 }, WHITE); // 使用实心三角形图标
        DrawText("RESUME", resumeBtnRect.x + (resumeBtnRect.width - MeasureText("RESUME", fontSize)) / 2, resumeBtnRect.y + 55, fontSize, WHITE);

        // 重启按钮
        DrawGlowRectangleLines(restartBtnRect, 2.0, WHITE, 0.5);
        DrawText("R", restartBtnRect.x + (restartBtnRect.width - MeasureText("R", iconSize)) / 2, restartBtnRect.y + 12, iconSize, WHITE);
        DrawText("RESTART", restartBtnRect.x + (restartBtnRect.width - MeasureText("RESTART", fontSize)) / 2, restartBtnRect.y + 55, fontSize, WHITE);

        // --- 绘制音量条 ---
        // 主音量
        DrawText("Master Volume", masterVolBar.x, masterVolBar.y - 30, 20, WHITE);
        DrawRectangleRec({ masterVolBar.x, masterVolBar.y, masterVolBar.width, 5 }, DARKGRAY);
        DrawRectangleRec({ masterVolBar.x, masterVolBar.y, masterVolBar.width * Assets::volume, 5 }, WHITE);
        DrawRectangle(masterVolBar.x + masterVolBar.width * Assets::volume - 5, masterVolBar.y - 2.5f, 10, 10, WHITE);

        // 游戏音效
        DrawText("SFX Volume", sfxVolBar.x, sfxVolBar.y - 30, 20, WHITE);
        DrawRectangleRec({ sfxVolBar.x, sfxVolBar.y, sfxVolBar.width, 5 }, DARKGRAY);
        DrawRectangleRec({ sfxVolBar.x, sfxVolBar.y, sfxVolBar.width * Assets::sfxVolume, 5 }, WHITE);
        DrawRectangle(sfxVolBar.x + sfxVolBar.width * Assets::sfxVolume - 5, sfxVolBar.y - 2.5f, 10, 10, WHITE);
    }

    // 实现查询函数
    bool ShouldResume() { return resume; }
    bool ShouldRestart() { return restart; }
    bool ShouldExitToMenu() { return exitToMenu; }
}