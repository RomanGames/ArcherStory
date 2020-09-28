#include "Game.hpp"

Game::Game(bool Debug, std::string ConfigFilePath) {

    this->Debug = Debug;
    this->ConfigFilePath = ConfigFilePath;
    this->Frame = 0;

    bool Error = false;

    if (this->Load() == -1) return;
    while (this->Update()) {
        this->Input.Update();
        if (DxLib::ProcessMessage()) {
            Error = true;
            break;
        }
        this->Frame++;
    }
    this->Unload(Error);

}

int Game::Load() {

    // json�ǂݍ���
    std::string JsonString = this->FileToString(this->ConfigFilePath);
    if (JsonString == "") JsonString = "{}";
    this->Config = json::parse(JsonString);

    // �Ȃ����ڂ͕⑫ ������͂���Ε����R�[�h�ϊ�
    if (this->Config["WindowName"].empty())         this->Config["WindowName"] = "�A�[�`���[����";
    else this->Config["WindowName"] = utf8_to_sjis(this->Config["WindowName"].get<std::string>());
    if (this->Config["WindowExtendRate"].empty())   this->Config["WindowExtendRate"] = 1.0;
    if (this->Config["Player"]["JoystickSize"].empty())         this->Config["Player"]["JoystickSize"] = 64;
    if (this->Config["Player"]["Speed"].empty())                this->Config["Player"]["Speed"] = 7.0;
    if (this->Config["Player"]["DefaultMaxHP"].empty())         this->Config["Player"]["DefaultMaxHP"] = 500;
    if (this->Config["Player"]["GodTimeMax"].empty())           this->Config["Player"]["GodTimeMax"] = 60;
    if (this->Config["Player"]["AttackCooldownMax"].empty())    this->Config["Player"]["AttackCooldownMax"] = 40;
    if (this->Config["Player"]["DefaultAttack"].empty())        this->Config["Player"]["DefaultAttack"] = 40;
    if (this->Config["Player"]["Arrow"]["Speed"].empty())       this->Config["Player"]["Arrow"]["Speed"] = 8.0;
    if (this->Config["Monsters"]["FlowerPlant"]["AttackSpeed"].empty())  this->Config["Monsters"]["FlowerPlant"]["AttackSpeed"] = 120;
    if (this->Config["Balls"]["Jump"]["High"].empty())      this->Config["Balls"]["Jump"]["High"] = 64.0;
    if (this->Config["Balls"]["Jump"]["Speed"].empty())     this->Config["Balls"]["Jump"]["Speed"] = 8.0;

    // �����ݒ�
    SetOutApplicationLogValidFlag(this->Debug);
    SetGraphMode(1280, 720, 16);
    SetDrawScreen(DX_SCREEN_BACK);
    ChangeWindowMode(TRUE);
    SetMainWindowText(this->Config["WindowName"].get<std::string>().c_str());
    //SetWindowIconID(IDI_ICON1);
    SetWindowSizeChangeEnableFlag(TRUE);
    SetWindowSizeExtendRate(this->Config["WindowExtendRate"].get<double>());
    if (DxLib_Init() == -1) return -1;

    this->Input = input();
    this->Map = map();
    this->Player = player(&this->Input, &this->Map, &this->Arrow, &this->Death, &this->Monster, this->Config["Player"]);

    for (int i = 0; i < 4; i++) {
        this->FlowerPlant.push_back(flower_plant(&this->Ball, pos(48.0 + DxLib::GetRand(1072), 48.0 + DxLib::GetRand(496)), 100, 100, &this->Player, this->Config));
    }

    return 0;

}

bool Game::Update() {
    if (this->Input.GetKey(KEY_INPUT_ESCAPE)) return false;
    switch (this->Seen) {
    case this->INTRO:
        return this->Intro();
    case this->STAGE:
        return this->Stage();
    case this->PAUSE:
        return this->Pause();
    case this->SKILL_SELECT:
        return this->SkillSelect();
    case this->DIE:
        return this->Die();
    }
}

void Game::Unload(bool Error) {

    //for (monster *m: this->Monster) free(m);
    DxLib::DxLib_End();

}

bool Game::Intro() {

    // ���� //
    if (this->Input.GetKey(KEY_INPUT_SPACE)) this->Seen = this->STAGE;

    // �`��
    DxLib::ClearDrawScreen();
    DxLib::DrawBox(64, 64, 256, 256, 0x00ffff, TRUE);
    DxLib::ScreenFlip();

    return true;

}

bool Game::Stage() {

    // ���� //
    // Monster�X�V
    //for (monster *m : this->Monster) free(m);
    this->Monster.resize(0);
    for (int i = 0; i < this->FlowerPlant.size(); i++) this->Monster.push_back(&this->FlowerPlant[i].Monster);
    // Update
    this->Player.Update();
    for (int j = 0; j < 4; j++) {   // �X�s�[�h���グ�邽�ߓ�d
        for (int i = 0; i < this->Arrow.size(); i++) this->Arrow[i].Update(this->Map);
    }
    for (int i = 0; i < this->FlowerPlant.size(); i++) this->FlowerPlant[i].Update(this->Map);
    for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Update(this->Map);
    // �g���ĂȂ����̂��폜(1�b����)
    if (this->Frame % 60 == 0) {
        int Size;
        // �{�[��
        Size = this->Ball.size();
        for (int i = 0; i < Size; i++) {
            if (!this->Ball[i].Use) {
                // �I��肾�������������/�r���Ȃ�Ō�������čŌ������
                if (i == Size - 1) {
                    Ball.pop_back();
                    break;
                }
                Ball[i] = Ball.back();
                Ball.pop_back();
                Size--;
                i--;
            }
        }
        // ��
        Size = this->Arrow.size();
        for (int i = 0; i < Size; i++) {
            if (!this->Arrow[i].Use) {
                // �I��肾�������������/�r���Ȃ�Ō�������čŌ������
                if (i == Size - 1) {
                    Arrow.pop_back();
                    break;
                }
                Arrow[i] = Arrow.back();
                Arrow.pop_back();
                Size--;
                i--;
            }
        }
    }
    // ���񂶂�����I
    if (this->Death) {
        this->Death = false;
        this->Seen = this->DIE;
    }

    // �`�� //
    DxLib::ClearDrawScreen();
    int Scroll = this->Player.Sprite.Pos.GetX() - this->Player.StartPos.GetX();
    // �w�i
    DxLib::DrawBox(0, 0, 1280, 720, 0x00FF00, TRUE);
    this->Map.Draw(Scroll);
    // ��
    for (arrow a : this->Arrow) a.Draw(Scroll);
    // �G
    for (flower_plant f : this->FlowerPlant) f.Draw(Scroll);
    // �v���C���[
    this->Player.Draw();
    // �G�̒e
    for (ball b : this->Ball) b.Draw(Scroll);
    // GUI
    this->Player.JoystickDraw();
    DxLib::ScreenFlip();

    return true;

}

bool Game::Pause() {

    // ���� //


    // �`��
    DxLib::ClearDrawScreen();

    DxLib::ScreenFlip();

    return true;

}

bool Game::SkillSelect() {

    // ���� //


    // �`��
    DxLib::ClearDrawScreen();

    DxLib::ScreenFlip();

    return true;

}

bool Game::Die() {

    // ���� //


    // �`��
    DxLib::ClearDrawScreen();

    DxLib::ScreenFlip();

    return true;

}

std::string Game::FileToString(std::string FilePath) {

    std::ifstream File(FilePath);
    if (!File) return "";
    return std::string(
        (std::istreambuf_iterator<char>(File)),
        std::istreambuf_iterator<char>()
    );

}
