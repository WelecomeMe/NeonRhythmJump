#include "Playing.h"
#include "Globals.h"
#include "Assets.h"
#include "PauseMenu.h"
#include "raylib.h"
#include "raymath.h"
#include "json.hpp"
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

using json = nlohmann::json;

namespace {
    // --- 调色板 ---
    const Color SKY_TOP = { 23, 20, 54, 255 };
    const Color SKY_BOTTOM = { 48, 52, 129, 255 };
    const Color NEON_MAGENTA = { 239, 61, 196, 255 };
    const Color FAR_BUILDING_COLOR = { 35, 35, 88, 255 };
    const Color MID_BUILDING_COLOR = { 45, 42, 100, 255 };
    const Color WINDOW_COLOR = { 255, 200, 244, 255 };
    const Color ATMOSPHERE_HAZE = { 76, 73, 169, 255 };

    // --- 游戏常量 ---
    const float GRAVITY = 1200.0f;
    const float PLAYER_JUMP_FORCE = -600.0f;
    const float PLAYER_HORIZONTAL_SPEED = 350.0f;

    // --- 游戏对象与背景结构体 ---
    struct Player {
        Vector2 position;
        Vector2 velocity;
        Rectangle rect;
        bool canJump;
        bool isDead;
        float animationTimer;
        int currentFrame;
    };

    struct Obstacle {
        Rectangle rect;
        ObstacleType type;
        bool active;
        Vector2 velocity;
        float moveRange;
        float initialX;
        bool isTouched;
        float explodeTimer;
        bool isSignal;
        bool isExploding;
        float explosionAnimTimer;
        int explosionCurrentFrame;
    };

    struct Star {
        Vector2 position;
        float radius;
        Color color;
    };

    struct Window {
        Rectangle rect;
        bool lit;
    };

    struct Building {
        Rectangle rect;
        std::vector<Window> windows;
    };

    // --- 模块内部状态 ---
    Player player;
    std::vector<LevelEvent> levelEvents;
    std::vector<Obstacle> activeObstacles;
    Camera2D camera;
    Rectangle finishLine;
    bool isAutoRunning = false;
    float autoRunTimer = 0.0f;
    float autoRunSpeedMultiplier = 1.5f;
    int nextEventIndex = 0;
    bool levelLoaded = false;
    bool levelFinished = false;
    int deathCount = 0;
    float restartTimer = 0.0f;
    bool isPaused = false;
    float neonColorTimer = 0.0f;
    std::vector<Star> stars;
    std::vector<Building> farBuildings;
    std::vector<Building> midBuildings;

    // --- 辅助函数 ---
    void DrawGlowRectangleLines(Rectangle rect, float thick, Color color, float glowIntensity) {
        for (int i = 1; i <= 5; ++i) {
            DrawRectangleLinesEx({ rect.x - i * 0.5f, rect.y - i * 0.5f, rect.width + i, rect.height + i }, thick, Fade(color, glowIntensity * (0.5f - i * 0.08f)));
        }
        DrawRectangleLinesEx(rect, thick, color);
    }

    void DrawGlowText(const char* text, int pX, int pY, int fS, Color c, float gI) {
        DrawText(text, pX + 1, pY, fS, Fade(c, gI * 0.4f));
        DrawText(text, pX - 1, pY, fS, Fade(c, gI * 0.4f));
        DrawText(text, pX, pY + 1, fS, Fade(c, gI * 0.4f));
        DrawText(text, pX, pY - 1, fS, Fade(c, gI * 0.4f));
        DrawText(text, pX, pY, fS, c);
    }

    Color GetCurrentNeonColor() {
        Color neonColors[] = { {0, 255, 255, 255}, {255, 0, 255, 255}, {0, 255, 128, 255}, {255, 255, 0, 255}, {255, 128, 0, 255}, {200, 0, 255, 255} };
        int numColors = sizeof(neonColors) / sizeof(neonColors[0]);
        float t = fmod(neonColorTimer * 0.1f, (float)numColors);
        int index1 = (int)t;
        int index2 = (index1 + 1) % numColors;
        float blend = t - index1;
        return ColorLerp(neonColors[index1], neonColors[index2], blend);
    }

    ObstacleType StringToObstacleType(const std::string& str) {
        if (str == "STATIC_PLATFORM") return ObstacleType::STATIC_PLATFORM;
        if (str == "MOVING_PLATFORM") return ObstacleType::MOVING_PLATFORM;
        if (str == "SPIKE") return ObstacleType::SPIKE;
        if (str == "EXPLODING_PLATFORM") return ObstacleType::EXPLODING_PLATFORM;
        if (str == "BOOST_GATE") return ObstacleType::BOOST_GATE;
        return ObstacleType::NONE;
    }

    void SpawnObstacle(const LevelEvent& event) {
        Obstacle obs = {};
        obs.isSignal = false; obs.active = true; obs.type = event.type;
        obs.isExploding = false; obs.explosionAnimTimer = 0.0f; obs.explosionCurrentFrame = 0;
        switch (event.type) {
        case ObstacleType::STATIC_PLATFORM: {
            float width = event.position.z > 0 ? event.position.z : 200.0f;
            obs.rect = { event.position.x, event.position.y, width, 20 };
        } break;
        case ObstacleType::MOVING_PLATFORM:
            obs.rect = { event.position.x, event.position.y, 180, 20 };
            obs.moveRange = event.position.z; obs.velocity.x = event.position.w; obs.initialX = event.position.x;
            break;
        case ObstacleType::SPIKE:
            obs.rect = { event.position.x, event.position.y, 40, 40 }; break;
        case ObstacleType::EXPLODING_PLATFORM:
            obs.rect = { event.position.x, event.position.y, 150, 20 };
            obs.isTouched = false; obs.explodeTimer = event.position.z > 0 ? event.position.z : 1.0f;
            break;
        case ObstacleType::BOOST_GATE:
            obs.rect = { event.position.x, event.position.y - 200, 20, 200 };
            obs.isSignal = true; break;
        default: obs.active = false; break;
        }
        if (obs.active) activeObstacles.push_back(obs);
    }
}

namespace Playing {
    void LoadLevel(const std::string& levelFilePath) {
        std::ifstream f(levelFilePath);
        if (!f.is_open()) { levelLoaded = false; return; }
        try {
            json data = json::parse(f);
            float finishLineX = data.value("finishLineX", 10000.0f);
            finishLine = { finishLineX, -1000, 50, 2000 };
            levelEvents.clear();
            if (data.contains("events")) {
                for (const auto& ev_j : data["events"]) {
                    LevelEvent ev = {};
                    ev.type = StringToObstacleType(ev_j.value("type", ""));
                    if (ev_j.contains("position")) {
                        ev.position.x = ev_j["position"].value("x", 0.0f);
                        ev.position.y = ev_j["position"].value("y", 0.0f);
                    }
                    ev.position.z = ev_j.value("param1", 0.0f);
                    ev.position.w = ev_j.value("param2", 0.0f);
                    levelEvents.push_back(ev);
                }
            }
            std::sort(levelEvents.begin(), levelEvents.end(), [](const LevelEvent& a, const LevelEvent& b) {
                return a.position.x < b.position.x;
                });
            levelLoaded = true;
        }
        catch (const json::parse_error&) { levelLoaded = false; }
    }

    void Init() {
        player = {};
        player.position = { -200, 500 }; player.rect = { player.position.x, player.position.y, 40, 40 };
        player.isDead = false; player.canJump = false; player.animationTimer = 0.0f; player.currentFrame = 0;
        activeObstacles.clear(); nextEventIndex = 0; levelFinished = false;
        deathCount = 0; restartTimer = 0.0f; isPaused = false;
        isAutoRunning = false; autoRunTimer = 0.0f;
        neonColorTimer = 0.0f;
        camera = {};
        camera.offset = { (float)GetScreenWidth() / 4.0f, (float)GetScreenHeight() / 2.0f };
        camera.target = player.position; camera.rotation = 0.0f; camera.zoom = 1.0f;

        
        stars.clear(); farBuildings.clear(); midBuildings.clear();

       
        for (int i = 0; i < 200; ++i) {
            Color starColor = (GetRandomValue(0, 10) > 8) ? Fade(NEON_MAGENTA, 0.6f) : Fade(SKY_BOTTOM, 0.8f);
            stars.push_back({
                {(float)GetRandomValue(-5000, 20000), (float)GetRandomValue(0, GetScreenHeight() / 2)},
                (float)GetRandomValue(1, 2) * 0.5f, starColor
                });
        }

       
        for (int i = 0; i < 120; i++) {
            float x_pos = (float)GetRandomValue(-5000, 20000);
            float width = (float)GetRandomValue(150, 350);
            float height = (float)GetRandomValue(50, 150);
            float y_pos = GetScreenHeight() - height;
            farBuildings.push_back({ {x_pos, y_pos, width, height}, {} });
        }

      
        const float WINDOW_WIDTH = 5.0f;
        const float WINDOW_HEIGHT = 3.0f;
        const float WINDOW_SPACING = 3.0f;
        for (int i = 0; i < 80; i++) {
            float x_pos = (float)GetRandomValue(-5000, 20000);
            float core_width = (float)GetRandomValue(80, 160);
            float core_height = (float)GetRandomValue(150, 300);
            float core_y = GetScreenHeight() - core_height;

            Building coreBuilding = { {x_pos, core_y, core_width, core_height} };

            int num_floors = (core_height - WINDOW_SPACING) / (WINDOW_HEIGHT + WINDOW_SPACING);
            int num_rooms = (core_width - WINDOW_SPACING) / (WINDOW_WIDTH + WINDOW_SPACING);
            for (int r = 0; r < num_rooms; ++r) {
                for (int f = 0; f < num_floors; ++f) {
                    if (GetRandomValue(0, 15) == 0) continue;
                    Window win;
                    win.rect.x = x_pos + WINDOW_SPACING + r * (WINDOW_WIDTH + WINDOW_SPACING);
                    win.rect.y = core_y + WINDOW_SPACING + f * (WINDOW_HEIGHT + WINDOW_SPACING);
                    win.rect.width = WINDOW_WIDTH;
                    win.rect.height = WINDOW_HEIGHT;
                    win.lit = GetRandomValue(0, 6) < 2;
                    coreBuilding.windows.push_back(win);
                }
            }
            midBuildings.push_back(coreBuilding);
        }
    }

    void Update() {
        if (!levelLoaded || levelFinished) return;
        float dt = GetFrameTime();
        neonColorTimer += dt;

        if (IsKeyPressed(KEY_ESCAPE)) {
            isPaused = !isPaused;
            if (isPaused) { PauseMenu::Init(); PauseMusicStream(Assets::levelMusic); }
            else { ResumeMusicStream(Assets::levelMusic); }
        }
        if (isPaused) {
            PauseMenu::Update();
            if (PauseMenu::ShouldResume()) { isPaused = false; ResumeMusicStream(Assets::levelMusic); }
            if (PauseMenu::ShouldRestart()) Init();
            return;
        }
        if (player.isDead) {
            player.animationTimer += dt;
            if (!Assets::playerDieFrames.empty()) {
                player.currentFrame = (int)(player.animationTimer / 0.1f);
                if (player.currentFrame >= Assets::playerDieFrames.size()) player.currentFrame = Assets::playerDieFrames.size() - 1;
            }
            restartTimer += dt;
            if (restartTimer > 2.0f) { int d = deathCount; Init(); deathCount = d + 1; }
            return;
        }
        if (player.canJump) {
            player.animationTimer += dt;
            if (player.animationTimer >= 0.2f) {
                player.animationTimer = 0.0f;
                if (!Assets::playerRunFrames.empty()) player.currentFrame = (player.currentFrame + 1) % Assets::playerRunFrames.size();
            }
        }
        const float LOOK_AHEAD_DISTANCE = GetScreenWidth() * 1.5f;
        while (nextEventIndex < levelEvents.size() && levelEvents[nextEventIndex].position.x < player.position.x + LOOK_AHEAD_DISTANCE) {
            SpawnObstacle(levelEvents[nextEventIndex]);
            nextEventIndex++;
        }
        if (isAutoRunning) {
            player.velocity.x = PLAYER_HORIZONTAL_SPEED * autoRunSpeedMultiplier;
            autoRunTimer -= dt;
            if (autoRunTimer <= 0) isAutoRunning = false;
        }
        else player.velocity.x = PLAYER_HORIZONTAL_SPEED;
        if (IsKeyPressed(KEY_SPACE) && player.canJump) { player.velocity.y = PLAYER_JUMP_FORCE; player.canJump = false; PlaySound(Assets::clickSound); }
        player.position.x += player.velocity.x * dt;
        player.rect.x = player.position.x;
        for (auto& obs : activeObstacles) {
            if (!obs.active || obs.type == ObstacleType::SPIKE || obs.isSignal) continue;
            if (CheckCollisionRecs(player.rect, obs.rect)) {
                if (player.velocity.x > 0) player.rect.x = obs.rect.x - player.rect.width;
                else if (player.velocity.x < 0) player.rect.x = obs.rect.x + obs.rect.width;
                player.position.x = player.rect.x;
            }
        }
        player.velocity.y += GRAVITY * dt;
        player.position.y += player.velocity.y * dt;
        player.rect.y = player.position.y;
        player.canJump = false;
        for (auto& obs : activeObstacles) {
            if (obs.isExploding) {
                obs.explosionAnimTimer += dt;
                if (!Assets::explosionFrames.empty()) {
                    obs.explosionCurrentFrame = (int)(obs.explosionAnimTimer / 0.08f);
                    if (obs.explosionCurrentFrame >= Assets::explosionFrames.size()) obs.isExploding = false;
                }
            }
            if (!obs.active) continue;
            if (obs.type == ObstacleType::EXPLODING_PLATFORM && obs.isTouched) {
                obs.explodeTimer -= dt;
                if (obs.explodeTimer <= 0) {
                    obs.active = false;
                    obs.isExploding = true;
                }
            }
            if (obs.type == ObstacleType::MOVING_PLATFORM) {
                obs.rect.x += obs.velocity.x * dt;
                if (fabs(obs.rect.x - obs.initialX) >= obs.moveRange) obs.velocity.x *= -1;
            }
            if (CheckCollisionRecs(player.rect, obs.rect)) {
                if (obs.type == ObstacleType::SPIKE) { player.isDead = true; PlaySound(Assets::clickSound); break; }
                if (obs.isSignal) continue;
                if (player.velocity.y > 0) {
                    player.rect.y = obs.rect.y - player.rect.height;
                    player.velocity.y = 0; player.canJump = true;
                    if (obs.type == ObstacleType::EXPLODING_PLATFORM) obs.isTouched = true;
                }
                else if (player.velocity.y < 0) {
                    player.rect.y = obs.rect.y + obs.rect.height;
                    player.velocity.y = 0;
                }
                player.position.y = player.rect.y;
            }
        }
        if (player.isDead) return;
        for (auto& obs : activeObstacles) {
            if (obs.active && obs.type == ObstacleType::BOOST_GATE && CheckCollisionRecs(player.rect, obs.rect)) {
                for (const auto& ev : levelEvents) {
                    if (ev.type == ObstacleType::BOOST_GATE && ev.position.x == obs.rect.x && ev.position.y == obs.rect.y + 200) {
                        isAutoRunning = true; autoRunTimer = ev.position.z > 0 ? ev.position.z : 3.0f;
                        autoRunSpeedMultiplier = ev.position.w > 0 ? ev.position.w : 1.5f;
                        obs.active = false; break;
                    }
                }
            }
        }
        if (player.position.y > GetScreenHeight() * 2) player.isDead = true;
        if (!player.isDead && CheckCollisionRecs(player.rect, finishLine)) levelFinished = true;
        camera.target.x = player.position.x;
    }

    void Draw() {
        if (!levelLoaded) {
            ClearBackground(BLACK);
            DrawText("Level could not be loaded.", 20, 20, 20, RED);
            return;
        }

        
        DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), SKY_BOTTOM, SKY_TOP);

        Camera2D bgCam = { 0 };
        bgCam.offset = { 0, 0 }; bgCam.rotation = 0.0f; bgCam.zoom = 1.0f;

      
        bgCam.target = { camera.target.x * 0.1f, 0 };
        BeginMode2D(bgCam);
        for (const auto& star : stars) DrawCircleV(star.position, star.radius, star.color);
        EndMode2D();

        
        bgCam.target = { camera.target.x * 0.2f, 0 };
        BeginMode2D(bgCam);
        for (const auto& b : farBuildings) DrawRectangleRec(b.rect, FAR_BUILDING_COLOR);
        EndMode2D();

        
        bgCam.target = { camera.target.x * 0.4f, 0 };
        BeginMode2D(bgCam);
        for (const auto& b : midBuildings) {
            DrawRectangleRec(b.rect, MID_BUILDING_COLOR);
            for (const auto& win : b.windows) {
                if (win.lit) {
                    DrawRectangleRec(win.rect, WINDOW_COLOR);
                    DrawRectangle(win.rect.x - 1, win.rect.y - 1, win.rect.width + 2, win.rect.height + 2, Fade(WINDOW_COLOR, 0.3f));
                }
                else {
                    DrawRectangleRec(win.rect, Fade(BLACK, 0.5f));
                }
            }
        }
        EndMode2D();

        
        DrawRectangleGradientV(0, GetScreenHeight() * 0.4f, GetScreenWidth(), GetScreenHeight() * 0.6f, Fade(ATMOSPHERE_HAZE, 0.0f), Fade(ATMOSPHERE_HAZE, 0.7f));

        
        BeginMode2D(camera);
        {
            
            Texture2D playerTex = { 0 };
            if (player.isDead) {
                if (!Assets::playerDieFrames.empty() && player.currentFrame < Assets::playerDieFrames.size()) playerTex = Assets::playerDieFrames[player.currentFrame];
            }
            else if (!player.canJump) {
                playerTex = (player.velocity.y < 0) ? Assets::playerJumpUpFrame : Assets::playerJumpDownFrame;
            }
            else {
                if (!Assets::playerRunFrames.empty()) playerTex = Assets::playerRunFrames[player.currentFrame];
            }
            if (playerTex.id > 0) {
                Vector4 colorVec = ColorNormalize(GetCurrentNeonColor());
                BeginShaderMode(Assets::neonShader);
                SetShaderValue(Assets::neonShader, Assets::neonShaderColorLoc, &colorVec, SHADER_UNIFORM_VEC4);
                DrawTexture(playerTex, (int)player.position.x, (int)player.position.y, WHITE);
                EndShaderMode();
            }

            
            for (auto& obs : activeObstacles) {
                if (obs.isExploding) {
                    if (!Assets::explosionFrames.empty() && obs.explosionCurrentFrame < Assets::explosionFrames.size()) {
                        Texture2D explosionTex = Assets::explosionFrames[obs.explosionCurrentFrame];
                        DrawTexture(explosionTex, obs.rect.x + obs.rect.width / 2 - 40, obs.rect.y + obs.rect.height / 2 - 40, WHITE);
                    }
                }
                if (!obs.active) continue;

                if (obs.type == ObstacleType::SPIKE) {
                    DrawTriangle({ obs.rect.x + obs.rect.width / 2, obs.rect.y }, { obs.rect.x, obs.rect.y + obs.rect.height }, { obs.rect.x + obs.rect.width, obs.rect.y + obs.rect.height }, RED);
                }
                else if (obs.type == ObstacleType::BOOST_GATE) {

                    Color gateColor = GetCurrentNeonColor();

                    DrawRectangleRec(obs.rect, Fade(gateColor, 0.2f));


                    DrawRectangleLinesEx(obs.rect, 2.0f, gateColor);


                    for (int i = 0; i < 3; ++i) {
        
                        float textY = obs.rect.y + obs.rect.height / 2 - 30 + (i * 25);
                        DrawGlowText(">>", obs.rect.x + 4, textY, 20, WHITE, 0.7f);
                    }
                }
                else {
                    Color platformColor = GetCurrentNeonColor();
                    if (obs.type == ObstacleType::EXPLODING_PLATFORM && obs.isTouched) {
                        platformColor = ColorLerp(RED, WHITE, obs.explodeTimer / 1.0f);
                    }
                    DrawGlowRectangleLines(obs.rect, 2.0f, platformColor, 0.7f);
                }
            }
            DrawRectangleRec(finishLine, Fade(GOLD, 0.5f));
        }
        EndMode2D();

        // 7. 绘制UI
        float currentTime = GetMusicTimePlayed(Assets::levelMusic);
        DrawGlowText(TextFormat("%02d:%02d", (int)currentTime / 60, (int)currentTime % 60), GetScreenWidth() / 2 - 40, 25, 30, WHITE, 0.8f);
        DrawGlowText(TextFormat("DEATHS: %d", deathCount), 40, 30, 30, WHITE, 0.8f);

        if (player.isDead) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
            DrawGlowText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 60) / 2, GetScreenHeight() / 2 - 30, 60, RED, 1.0f);
        }
        if (isPaused) PauseMenu::Draw();
    }

    void UnloadLevel() {
        levelEvents.clear(); activeObstacles.clear(); levelLoaded = false;
    }
    bool IsLevelFinished() { return levelFinished; }
    int GetScore() { return deathCount; }
    bool ShouldExitToMenu() { return isPaused && PauseMenu::ShouldExitToMenu(); }
}