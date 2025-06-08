#include "GameApp.h"
#include "Assets.h"
#include "Globals.h"
#include "Menu.h"
#include "Settings.h"
#include "Leaderboard.h"
#include "Credits.h"
#include "LevelSelect.h"
#include "Playing.h"
#include "VictoryScreen.h"
#include <cmath>
#include <fstream>

GameApp::GameApp()
    : currentState(MENU),
    currentTitleFrame(0), titleFrameCounter(0),
    currentBgFrame(0), bgFrameCounter(0),
    shouldExit(false) {
    Init();
}

GameApp::~GameApp() {
    LevelSelect::Unload();
    Assets::SaveConfig();
    Assets::UnloadAll();
    CloseAudioDevice();
}

void GameApp::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Neon Rhythm Jump");
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    Assets::LoadAll();
    if (Assets::isFullscreen) {
        ToggleFullscreen();
    }
    PlayMusicStream(Assets::bgm); 
    SetMusicVolume(Assets::bgm, Assets::volume);
    Menu::Init();
    LevelSelect::Init();
    leaderboard = {
        {"PlayerA", 600 }, { "PlayerB", 815 }, { "PlayerC", 890 },
        {"PlayerD", 930}, {"PlayerE", 980}
    };
}

void GameApp::Run() {
    SetTargetFPS(60);
    while (!WindowShouldClose() && !shouldExit) {
        Update();
        BeginDrawing();
        ClearBackground(BLACK);
        Draw();
        EndDrawing();
    }
}

void GameApp::Update() {
    if (currentState == PLAYING) {
        UpdateMusicStream(Assets::levelMusic);
    }
    else {
        UpdateMusicStream(Assets::bgm);
    }

    if (currentState != PLAYING && currentState != VICTORY) {
        if (++bgFrameCounter >= 4) {
            currentBgFrame = (currentBgFrame + 1) % Assets::bgFrames.size();
            bgFrameCounter = 0;
        }
    }

    if (currentState == MENU) {
        if (++titleFrameCounter >= 3) {
            currentTitleFrame = (currentTitleFrame + 1) % Assets::titleFrames.size();
            titleFrameCounter = 0;
        }
    }

    switch (currentState) {
    case MENU: {
        Menu::Update();
        int clickedButton = Menu::GetClickedButton();
        if (clickedButton != -1) {
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
            switch (clickedButton) {
            case 0: currentState = LEVEL_SELECT; break;
            case 1: currentState = CREDITS; break;
            case 2: currentState = STATS; break;
            case 3: currentState = SETTINGS; break;
            case 4: shouldExit = true; break;
            }
        }
        break;
    }

    case LEVEL_SELECT: {
        LevelSelect::Update();
        if (LevelSelect::BackButtonPressed() || IsKeyPressed(KEY_ESCAPE)) {
            currentState = MENU;
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
        }
        int selectedLevel = LevelSelect::GetSelectedLevel();
        if (selectedLevel != -1) {
            if (selectedLevel == 0) {
                Playing::LoadLevel("Assets/levels/luminescence.json");
                Playing::Init();

                // 切换到游戏音乐
                StopMusicStream(Assets::bgm);
                PlayMusicStream(Assets::levelMusic);
                SetMusicVolume(Assets::levelMusic, Assets::volume);

                currentState = PLAYING;
                SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
                PlaySound(Assets::clickSound);
            }
        }
        break;
    }

    case SETTINGS: {
        SettingsUI::Update();
        if (SettingsUI::BackButtonPressed() || IsKeyPressed(KEY_ESCAPE)) {
            currentState = MENU;
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
        }
        break;
    }
    case STATS: {
        Leaderboard::Update();
        if (Leaderboard::BackButtonPressed() || IsKeyPressed(KEY_ESCAPE)) {
            currentState = MENU;
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
        }
        break;
    }
    case CREDITS: {
        Credits::Update();
        if (Credits::BackButtonPressed() || IsKeyPressed(KEY_ESCAPE)) {
            currentState = MENU;
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
        }
        break;
    }

    case PLAYING: {
        Playing::Update();

        bool shouldSwitchToMenu = Playing::ShouldExitToMenu();
        bool levelIsFinished = Playing::IsLevelFinished();

        if (levelIsFinished || shouldSwitchToMenu) {
         
            StopMusicStream(Assets::levelMusic);
            PlayMusicStream(Assets::bgm);
            SetMusicVolume(Assets::bgm, Assets::volume);

            if (levelIsFinished) {
                VictoryScreen::Init(Playing::GetScore());
                currentState = VICTORY;
            }
            if (shouldSwitchToMenu) {
                currentState = MENU;
                Playing::UnloadLevel();
            }
        }
        break;
    }

    case VICTORY: {
        VictoryScreen::Update();
        VictoryScreen::Option choice = VictoryScreen::GetClickedOption();

        if (choice == VictoryScreen::BACK_TO_MENU) {
            Playing::UnloadLevel();
            currentState = MENU;
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
        }
        else if (choice == VictoryScreen::RESTART_LEVEL) {
         
            StopMusicStream(Assets::bgm);
            PlayMusicStream(Assets::levelMusic);
            SetMusicVolume(Assets::levelMusic, Assets::volume);

            Playing::Init();
            currentState = PLAYING;
            SetSoundVolume(Assets::clickSound, Assets::sfxVolume);
            PlaySound(Assets::clickSound);
        }
        break;
    }
    }
}

void GameApp::Draw() {
    if (currentState != PLAYING && currentState != VICTORY) {
        DrawBackground();
        if (currentState == MENU) {
            DrawPulseEffect();
            DrawTitle();
        }
        else {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
        }
    }

    switch (currentState) {
    case MENU:
        Menu::Draw();
        break;
    case LEVEL_SELECT:
        LevelSelect::Draw();
        break;
    case SETTINGS:
        SettingsUI::Draw();
        break;
    case STATS:
        Leaderboard::Draw(leaderboard);
        break;
    case CREDITS:
        Credits::Draw();
        break;
    case PLAYING:
        Playing::Draw();
        break;
    case VICTORY:
        Playing::Draw();
        VictoryScreen::Draw();
        break;
    }
}

void GameApp::DrawBackground() {
    if (!Assets::bgFrames.empty()) {
        Texture2D& bg = Assets::bgFrames[currentBgFrame];
        Rectangle src = { 0, 0, (float)bg.width, (float)bg.height };
        Rectangle dst = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(bg, src, dst, { 0, 0 }, 0, WHITE);
    }
}

void GameApp::DrawPulseEffect() {
    float globalScale = fminf((float)GetScreenWidth() / 1200.0f, (float)GetScreenHeight() / 800.0f);
    float cx = GetScreenWidth() * 0.3f;
    float cy = GetScreenHeight() * 0.5f;
    float radius = 230 * globalScale;
    float amp = 70 * globalScale;
    float t = GetTime() * 10.0f;
    for (int i = 0; i < 64; ++i) {
        float angle = i * 2 * PI / 64;
        float pulse = sinf(t + i * 0.4f) * 0.5f + 0.5f;
        float len = radius + pulse * amp;
        Vector2 start = { cx + cosf(angle) * radius, cy + sinf(angle) * radius };
        Vector2 end = { cx + cosf(angle) * len, cy + sinf(angle) * len };
        DrawLineEx(start, end, 2.0f * globalScale, WHITE);
    }
    DrawCircle(cx, cy, radius - 10 * globalScale, Fade(BLACK, 0.6f));
}

void GameApp::DrawTitle() {
    if (!Assets::titleFrames.empty()) {
        float globalScale = fminf((float)GetScreenWidth() / 1200.0f, (float)GetScreenHeight() / 800.0f);
        float baseRadius = 230 * globalScale;
        Texture2D& title = Assets::titleFrames[currentTitleFrame];
        float scale = (baseRadius * 1.8f) / (float)title.width;
        float x = GetScreenWidth() * 0.3f - (title.width * scale / 2.0f);
        float y = GetScreenHeight() * 0.5f - (title.height * scale / 2.0f);
        DrawTextureEx(title, { x, y }, 0, scale, WHITE);
    }
}