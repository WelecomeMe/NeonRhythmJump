// VictoryScreen.h
#pragma once

namespace VictoryScreen {
    // ���������ʤ�������Ͽ���������ѡ��
    enum Option {
        NONE,
        RESTART_LEVEL, // ���¿�ʼ����
        BACK_TO_MENU
    };

    void Init(int deathCount); // ֻ��Ҫ������������
    void Update();
    void Draw();
    Option GetClickedOption();
}