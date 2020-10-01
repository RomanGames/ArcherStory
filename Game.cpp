#include "Game.hpp"

Game::Game(bool Debug, std::string ConfigFilePath) {

    this->Debug = Debug;
    this->ConfigFilePath = ConfigFilePath;
    this->Frame = 0;

    SetOutApplicationLogValidFlag(this->Debug);

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
    this->Config = json::parse(JsonString, nullptr, false, true);

    // �Ȃ����ڂ͕⑫ ������͂���Ε����R�[�h�ϊ�
    if (this->Config["WindowName"].empty())         this->Config["WindowName"] = "�A�[�`���[����";
    else this->Config["WindowName"] = utf8_to_sjis(this->Config["WindowName"].get<std::string>());
    if (this->Config["WindowExtendRate"].empty())   this->Config["WindowExtendRate"] = 1.0;
    if (this->Config["Player"]["JoystickSize"].empty())         this->Config["Player"]["JoystickSize"] = 64;
    if (this->Config["Player"]["Speed"].empty())                this->Config["Player"]["Speed"] = 7.0;
    if (this->Config["Player"]["DefaultMaxHP"].empty())         this->Config["Player"]["DefaultMaxHP"] = 500;
    if (this->Config["Player"]["GodTimeMax"].empty())           this->Config["Player"]["GodTimeMax"] = 15;
    if (this->Config["Player"]["AttackCooldownMax"].empty())    this->Config["Player"]["AttackCooldownMax"] = 36;
    if (this->Config["Player"]["DefaultAttack"].empty())        this->Config["Player"]["DefaultAttack"] = 40;
    if (this->Config["Player"]["Arrow"]["Speed"].empty())       this->Config["Player"]["Arrow"]["Speed"] = 8.0;
    if (this->Config["Monsters"]["FlowerPlant"]["AttackSpeed"].empty()) this->Config["Monsters"]["FlowerPlant"]["AttackSpeed"] = 120;
    if (this->Config["Monsters"]["Slime"]["MoveCooldown"].empty())      this->Config["Monsters"]["Slime"]["MoveCooldown"] = 60;
    if (this->Config["Balls"]["Jump"]["High"].empty())      this->Config["Balls"]["Jump"]["High"] = 64.0;
    if (this->Config["Balls"]["Jump"]["Speed"].empty())     this->Config["Balls"]["Jump"]["Speed"] = 8.0;

    // �����ݒ�
    SetGraphMode(1280, 720, 16);
    SetDrawScreen(DX_SCREEN_BACK);
    ChangeWindowMode(TRUE);
    SetMainWindowText(this->Config["WindowName"].get<std::string>().c_str());
    //SetWindowIconID(IDI_ICON1);
    SetWindowSizeChangeEnableFlag(TRUE);
    SetWindowSizeExtendRate(this->Config["WindowExtendRate"].get<double>());
    SetUseTransColor(FALSE);
    if (DxLib_Init() == -1) return -1;

    return 0;

}

bool Game::Update() {
    if (this->Input.GetKey(KEY_INPUT_ESCAPE)) return false;
    bool Continue = false;
    switch (this->Scene) {
    case this->INTRO:
        Continue = this->Intro();
        break;
    case this->STAGE:
        Continue = this->Stage();
        break;
    case this->PAUSE:
        Continue = this->Pause();
        break;
    case this->SKILL_SELECT:
        Continue = this->SkillSelect();
        break;
    case this->DIE:
        Continue = this->Die();
        break;
    }
    return Continue;
}

void Game::Unload(bool Error) {

    //for (monster *m: this->Monster) free(m);
    DxLib::DxLib_End();

}

bool Game::Intro() {

    if (BeforeIntroFrame + 1 != Frame) {

        // �摜�ǂݍ���

        std::map<std::string, std::map<std::string, int>> Graph;
        Graph["map"]["ground"] = DxLib::LoadGraph("data/stable/img/map/ground.png");
        Graph["map"]["wall"] = DxLib::LoadGraph("data/stable/img/map/wall.png");
        Graph["map"]["stone"] = DxLib::LoadGraph("data/stable/img/map/stone.png");
        int PondGraph[3 * 3];
        DxLib::LoadDivGraph("data/stable/img/map/pond.png", 3 * 3, 3, 3, 48 / 2, 48 / 2, PondGraph);
        Graph["map"]["pond00"] = PondGraph[3 * 0 + 0];
        Graph["map"]["pond01"] = PondGraph[3 * 0 + 1];
        Graph["map"]["pond02"] = PondGraph[3 * 0 + 2];
        Graph["map"]["pond10"] = PondGraph[3 * 1 + 0];
        Graph["map"]["pond11"] = PondGraph[3 * 1 + 1];
        Graph["map"]["pond12"] = PondGraph[3 * 1 + 2];
        Graph["map"]["pond20"] = PondGraph[3 * 2 + 0];
        Graph["map"]["pond21"] = PondGraph[3 * 2 + 1];
        Graph["map"]["pond22"] = PondGraph[3 * 2 + 2];
        Graph["map"]["shutter"] = DxLib::LoadGraph("data/stable/img/map/shutter.png");
        Graph["map"]["shutter_gate"] = DxLib::LoadGraph("data/stable/img/map/shutter_gate.png");
        Graph["player"]["ring"] = DxLib::LoadGraph("data/stable/img/player/ring.png");
        Graph["player"]["right"] = DxLib::LoadGraph("data/stable/img/player/right.png");
        Graph["player"]["joystick_base"] = DxLib::LoadGraph("data/stable/img/player/joystick_base.png");
        Graph["player"]["joystick_stick"] = DxLib::LoadGraph("data/stable/img/player/joystick_stick.png");
        Graph["player"]["arrow_arrow"] = DxLib::LoadGraph("data/stable/img/player/arrow.png");
        Graph["monsters"]["ring"] = DxLib::LoadGraph("data/stable/img/monsters/ring.png");
        Graph["monsters"]["flower_plant_normal"] = DxLib::LoadGraph("data/stable/img/monsters/flower_plant_normal.png");
        Graph["monsters"]["flower_plant_attack"] = DxLib::LoadGraph("data/stable/img/monsters/flower_plant_attack.png");
        Graph["monsters"]["slime"] = DxLib::LoadGraph("data/stable/img/monsters/slime.png");
        Graph["monsters"]["slime_lvlup"] = DxLib::LoadGraph("data/stable/img/monsters/slime_lvlup.png");
        Graph["monsters"]["ball_jumping"] = DxLib::LoadGraph("data/stable/img/monsters/ball/jumping.png");
        Graph["monsters"]["ball_shadow"] = DxLib::LoadGraph("data/stable/img/monsters/ball/shadow.png");

        // �}�b�v�ǂݍ���
        json Maps;
        std::string JsonString = this->FileToString("data/stable/map.json");
        if (JsonString == "") JsonString = "{}";
        Maps = json::parse(JsonString, nullptr, false, true);

        // ������

        this->Input = input(false);
        this->Map = map(Maps, Graph, &this->FlowerPlant, &this->Slime, &this->Ball, &this->Player, this->Config);
        this->Player = player(&this->Input, &this->Map, &this->Arrow, &this->Death, &this->Monster, Graph["player"], this->Config["Player"]);

        for (int i = 0; i < 4; i++) {
            this->FlowerPlant.push_back(flower_plant(&this->Ball, pos(48.0 + DxLib::GetRand(1072), 48.0 + DxLib::GetRand(496)), 100, 100, &this->Map, &this->Player, Graph["monsters"], this->Config));
        }
        for (int i = 0; i < 4; i++) {
            this->Slime.push_back(slime(pos(48.0 + DxLib::GetRand(1072), 48.0 + DxLib::GetRand(496)), 100, 2, &this->Map, &this->Player, Graph["monsters"], this->Config));
        }

    }

    // ���� //
    if (this->Input.GetKey(KEY_INPUT_SPACE)) this->Scene = this->STAGE;

    // �`��
    DxLib::ClearDrawScreen();
    DxLib::DrawBox(64, 64, 256, 256, 0x00ffff, TRUE);
    DxLib::ScreenFlip();

    this->BeforeIntroFrame = this->Frame;

    return true;

}

bool Game::Stage() {

    // ���� //
    // Monster�X�V
    this->Monster.resize(0);
    for (int i = 0; i < this->FlowerPlant.size(); i++) this->Monster.push_back(this->FlowerPlant[i].Monster);
    for (int i = 0; i < this->Slime.size(); i++) this->Monster.push_back(this->Slime[i].Monster);
    // Update
    this->Player.Update();
    for (int j = 0; j < 4; j++) {   // �X�s�[�h���グ�邽�ߓ�d
        for (int i = 0; i < this->Arrow.size(); i++) this->Arrow[i].Update();
    }
    for (int i = 0; i < this->FlowerPlant.size(); i++) this->FlowerPlant[i].Update();
    std::vector<slime> SlimeToAdd;
    for (int i = 0; i < this->Slime.size(); i++) {
        SlimeToAdd = this->Slime[i].Update();
        //for (int j = 0; j < SlimeToAdd.size(); j++) {
        //    this->Slime.push_back(SlimeToAdd[j]);
        //}
        this->Slime.insert(this->Slime.end(), SlimeToAdd.begin(), SlimeToAdd.end());
    }
    for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Update();
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
        this->Scene = this->DIE;
    }
    // �S���|�����I
    bool IsClear = true;
    for (monster *m : this->Monster) if (m->Use) IsClear = false;
    if (IsClear) this->Map.Clear();

    // �`�� //
    DxLib::ClearDrawScreen();
    int Scroll = this->Player.Sprite.Pos.GetX() - this->Player.StartPos.GetX();
    // �w�i
    this->Map.Draw(Scroll);
    // ��
    for (arrow a : this->Arrow) a.Draw(Scroll);
    // �G
    for (flower_plant f : this->FlowerPlant) f.Draw(Scroll);
    for (slime s : this->Slime) s.Draw(Scroll);
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
