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
    if (this->Config["FullScreen"].empty())         this->Config["FullScreen"] = false;
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
    if (this->Config["Monsters"]["Golem"]["AttackSpeed"].empty())       this->Config["Monsters"]["Golem"]["AttackSpeed"] = 120;
    if (this->Config["Monsters"]["Bat"]["AttackSpeed"].empty())         this->Config["Monsters"]["Bat"]["AttackSpeed"] = 90;
    if (this->Config["Monsters"]["Tree"]["AttackSpeed"].empty())        this->Config["Monsters"]["Tree"]["AttackSpeed"] = 180;
    if (this->Config["Balls"]["Stone"]["Speed"].empty())    this->Config["Balls"]["Stone"]["Speed"] = 8.0;
    if (this->Config["Balls"]["Jump"]["High"].empty())      this->Config["Balls"]["Jump"]["High"] = 64.0;
    if (this->Config["Balls"]["Jump"]["Speed"].empty())     this->Config["Balls"]["Jump"]["Speed"] = 8.0;
    if (this->Config["Balls"]["Beam"]["Speed"].empty())     this->Config["Balls"]["Beam"]["Speed"] = 8.0;

    // �����ݒ�
    DxLib::SetGraphMode(1280, 720, 16);
    DxLib::SetDrawScreen(DX_SCREEN_BACK);
    DxLib::ChangeWindowMode(!this->Config["FullScreen"].get<bool>());
    DxLib::SetMainWindowText(this->Config["WindowName"].get<std::string>().c_str());
    //DxLib::SetWindowIconID(IDI_ICON1);
    DxLib::SetWindowSizeChangeEnableFlag(TRUE);
    DxLib::SetWindowSizeExtendRate(this->Config["WindowExtendRate"].get<double>());
    DxLib::SetUseTransColor(FALSE);
    if (DxLib_Init() == -1) return -1;
    DxLib::SetMouseDispFlag(TRUE);

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

        this->StartGraph = DxLib::LoadGraph("data/stable/img/start.png");
        DxLib::DrawGraph(0, 0, this->StartGraph, FALSE);
        this->EndGraph = DxLib::LoadGraph("data/stable/img/end.png");

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
        Graph["monsters"]["golem"] = DxLib::LoadGraph("data/stable/img/monsters/golem.png");
        Graph["monsters"]["bat"] = DxLib::LoadGraph("data/stable/img/monsters/bat.png");
        Graph["monsters"]["tree"] = DxLib::LoadGraph("data/stable/img/monsters/tree.png");
        Graph["monsters"]["virus"] = DxLib::LoadGraph("data/stable/img/monsters/virus.png");
        Graph["monsters"]["virus_lvlup"] = DxLib::LoadGraph("data/stable/img/monsters/virus_lvlup.png");
        Graph["monsters"]["ball_stone"] = DxLib::LoadGraph("data/stable/img/monsters/ball/stone.png");
        Graph["monsters"]["ball_jumping"] = DxLib::LoadGraph("data/stable/img/monsters/ball/jumping.png");
        Graph["monsters"]["ball_shadow"] = DxLib::LoadGraph("data/stable/img/monsters/ball/shadow.png");
        Graph["monsters"]["ball_beam"] = DxLib::LoadGraph("data/stable/img/monsters/ball/beam.png");

        // �t�H���g�쐬
        std::map<std::string, int> Font;
        Font["Parameters"] = CreateFontToHandle("ParametersFont", 24, 1, DX_FONTTYPE_NORMAL);
        Font["NewSkill"] = CreateFontToHandle("ParametersFont", 48, 1, DX_FONTTYPE_NORMAL);

        // �}�b�v�ǂݍ���
        json Maps;
        std::string JsonString = this->FileToString("data/stable/map.json");
        if (JsonString == "") JsonString = "{}";
        Maps = json::parse(JsonString, nullptr, false, true);

        // ������

        this->Input = input(false);
        this->Map = map(Maps, Graph, Font, &this->FlowerPlant, &this->Slime, &this->Golem, &this->Bat, &this->Tree, &this->Virus, &this->Ball, &this->Player, this->Config);
        this->Player = player(&this->Input, &this->Map, &this->Arrow, &this->Death, &this->Next, &this->Monster, Graph["player"], Font, this->Config["Player"]);

        // �����p
        //for (int i = 0; i < 8; i++) {
        //    this->Virus.push_back(virus(pos(48.0 + DxLib::GetRand(1072), 48.0 + DxLib::GetRand(496)), 100, 2, &this->Map, &this->Player, Graph["monsters"]));
        //}

    }

    // ���� //
    if (this->Input.GetKey(KEY_INPUT_SPACE) || this->Input.GetMouseDown(MOUSE_INPUT_LEFT)) this->Scene = this->STAGE;

    // �`��
    DxLib::ClearDrawScreen();
    DxLib::DrawGraph(0, 0, this->StartGraph, FALSE);
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
    for (int i = 0; i < this->Golem.size(); i++) this->Monster.push_back(this->Golem[i].Monster);
    for (int i = 0; i < this->Bat.size(); i++) this->Monster.push_back(this->Bat[i].Monster);
    for (int i = 0; i < this->Tree.size(); i++) this->Monster.push_back(this->Tree[i].Monster);
    for (int i = 0; i < this->Virus.size(); i++) this->Monster.push_back(this->Virus[i].Monster);
    // �f�o�b�O�@�\
    if (this->Debug) {
        // kill
        if (this->Input.GetKey(KEY_INPUT_K)) for (int i = 0; i < this->Monster.size(); i++) {
            this->Monster[i]->HP = 0;
            this->Monster[i]->Use = false;
        }
        // speed
        if (this->Input.GetKey(KEY_INPUT_S)) {
            this->Player.Update();
        }
    }
    // Update
    this->Player.Update();
    for (int j = 0; j < 4; j++) {   // �X�s�[�h���グ�邽�ߓ�d
        std::vector<arrow> ArrowToAdd;
        for (int i = 0; i < this->Arrow.size(); i++) {
            ArrowToAdd = this->Arrow[i].Update();
            this->Arrow.insert(this->Arrow.end(), ArrowToAdd.begin(), ArrowToAdd.end());
        }
    }
    for (int i = 0; i < this->FlowerPlant.size(); i++) this->FlowerPlant[i].Update();
    std::vector<slime> SlimeToAdd;
    for (int i = 0; i < this->Slime.size(); i++) {
        SlimeToAdd = this->Slime[i].Update();
        this->Slime.insert(this->Slime.end(), SlimeToAdd.begin(), SlimeToAdd.end());
    }
    for (int i = 0; i < this->Golem.size(); i++) this->Golem[i].Update();
    for (int i = 0; i < this->Bat.size(); i++) this->Bat[i].Update();
    for (int i = 0; i < this->Tree.size(); i++) this->Tree[i].Update();
    std::vector<virus> VirusToAdd;
    for (int i = 0; i < this->Virus.size(); i++) {
        VirusToAdd = this->Virus[i].Update();
        this->Virus.insert(this->Virus.end(), VirusToAdd.begin(), VirusToAdd.end());
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
        this->FadeOutCount = 0;
    }
    // �S���|�����I
    this->ClearCount++;
    for (monster *m : this->Monster) if (m->Use) this->ClearCount = 0;
    if (this->ClearCount == 0) this->Map.ClearCancel();
    if (this->ClearCount >= 2) {
        this->Map.Clear();
        this->ClearCount = 0;
    }
    // �Q�[�g�����������I
    if (this->Next) {
        this->Next = false;
        this->Scene = this->SKILL_SELECT;
    }

    // �`�� //
    DxLib::ClearDrawScreen();
    int Scroll = this->Player.Sprite.Pos.GetX() - this->Player.StartPos.GetX();
    // �w�i
    this->Map.Draw(Scroll);
    // ��
    for (arrow a : this->Arrow) a.Draw(Scroll);
    // �G
    for (int i = 0; i < this->FlowerPlant.size(); i++) this->FlowerPlant[i].Draw(Scroll);
    for (int i = 0; i < this->Slime.size(); i++) this->Slime[i].Draw(Scroll);
    for (int i = 0; i < this->Golem.size(); i++) this->Golem[i].Draw(Scroll);
    for (int i = 0; i < this->Bat.size(); i++) this->Bat[i].Draw(Scroll);
    for (int i = 0; i < this->Tree.size(); i++) this->Tree[i].Draw(Scroll);
    for (int i = 0; i < this->Virus.size(); i++) this->Virus[i].Draw(Scroll);
    // �v���C���[
    this->Player.Draw();
    // �G�̒e
    for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Draw(Scroll);
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
    if (this->FadeOutCount++ >= 15) {
        this->FadeOutCount = 0;
        this->Scene = this->STAGE;
        //�v���C���[
        this->Player.Sprite.Pos.SetPos(48.0, this->Player.StartPos.GetY());
        this->Player.Sprite.Direction = 0.0;
        // �X�L��
        std::vector<player::skill> SkillLeft;
        for (auto s : this->Player.GetSkillLeft()) SkillLeft.push_back(s.first);
        int Rand = DxLib::GetRand(99);
        if (Rand < 80) this->Player.GiveSkill(SkillLeft[DxLib::GetRand(SkillLeft.size() - 1)]);
        else this->Player.GiveSkill(player::HEAL);
        // �}�b�v
        this->Map.NextStage();
    }
    // Update
    pos *InputDirection = new pos;
    this->Player.JoystickInput(InputDirection);
    delete InputDirection;
    for (int j = 0; j < 4; j++) {   // �X�s�[�h���グ�邽�ߓ�d
        for (int i = 0; i < this->Arrow.size(); i++) this->Arrow[i].Update();
    }
    for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Update();

    // �`��
    DxLib::ClearDrawScreen();
    int Scroll = this->Player.Sprite.Pos.GetX() - this->Player.StartPos.GetX();
    // �w�i
    this->Map.Draw(Scroll);
    // ��
    for (arrow a : this->Arrow) a.Draw(Scroll);
    // �v���C���[
    this->Player.Draw();
    // �G�̒e
    for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Draw(Scroll);
    // �t�F�[�h
    DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * FadeOutCount / 15);
    DxLib::DrawBox(0, 0, 1280, 720, 0x000000, TRUE);
    DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

    DxLib::ScreenFlip();

    return true;

}

bool Game::Die() {

    // ���� //
    this->FadeOutCount++;
    // Update
    if (this->FadeOutCount < 240) {
        pos *InputDirection = new pos;
        delete InputDirection;
        for (int j = 0; j < 4; j++) {   // �X�s�[�h���グ�邽�ߓ�d
            for (int i = 0; i < this->Arrow.size(); i++) this->Arrow[i].Update();
        }
        for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Update();
    }

    // �`��
    DxLib::ClearDrawScreen();
    if (this->FadeOutCount < 240) {
        int Scroll = this->Player.Sprite.Pos.GetX() - this->Player.StartPos.GetX();
        // �w�i
        this->Map.Draw(Scroll);
        // ��
        for (arrow a : this->Arrow) a.Draw(Scroll);
        // �G
        for (int i = 0; i < this->FlowerPlant.size(); i++) this->FlowerPlant[i].Draw(Scroll);
        for (int i = 0; i < this->Slime.size(); i++) this->Slime[i].Draw(Scroll);
        for (int i = 0; i < this->Golem.size(); i++) this->Golem[i].Draw(Scroll);
        for (int i = 0; i < this->Bat.size(); i++) this->Bat[i].Draw(Scroll);
        for (int i = 0; i < this->Tree.size(); i++) this->Tree[i].Draw(Scroll);
        for (int i = 0; i < this->Virus.size(); i++) this->Virus[i].Draw(Scroll);
        // �G�̒e
        for (int i = 0; i < this->Ball.size(); i++) this->Ball[i].Draw(Scroll);
    }
    // �X�R�A���
    if (this->FadeOutCount > 180) {
        DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * (this->FadeOutCount - 180) / 60);
        DxLib::DrawGraph(0, 0, this->EndGraph, FALSE);
        DxLib::SetFontSize(48 * 3.2);
        DxLib::DrawFormatString(64 * 3.2, 128 * 3.2, 0xff0000, "�X�e�[�W%d", this->Map.GetStage());
        DxLib::SetFontSize(16);
        DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
    }

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
