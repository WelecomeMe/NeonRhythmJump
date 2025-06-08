// Leaderboard.h
#pragma once
#include "Globals.h"
#include <vector>

namespace Leaderboard {
    void Update(); // ✅ 添加 Update
    void Draw(const std::vector<ScoreEntry>& scores);
    bool BackButtonPressed(); // ✅ 添加查询函数
}