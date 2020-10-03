#include "monster.hpp"

void monster::Update() {

    // ����HP���J���X�g���Ă��璼��
    if (this->HP > this->MaxHP) this->HP = this->MaxHP;
    // ��
    if (PoisonCount > 0) {
        PoisonCount--;
        if (PoisonCount % 60 == 0) this->Damage(20, pos(0, 0));
    }
    // AttackCount���X�V
    if (this->AttackCount > 0) this->AttackCount--;

    if (this->KnockBackCount > 0) {
        // �m�b�N�o�b�N
        this->KnockBackCount--;
        this->Sprite.Pos.AddPos(this->KnockBack.GetX(), this->KnockBack.GetY());
        // �u���b�N�Ƃ̓����蔻��
        pos Motion = this->Sprite.Motion;
        this->Sprite.Motion = this->KnockBack;
        this->Map->Collision(&(this->Sprite), BlockCol);
        this->Sprite.Motion = Motion;
    } else {
        // Motion�̕������ړ�
        this->Sprite.Move();
        // �u���b�N�Ƃ̓����蔻��
        this->Map->Collision(&(this->Sprite), BlockCol);
    }
    // �O�ɂ͏o�����Ȃ�
    if (!this->Map->GetInMap(this->Sprite)) this->Sprite.Pos = this->SpawnPoint;
    // �ڐG�_���[�W
    if (!this->Hide && this->Player->CheckHit(this->Sprite, player::CIRCLE)) this->Player->Damage(this->Player->GetMaxHP() / 64);

}

void monster::DrawRing(int Scroll) {
    if (!this->Hide) {

        DxLib::DrawExtendGraph(
            -Scroll + this->Sprite.GetCenterPos().GetXInt() - min(this->Sprite.Size.GetXInt(), this->Sprite.Size.GetYInt()) / 2,
            96 + this->Sprite.GetCenterPos().GetYInt() - min(this->Sprite.Size.GetXInt(), this->Sprite.Size.GetYInt()) / 2,
            -Scroll + this->Sprite.GetCenterPos().GetXInt() + min(this->Sprite.Size.GetXInt(), this->Sprite.Size.GetYInt()) / 2,
            96 + this->Sprite.GetCenterPos().GetYInt() + min(this->Sprite.Size.GetXInt(), this->Sprite.Size.GetYInt()) / 2,
            this->Graph["ring"], TRUE
        );

    }
}

void monster::DrawHP(int Scroll) {
    if (!this->Hide) {

        DxLib::DrawBox(
            -Scroll + this->Sprite.GetCenterPos().GetXInt() - 32, 96 + this->Sprite.Pos.GetYInt() - 16,
            -Scroll + this->Sprite.GetCenterPos().GetXInt() + 32, 96 + this->Sprite.Pos.GetYInt() - 8,
            0x000000, TRUE
        );
        DxLib::DrawBox(
            -Scroll + this->Sprite.GetCenterPos().GetXInt() - 32, 96 + this->Sprite.Pos.GetYInt() - 16,
            -Scroll + this->Sprite.GetCenterPos().GetXInt() - 32 + 1 + (64 - 1) * this->HP / this->MaxHP, 96 + this->Sprite.Pos.GetYInt() - 8,
            0xff0000, TRUE
        );
        DxLib::DrawBox(
            -Scroll + this->Sprite.GetCenterPos().GetXInt() - 32, 96 + this->Sprite.Pos.GetYInt() - 16,
            -Scroll + this->Sprite.GetCenterPos().GetXInt() + 32, 96 + this->Sprite.Pos.GetYInt() - 8,
            0x000000, FALSE
        );
        DxLib::DrawFormatString(
            -Scroll + this->Sprite.GetCenterPos().GetXInt() - 32, 96 + this->Sprite.Pos.GetYInt() - 32,
            0x000000, "%d", this->HP
        );

    }
}

bool monster::GetAttack(int ResetAttackCount) {
    if (this->AttackCount == 0) {
        this->AttackCount = ResetAttackCount;
        if (this->AttackCount == 0) this->AttackCount = this->MaxAttackCount;
        return true;
    }
    else return false;
}

bool monster::GetDeath() {
    bool Death = (this->BeforeHP > 0 && HP <= 0);
    this->BeforeHP = this->HP;
    return Death;
}

int monster::GetHP() {
    return this->HP;
}

void monster::Heal(int AddHP) {
    this->HP += AddHP;
    if (this->HP > this->MaxHP) this->HP = this->MaxHP;
}

void monster::Damage(int Damage, pos Motion, bool Poison) {
    this->HP -= Damage;
    this->AttackCount += 10;
    this->KnockBack = pos(Motion.GetX() / 4, Motion.GetY() / 4);
    this->KnockBackCount = 4;
    if (Poison) this->PoisonCount = 480;
    if (this->HP <= 0) {
        // ���񂶂�����I
        this->HP = 0;
        this->Use = false;
    }
}

bool monster::CheckHit(sprite Sprite, enum shape Shape) {

    switch (Shape) {
    case this->SQUARE:
        if (this->Sprite.GetSidePos(sprite::LEFT) >= Sprite.GetSidePos(sprite::RIGHT)) return false;
        if (this->Sprite.GetSidePos(sprite::RIGHT) <= Sprite.GetSidePos(sprite::LEFT)) return false;
        if (this->Sprite.GetSidePos(sprite::UP) >= Sprite.GetSidePos(sprite::DOWN)) return false;
        if (this->Sprite.GetSidePos(sprite::DOWN) <= Sprite.GetSidePos(sprite::UP)) return false;
        return true;
    case this->CIRCLE:
        if (Distance2d(
            this->Sprite.GetCenterPos(), Sprite.GetCenterPos()
        ) < (
            min(this->Sprite.Size.GetX(), this->Sprite.Size.GetY()) + min(Sprite.Size.GetX(), Sprite.Size.GetY())
            )) return true;
        return false;
    }

}

monster::monster() {
    this->Use = false;
}
monster::monster(pos Pos, int HP, int StartAttackCount, map *Map, player *Player, std::map<std::string, int> Graph, pos Size) {

    this->Use = true;

    this->Sprite.Pos = Pos;
    this->SpawnPoint = this->Sprite.Pos;
    this->MaxHP = HP;
    this->HP = HP;
    this->AttackCount = StartAttackCount;
    this->MaxAttackCount = StartAttackCount;
    this->Map = Map;
    this->Player = Player;
    this->Graph = Graph;
    this->Sprite.Size = Size;
    
    // �摜�T�C�Y�擾
    int X, Y;

    DxLib::GetGraphSize(this->Graph["ring"], &X, &Y);
    this->GraphSize["ring"].SetPos(X, Y);

}
