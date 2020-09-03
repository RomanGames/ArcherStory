#pragma once
#include "DxLib.h"
#include <string>
#include "input.hpp"
#include "pos.hpp"

class Game {

private:

    bool Debug;
    unsigned int Frame;
    std::string ConfigFilePath;

    void Load();                // ��ԍŏ�
    bool Update();              // �}�C�t���[�� return false:�I��
    void Unload(bool Error);    // �Ō� Error:�G���[�ŏI��������
    
public:

    Game(bool Debug = false, std::string ConfigFilePath = "config.ini");

};

