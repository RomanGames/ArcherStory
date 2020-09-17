#pragma once
#include "DxLib.h"
#include <string>
#include "input.hpp"
#include "pos.hpp"
#include "player.hpp"
#include "map.hpp"

class Game {

private:

    bool Debug;
    unsigned int Frame;
    std::string ConfigFilePath;
    input Input{};

    map Map{};
    player Player{&Input, &Map};

    void Load();                // ��ԍŏ�
    bool Update();              // �}�C�t���[�� return false:�I��
    void Unload(bool Error);    // �Ō� Error:�G���[�ŏI��������
    
public:

    Game(bool Debug = false, std::string ConfigFilePath = "config.ini");

};

