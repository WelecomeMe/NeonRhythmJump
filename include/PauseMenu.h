// PauseMenu.h
#pragma once

namespace PauseMenu {
    void Init();
    void Update();
    void Draw();

    // ��ѯ���������� Playing ģ�����ѡ����ʲô
    bool ShouldResume();      // �Ƿ�Ҫ������Ϸ
    bool ShouldRestart();     // �Ƿ�Ҫ�����ؿ�
    bool ShouldExitToMenu();  // �Ƿ�Ҫ�˳������˵�
}