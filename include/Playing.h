#pragma once

#include "PauseMenu.h" // 包含暂停菜单模块
#include <string>

namespace Playing {
    // 主要功能
    void LoadLevel(const std::string& levelFilePath);
    void Init();
    void Update();
    void Draw();
    void UnloadLevel();

    // 查询函数
    bool IsLevelFinished();
    int GetScore();
    bool ShouldExitToMenu(); // <--- 新增：用于通知 GameApp 返回主菜单
}