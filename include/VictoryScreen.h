// VictoryScreen.h
#pragma once

namespace VictoryScreen {
    // 定义玩家在胜利界面上可能做出的选择
    enum Option {
        NONE,
        RESTART_LEVEL, // 重新开始本关
        BACK_TO_MENU
    };

    void Init(int deathCount); // 只需要传入死亡次数
    void Update();
    void Draw();
    Option GetClickedOption();
}