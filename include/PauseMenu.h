// PauseMenu.h
#pragma once

namespace PauseMenu {
    void Init();
    void Update();
    void Draw();

    // 查询函数，告诉 Playing 模块玩家选择了什么
    bool ShouldResume();      // 是否要返回游戏
    bool ShouldRestart();     // 是否要重启关卡
    bool ShouldExitToMenu();  // 是否要退出到主菜单
}