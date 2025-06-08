// LevelSelect.h
#pragma once

namespace LevelSelect {
    void Init();      // ✅ 添加 Init 来加载关卡信息
    void Update();
    void Draw();
    int GetSelectedLevel();
    bool BackButtonPressed();
    void Unload();    // ✅ 添加 Unload 来释放资源
}