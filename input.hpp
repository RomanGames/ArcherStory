#pragma once
#include "DxLib.h"
#include <vector>
#include "pos.hpp"

class input {

private:

    // �L�[�{�[�h //
    char Key[256];

    // �}�E�X //
    pos MousePos{};
    int MouseInput;
    int BeforeMouseInput;
    // int MouseScroll

    // �^�b�`�p�l��//


    // �W���C�p�b�h //


public:

    void Update();

    bool GetKey(unsigned char KeyCode);
    pos GetMousePos();
    bool GetMouseInput(short MouseCode);
    bool GetMouseDown(short MouseCode);
    bool GetMouseUp(short MouseCode);

    input(bool Init = true);

};

