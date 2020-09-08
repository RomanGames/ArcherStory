#include "input.hpp"

void input::Update() {
    // �L�[�{�[�h //
    DxLib::GetHitKeyStateAll(this->Key);
    // �}�E�X //
    int X = 0, Y = 0;
    DxLib::GetMousePoint(&X, &Y);
    this->MousePos.SetPos((double)X, (double)Y);
    this->MouseInput = DxLib::GetMouseInput();
}

bool input::GetKey(unsigned char KeyCode) {
    return this->Key[KeyCode] == 1;
}

pos input::GetMousePos() {
    return this->MousePos;
}

bool input::GetMouseInput(short MouseCode) {
    return this->MouseInput & MouseCode;
}

input::input() {
    this->Update();
}
