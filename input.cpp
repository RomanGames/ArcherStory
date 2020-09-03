#include "input.hpp"

void input::Update() {
    // �L�[�{�[�h //
    DxLib::GetHitKeyStateAll(this->Key);
    // �}�E�X //
    int *X, *Y;
    DxLib::GetMousePoint(X, Y);
    this->MousePos.SetPos((double)*X, (double)*Y);
    free(X);
    free(Y);
    this->MouseInput = DxLib::GetMouseInput();
}

bool input::GetKey(char KeyCode) {
    return this->Key[KeyCode];
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
