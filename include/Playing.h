#pragma once

#include "PauseMenu.h" // ������ͣ�˵�ģ��
#include <string>

namespace Playing {
    // ��Ҫ����
    void LoadLevel(const std::string& levelFilePath);
    void Init();
    void Update();
    void Draw();
    void UnloadLevel();

    // ��ѯ����
    bool IsLevelFinished();
    int GetScore();
    bool ShouldExitToMenu(); // <--- ����������֪ͨ GameApp �������˵�
}