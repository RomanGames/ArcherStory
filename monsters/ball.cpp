#include "ball.hpp"

void ball::Update(map Map) {
    if (this->Use) {

        switch (this->Type) {
        case this->JUMP:
            // Shadow�ړ�
            this->Shadow.Move();
            // Ball�ړ�
            this->Sprite.Pos.SetY(
                this->Shadow.Pos.GetY() - (this->Jump_a * pow(this->Jump_CenterX - this->Shadow.Pos.GetX(), 2) + this->Config["Jump"]["High"].get<double>())
            );
            // ���a���X�s�[�h/2�ȓ��Ȃ����
            if (Distance2d(this->Shadow.Pos, this->PlayerCenterPos) <= Config["Jump"]["Speed"].get<double>() / 2) this->Use = false;
            break;
        }
        // �ړ�
        this->Sprite.Move();
        // �}�b�v�Ƃ̓����蔻��
        switch (this->Type) {
        case this->JUMP:
            std::vector<bool> Col = Map.Collision(&this->Sprite, {
                false,  // air
                false,  // wall
                false,  // stone
                false,  // pond
            });
            for (bool b : Col) if (b == true) this->Use = false;
            break;
        }
        // �_���[�W
        if (this->Type == this->TEST || this->Type == this->JUMP) {
            if (this->Player->CheckHit(this->Sprite, player().CIRCLE)) {
                this->Player->Damage(this->Attack);
                this->Use = false;
            }
        }
        // �O�ɏo�Ă������
        if (!Map.GetInMap(this->Sprite)) this->Use = false;

    }
}

void ball::Draw(int Scroll) {
    if (this->Use) {

        switch (this->Type) {
        case this->TEST:
            DxLib::DrawCircle(
                -Scroll + this->Sprite.GetCenterPos().GetXInt(), 96 + this->Sprite.GetCenterPos().GetYInt(),
                4, 0xff0000, TRUE
            );
            break;
        case this->JUMP:
            DxLib::DrawCircle(
                -Scroll + this->Shadow.GetCenterPos().GetXInt(), 96 + this->Shadow.GetCenterPos().GetYInt(),
                4, 0x000000, TRUE
            );
            DxLib::DrawCircle(
                -Scroll + this->Sprite.GetCenterPos().GetXInt(), 96 + this->Sprite.GetCenterPos().GetYInt(),
                4, 0xff0000, TRUE
            );
            break;
        }

    }
}

ball::ball() {
    this->Use = false;
}
// TEST, JUMP
ball::ball(enum type Type, int Attack, player *Player, pos Pos, pos PlayerCenterPos, json Config) {

    this->Use = true;
    this->Type = Type;
    this->Attack = Attack;
    this->Player = Player;
    this->Sprite.Pos = Pos;
    this->PlayerCenterPos = PlayerCenterPos;
    this->Config = Config;
    switch (Type) {
    case TEST:
        this->Sprite.SetDrectionFromPos(pos(
            this->PlayerCenterPos.GetX() - this->Sprite.Pos.GetX(),
            this->PlayerCenterPos.GetY() - this->Sprite.Pos.GetY()
        ));
        this->Sprite.Motion = this->Sprite.GetPosFromDirection(4.0);
        break;
    case JUMP:
        this->Jump_a = -4 * this->Config["Jump"]["High"].get<double>() / std::pow(this->PlayerCenterPos.GetX() - this->Sprite.Pos.GetX(), 2);
        this->Shadow.Pos = this->Sprite.Pos;
        this->Shadow.SetDrectionFromPos(pos(
            this->PlayerCenterPos.GetX() - this->Sprite.Pos.GetX(),
            this->PlayerCenterPos.GetY() - this->Sprite.Pos.GetY()
        ));
        this->Shadow.Motion = this->Shadow.GetPosFromDirection(Config["Jump"]["Speed"].get<double>());
        this->Sprite.Motion.SetX(Shadow.Motion.GetX());
        this->Jump_CenterX = (this->Sprite.Pos.GetX() + this->PlayerCenterPos.GetX()) / 2;
        break;
    }

}
