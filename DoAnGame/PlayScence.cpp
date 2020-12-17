#include <iostream>
#include <fstream>
#include "PlayScence.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include <cmath> 

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	player = NULL;
	key_handler = new CPlayScenceKeyHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS	6

#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_BRICK	1
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPAS	3
#define OBJECT_TYPE_ENVIRONMENT 4
#define OBJECT_TYPE_UPSIDEBRICK 5
#define OBJECT_TYPE_COIN 6
#define OBJECT_TYPE_QBRICK 7
#define OBJECT_TYPE_FIREBALL 8
#define OBJECT_TYPE_FLYGOOMBA 9
#define OBJECT_TYPE_FLYKOOPAS 10
#define OBJECT_TYPE_PLANT 11
#define OBJECT_TYPE_PIRANHAPLANT 12
#define OBJECT_TYPE_SWITCH 13
#define OBJECT_TYPE_BROKENBRICK 14
#define OBJECT_TYPE_PBUTTON 15
#define OBJECT_TYPE_BOARD 16
#define OBJECT_TYPE_PORTAL	50

#define MAX_SCENE_LINE 1024


void CPlayScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < int(tokens.size()); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < int(tokens.size()); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS]
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = float(atof(tokens[1].c_str()));
	float y = float(atof(tokens[2].c_str()));
	int ani_set_id = atoi(tokens[3].c_str());
	int object_setting = 0;
	float xSwitch = 0;
	float ySwitch = 0;

	if (tokens.size() == 5)
		object_setting = atoi(tokens[4].c_str());
	else if (tokens.size() == 6)
	{
		xSwitch = float(atof(tokens[4].c_str()));
		ySwitch = float(atof(tokens[5].c_str()));
	}

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new CMario(x, y);
		obj->type = 0;
		player = (CMario*)obj; 

		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_GOOMBA: 
		obj = new CGoomba(); 
		obj->type = 1;
		break;
	case OBJECT_TYPE_BRICK: obj = new CBrick(); break;
	case OBJECT_TYPE_KOOPAS: obj = new CKoopas(); break;
	case OBJECT_TYPE_ENVIRONMENT: 
		obj = new CEnvironment();
		obj->type = 4;
		break;
	case OBJECT_TYPE_UPSIDEBRICK: obj = new CUpsideBrick(); break;
	case OBJECT_TYPE_COIN:	obj = new CCoin(); break;
	case OBJECT_TYPE_QBRICK: 
		obj = new CQBrick(player, object_setting, y);
		qbrick.push_back((CQBrick*)obj);
		break;
	case OBJECT_TYPE_FLYGOOMBA: obj = new CFlyGoomba(); break;
	case OBJECT_TYPE_FLYKOOPAS: obj = new CFlyKoopas(); break;
	case OBJECT_TYPE_PLANT: 
		obj = new CPlant(player, y); 
		break;
	case OBJECT_TYPE_PIRANHAPLANT:
		obj = new CPiranhaPlant(player, y); 
		plant.push_back((CPiranhaPlant*)obj);
		break;
	case OBJECT_TYPE_SWITCH:
		obj = new CSwitch(xSwitch, ySwitch);
		break;
	case OBJECT_TYPE_BROKENBRICK:
		obj = new CBrokenBrick();
		bbrick.push_back((CBrokenBrick*)obj);
		break;
	case OBJECT_TYPE_PBUTTON:
		obj = new CPButton();
		button = (CPButton*)obj;
		break;
	case OBJECT_TYPE_BOARD:
		obj = new CBoard();
		board = (CBoard*)obj;
		break;
	case OBJECT_TYPE_PORTAL:
	{
		float r = float(atof(tokens[4].c_str()));
		float b = float(atof(tokens[5].c_str()));
		int scene_id = atoi(tokens[6].c_str());
		obj = new CPortal(x, y, r, b, scene_id);
	}
	break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
}

void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(237, 28, 36));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	if (player->fireball > 0)						// Draw fireball
	{
		player->fireball -= 1;
		objects.push_back(player->NewFireBall());
	}

	for (int i = 0; i < int(plant.size()); i++)
	{
		if (plant[i]->fireball > 0)						// Draw fireball
		{
			plant[i]->fireball -= 1;
			objects.push_back(plant[i]->NewFireBall());
		}
	}

	for (int i = 0; i < int(qbrick.size()); i++)
	{
		if (qbrick[i]->trigger)						// show item
		{
			qbrick[i]->trigger -= 1;
			objects.push_back(qbrick[i]->ShowItem());
		}
	}

	for (int i = 0; i < int(bbrick.size()); i++)
	{
		if (bbrick[i]->trigger > 0)						// Draw fragment
		{
			bbrick[i]->trigger -= 1;
			vector<CGameObject*> temp = bbrick[i]->Broken();
			objects.insert(objects.end(), temp.begin(), temp.end());
		}

		if (button->trigger > 0)
		{												// Turn broken block into coin
			bbrick[i]->SetState(BROKENBRICK_STATE_COIN);
		}
	}


	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	// Update camera to follow mario
	float cx, cy;
	float camx, camy;
	player->GetPosition(cx, cy);
	CGame* game = CGame::GetInstance();
	game->GetCamPos(camx, camy);

	if (player->GetState() == MARIO_STATE_DIE)
		return;

	if (cx < game->GetScreenWidth() / 2)
	{
		if (cy < 50.0f)
		{
			cy -= 50.0f;
			CGame::GetInstance()->SetCamPos(0.0f, round(cy));
		}
		else if (cy > 0 && cy < 240)
			CGame::GetInstance()->SetCamPos(0.0f, 0.0f); // set Cam when game start
		else if (cy > 240 && cy < 442)					// Cam in other screen
		{
			cy -= game->GetScreenHeight() / 2;
			CGame::GetInstance()->SetCamPos(0.0f, 240.0f);
		}
	}
	else if (cx > 2675.0f)
	{
		if (cy < 50.0f)
		{
			cy -= 50.0f;
			CGame::GetInstance()->SetCamPos(2526.0f, round(cy));
		}
		else
			CGame::GetInstance()->SetCamPos(2526.0f, 0.0f); //set Cam when game end
	}
	else
	{
		if (cy < 50.0f )
		{
			cx -= game->GetScreenWidth() / 2;
			cy -= 50.0f;
			CGame::GetInstance()->SetCamPos(round(cx), round(cy));
		}
		else if (cy > 0 && cy < 240 )
		{
			cx -= game->GetScreenWidth() / 2;
			cy -= 50.0f;
			CGame::GetInstance()->SetCamPos(round(cx), 0.0f); // set Cam Focus
		}
		else if (cy > 240 && cy < 442)					// Cam in other screen
		{
			cx -= game->GetScreenWidth() / 2;
			cy -= 50.0f;
			CGame::GetInstance()->SetCamPos(round(cx), 240.0f);
		}
	}

	board->Update();		// Update Board follow mario
}

void CPlayScene::Render()
{
	for (int i = 0; i < int(objects.size()); i++)
		objects[i]->Render();
}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	for (int i = 0; i < int(objects.size()); i++)
		delete objects[i];

	objects.clear();
	player = NULL;

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMario* mario = ((CPlayScene*)scence)->GetPlayer();
	if (mario->GetState() == MARIO_STATE_DIE) return;
	switch (KeyCode)
	{
	case DIK_X:
		if ((mario->getLevel() == MARIO_LEVEL_RACOON && mario->isFlying == 1 && mario->isRunning == 1 && mario->sliding == 1) || mario->flying)			//condition to fly
			mario->SetState(MARIO_RACOON_STATE_FLY);
		else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->isFlying == 1)
			mario->SetState(MARIO_RACOON_STATE_FALL);
		else if (mario->GetState() != MARIO_STATE_DUCK)
			mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_A:
		mario->Reset();
		break;
	case DIK_T:
		mario->SetPosition(138,382);
		break;
	case DIK_Z:
		if (mario->getLevel() == MARIO_LEVEL_RACOON)
			mario->SetState(MARIO_RACOON_STATE_TAIL);
		else if (mario->getLevel() == MARIO_LEVEL_FIRE)
			mario->SetState(MARIO_FIRE_STATE_THROW);						
		break;
	case DIK_1:
		if (mario->getLevel() != MARIO_LEVEL_SMALL)
			CMario::ToSmall(mario->y);
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		if (mario->getLevel() == MARIO_LEVEL_SMALL)
			CMario::ToBig(mario->y);
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		if (mario->getLevel() == MARIO_LEVEL_SMALL)
			CMario::ToBig(mario->y);
		mario->SetLevel(MARIO_LEVEL_FIRE);
		break;
	case DIK_4:
		if (mario->getLevel() == MARIO_LEVEL_SMALL)
			CMario::ToBig(mario->y);
		mario->SetLevel(MARIO_LEVEL_RACOON);
		break;
	}
}

void CPlayScenceKeyHandler::OnKeyUp(int KeyCode)
{
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_DOWN:
		if (mario->isFlying == 0 && mario->getLevel() != MARIO_LEVEL_SMALL)
		{
			mario->isDucking = 0;
			mario->y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_BIG_DUCK_BBOX_HEIGHT);
		}
		break;
	}

}

void CPlayScenceKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE) return;

	if (game->IsKeyDown(DIK_LSHIFT))
	{
		mario->isRunning = 1;
		if (mario->GetVx() != 0 && mario->canSlide == 0)
		{
			mario->SetState(MARIO_STATE_SLIDE);
		}

	}
	else {
		mario->isRunning = 0;
		mario->canSlide = 0;
		mario->sliding = 0;
	}

	if (game->IsKeyDown(DIK_C))
	{
		mario->canHold = 1;
	}
	else
	{
		mario->canHold = 0;
		mario->holding = 0;
	}

	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (mario->vx < 0 && mario->turning == 0)
			mario->SetState(MARIO_RACOON_STATE_TURN);
		else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->nx < 0)
		{
			mario->x = mario->x - (MARIO_RACOON_BBOX_WIDTH - MARIO_BIG_BBOX_WIDTH);
		}
		mario->SetState(MARIO_STATE_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (mario->vx > 0 && mario->turning == 0)
			mario->SetState(MARIO_RACOON_STATE_TURN);
		else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->nx > 0)
		{
			mario->x = mario->x + MARIO_RACOON_BBOX_WIDTH - MARIO_BIG_BBOX_WIDTH;
		}
		mario->SetState(MARIO_STATE_WALKING_LEFT);
	}
	else if (game->IsKeyDown(DIK_DOWN))
	{
		mario->KeyDownPressed = 1;
		if (mario->getLevel() != MARIO_LEVEL_SMALL)
			mario->SetState(MARIO_STATE_DUCK);
	}
	else if (game->IsKeyDown(DIK_UP))
	{
		mario->KeyUpPressed = 1;
	}
	else
	{
		mario->KeyUpPressed = 0;
		mario->KeyDownPressed = 0;
		mario->SetState(MARIO_STATE_IDLE);	
	}

}
