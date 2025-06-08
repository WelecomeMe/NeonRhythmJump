#include "Assets.h"
#include <fstream>
#include <string>
#include "raylib.h"

namespace {
    // 辅助函数：在 Image 上绘制基本形状
    void DrawRectangleOnImage(Image& dst, int pX, int pY, int w, int h, Color c) { ImageDrawRectangle(&dst, pX, pY, w, h, c); }
    void DrawTriangleOnImage(Image& dst, Vector2 v1, Vector2 v2, Vector2 v3, Color c) { ImageDrawTriangle(&dst, v1, v2, v3, c); }

    // 辅助函数：创建纯白色的玩家动画雪碧图
    Image GenPlayerSpriteSheet() {
        const int frameWidth = 40;
        const int frameHeight = 40;
        const int totalFrames = 8;
        Image sheet = GenImageColor(frameWidth * totalFrames, frameHeight, BLANK);
        int currentX = 0;

        // 生成纯白色的玩家动画，颜色将由着色器赋予
        DrawRectangleOnImage(sheet, currentX + 2, 2, 36, 36, WHITE);
        DrawRectangleOnImage(sheet, currentX + 8, 8, 24, 24, Fade(BLACK, 0.7f));
        DrawRectangleOnImage(sheet, currentX + 12, 12, 16, 16, WHITE);
        currentX += frameWidth;
        DrawRectangleOnImage(sheet, currentX + 2, 2, 36, 36, WHITE);
        DrawRectangleOnImage(sheet, currentX + 10, 10, 20, 20, Fade(BLACK, 0.7f));
        DrawRectangleOnImage(sheet, currentX + 14, 14, 12, 12, WHITE);
        currentX += frameWidth;
        DrawRectangleOnImage(sheet, currentX + 4, 0, 32, 32, WHITE);
        DrawTriangleOnImage(sheet, { (float)currentX + 4, 32 }, { (float)currentX + 36, 32 }, { (float)currentX + 20, 42 }, LIME);
        currentX += frameWidth;
        DrawRectangleOnImage(sheet, currentX, 6, 40, 28, WHITE);
        currentX += frameWidth;
        for (int i = 0; i < 4; ++i) {
            float offset = (i + 1) * 4.0f;
            DrawRectangleOnImage(sheet, currentX + 10 - offset, 10 - offset, 10, 10, WHITE);
            DrawRectangleOnImage(sheet, currentX + 20 + offset, 10 - offset, 10, 10, WHITE);
            DrawRectangleOnImage(sheet, currentX + 10 - offset, 20 + offset, 10, 10, WHITE);
            DrawRectangleOnImage(sheet, currentX + 20 + offset, 20 + offset, 10, 10, WHITE);
            currentX += frameWidth;
        }
        return sheet;
    }

    // 新增：生成平台爆炸动画的雪碧图
    Image GenExplosionSpriteSheet() {
        const int frameSize = 80;
        const int totalFrames = 5;
        Image sheet = GenImageColor(frameSize * totalFrames, frameSize, BLANK);

        for (int i = 0; i < totalFrames; i++) {
            int currentX = i * frameSize;
            float progress = (float)i / (float)(totalFrames - 1);

            for (int j = 0; j < 30; j++) {
                float angle = (float)j * (360.0f / 30.0f) * DEG2RAD;
                float radius = 35.0f * progress;
                Color particleColor = Fade(ColorLerp(ORANGE, YELLOW, (float)j / 30.0f), 1.0f - progress);

                float px = currentX + frameSize / 2.0f + cos(angle) * radius;
                float py = frameSize / 2.0f + sin(angle) * radius;
                ImageDrawCircle(&sheet, (int)px, (int)py, 3, particleColor);
            }
        }
        return sheet;
    }
}

namespace Assets {
    Music bgm; Music levelMusic; Sound clickSound;
    std::vector<Texture2D> titleFrames; std::vector<Texture2D> bgFrames;
    std::vector<Texture2D> playerRunFrames;
    Texture2D playerJumpUpFrame; Texture2D playerJumpDownFrame;
    std::vector<Texture2D> playerDieFrames;
    std::vector<Texture2D> explosionFrames;
    Shader neonShader; int neonShaderColorLoc;
    float volume = 0.5f; float sfxVolume = 0.5f; bool isFullscreen = false;

    void LoadAll() {
        bgm = LoadMusicStream("Assets/audio/TIME.mp3");
        levelMusic = LoadMusicStream("Assets/audio/luminescence.mp3");
        clickSound = LoadSound("Assets/audio/click.wav");

        std::ifstream config("config.txt");
        if (config) {
            config >> volume >> sfxVolume;
            int fsFlag; config >> fsFlag;
            isFullscreen = (fsFlag == 1);
        }

        for (int i = 1; i <= 88; ++i) {
            std::string path = "Assets/images/title/" + std::to_string(i) + ".png";
            if (FileExists(path.c_str())) titleFrames.push_back(LoadTexture(path.c_str()));
        }
        for (int i = 1; i <= 297; ++i) {
            std::string path = "Assets/images/bg/" + std::to_string(i) + ".png";
            if (FileExists(path.c_str())) bgFrames.push_back(LoadTexture(path.c_str()));
        }

        Image playerSpriteSheet = GenPlayerSpriteSheet();
        const int frameWidth = 40, frameHeight = 40;
        int currentX = 0;
        playerRunFrames.push_back(LoadTextureFromImage(ImageFromImage(playerSpriteSheet, { (float)currentX, 0, (float)frameWidth, (float)frameHeight }))); currentX += frameWidth;
        playerRunFrames.push_back(LoadTextureFromImage(ImageFromImage(playerSpriteSheet, { (float)currentX, 0, (float)frameWidth, (float)frameHeight }))); currentX += frameWidth;
        playerJumpUpFrame = LoadTextureFromImage(ImageFromImage(playerSpriteSheet, { (float)currentX, 0, (float)frameWidth, (float)frameHeight })); currentX += frameWidth;
        playerJumpDownFrame = LoadTextureFromImage(ImageFromImage(playerSpriteSheet, { (float)currentX, 0, (float)frameWidth, (float)frameHeight })); currentX += frameWidth;
        for (int i = 0; i < 4; ++i) {
            playerDieFrames.push_back(LoadTextureFromImage(ImageFromImage(playerSpriteSheet, { (float)currentX, 0, (float)frameWidth, (float)frameHeight })));
            currentX += frameWidth;
        }
        UnloadImage(playerSpriteSheet);

        Image explosionSheet = GenExplosionSpriteSheet();
        for (int i = 0; i < 5; ++i) {
            Rectangle crop = { (float)i * 80, 0, 80, 80 };
            Image frameImage = ImageFromImage(explosionSheet, crop);
            explosionFrames.push_back(LoadTextureFromImage(frameImage));
            UnloadImage(frameImage);
        }
        UnloadImage(explosionSheet);

        neonShader = LoadShader("Assets/shaders/shader.vs", "Assets/shaders/shader.fs");
        neonShaderColorLoc = GetShaderLocation(neonShader, "targetColor");
    }

    void UnloadAll() {
        UnloadMusicStream(bgm); UnloadMusicStream(levelMusic); UnloadSound(clickSound);
        for (auto& tex : titleFrames) UnloadTexture(tex);
        for (auto& tex : bgFrames) UnloadTexture(tex);
        for (auto& tex : playerRunFrames) UnloadTexture(tex);
        UnloadTexture(playerJumpUpFrame); UnloadTexture(playerJumpDownFrame);
        for (auto& tex : playerDieFrames) UnloadTexture(tex);
        for (auto& tex : explosionFrames) UnloadTexture(tex);
        UnloadShader(neonShader);
    }

    void SaveConfig() {
        std::ofstream out("config.txt");
        out << volume << "\n" << sfxVolume << "\n" << (isFullscreen ? 1 : 0);
    }
}