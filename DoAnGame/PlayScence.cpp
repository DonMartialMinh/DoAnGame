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
	this->id = id;
	player = NULL;
	bros = NULL;
	speedBar = NULL;
	item = NULL;
	gameclearboard = NULL;
	Dtime = NULL;
	board = NULL;
	button = NULL;
	isWaiting = 1;
	TimeWaitToScene = DWORD(GetTickCount64());
	key_handler = new CPlayScenceKeyHandler(this);


}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN			-1
#define SCENE_SECTION_TEXTURES			2
#define SCENE_SECTION_SPRITES			3
#define SCENE_SECTION_ANIMATIONS		4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS			6

#define OBJECT_TYPE_MARIO				0
#define OBJECT_TYPE_BRICK				1
#define OBJECT_TYPE_GOOMBA				2
#define OBJECT_TYPE_KOOPAS				3
#define OBJECT_TYPE_ENVIRONMENT			4
#define OBJECT_TYPE_UPSIDEBRICK			5
#define OBJECT_TYPE_COIN				6
#define OBJECT_TYPE_QBRICK				7
#define OBJECT_TYPE_FIREBALL			8
#define OBJECT_TYPE_FLYGOOMBA			9
#define OBJECT_TYPE_FLYKOOPAS			10
#define OBJECT_TYPE_PLANT				11
#define OBJECT_TYPE_PIRANHAPLANT		12
#define OBJECT_TYPE_SWITCH				13
#define OBJECT_TYPE_BROKENBRICK			14
#define OBJECT_TYPE_PBUTTON				15
#define OBJECT_TYPE_BOARD				16
#define OBJECT_TYPE_PLANTFIREBALL		17
#define OBJECT_TYPE_MUSHROOM			18
#define OBJECT_TYPE_LEAF				19
#define OBJECT_TYPE_ENDPOINTITEM		20
#define OBJECT_TYPE_GAMECLEARBOARD		21
#define OBJECT_TYPE_NUMBER				22
#define OBJECT_TYPE_SPEEDBAR			23
#define OBJECT_TYPE_ITEM				24
#define OBJECT_TYPE_BOOMERANGBROS		25
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
	float setting1 = 0;
	float setting2 = 0;

	if (tokens.size() == 5)
		object_setting = atoi(tokens[4].c_str());
	else if (tokens.size() == 6)
	{
		setting1 = float(atof(tokens[4].c_str()));
		setting2 = float(atof(tokens[5].c_str()));
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
		obj->type = OBJECT_TYPE_MARIO;
		player = (CMario*)obj; 

		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_GOOMBA: 
		obj = new CGoomba(); 
		obj->type = OBJECT_TYPE_GOOMBA;
		break;
	case OBJECT_TYPE_BRICK: 
		obj = new CBrick(); 
		obj->type = OBJECT_TYPE_BRICK;
		break;
	case OBJECT_TYPE_KOOPAS: 
		obj = new CKoopas(setting1, setting2);
		obj->type = OBJECT_TYPE_KOOPAS;
		break;
	case OBJECT_TYPE_ENVIRONMENT: 
		obj = new CEnvironment();
		obj->type = OBJECT_TYPE_ENVIRONMENT;
		break;
	case OBJECT_TYPE_UPSIDEBRICK: 
		obj = new CUpsideBrick(); 
		obj->type = OBJECT_TYPE_UPSIDEBRICK;
		break;
	case OBJECT_TYPE_COIN:	
		obj = new CCoin();
		obj->type = OBJECT_TYPE_COIN;
		break;
	case OBJECT_TYPE_QBRICK: 
		obj = new CQBrick(player, int(setting1), int(setting2), y);
		obj->type = OBJECT_TYPE_QBRICK;
		qbrick.push_back((CQBrick*)obj);
		break;
	case OBJECT_TYPE_FLYGOOMBA: 
		obj = new CFlyGoomba(); 
		obj->type = OBJECT_TYPE_FLYGOOMBA;
		break;
	case OBJECT_TYPE_FLYKOOPAS: 
		obj = new CFlyKoopas(); 
		obj->type = OBJECT_TYPE_FLYKOOPAS;
		break;
	case OBJECT_TYPE_PLANT: 
		obj = new CPlant(player, y); 
		obj->type = OBJECT_TYPE_PLANT;
		break;
	case OBJECT_TYPE_PIRANHAPLANT:
		obj = new CPiranhaPlant(player, y); 
		plant.push_back((CPiranhaPlant*)obj);
		obj->type = OBJECT_TYPE_PIRANHAPLANT;
		break;
	case OBJECT_TYPE_SWITCH:
		obj = new CSwitch(setting1, setting2);
		obj->type = OBJECT_TYPE_SWITCH;
		break;
	case OBJECT_TYPE_BROKENBRICK:
		obj = new CBrokenBrick();
		bbrick.push_back((CBrokenBrick*)obj);
		obj->type = OBJECT_TYPE_BROKENBRICK;
		break;
	case OBJECT_TYPE_BOARD:
		obj = new CBoard();
		board = (CBoard*)obj;
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_ENDPOINTITEM:
		obj = new CEndPointItem();
		item = (CEndPointItem*)obj;
		obj->type = OBJECT_TYPE_ENDPOINTITEM;
		break;
	case OBJECT_TYPE_GAMECLEARBOARD:
		obj = new CGameClearBoard();
		gameclearboard = (CGameClearBoard*)obj;
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_NUMBER:
		obj = new CNumber();
		if (object_setting == 0)
			numCoin.push_back((CNumber*)obj);
		else if (object_setting == 1)
			numTime.push_back((CNumber*)obj);
		else if (object_setting == 2)
			numScore.push_back((CNumber*)obj);
		else if (object_setting == 3)
			numLive.push_back((CNumber*)obj);
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_SPEEDBAR:
		obj = new CSpeedBar();
		speedBar = (CSpeedBar*)obj;
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_ITEM:
		obj = new CItem();
		itemList.push_back((CItem*)obj);
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_BOOMERANGBROS:
		obj = new CBoomerangBros(setting1, setting2);
		obj->type = OBJECT_TYPE_BOOMERANGBROS;
		bros = (CBoomerangBros*)obj;
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
		//DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
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

	//CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{

	TimeLapse();					// reduce time 1 sec
	if (isWaiting)
		return;

	vector<LPGAMEOBJECT> coObjects;
	CGame* game = CGame::GetInstance();

	if (player->transform || player->transformRacoon || player->GetState() == MARIO_STATE_DIE)	// frozen scene
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			if (objects[i]->type == OBJECT_TYPE_MARIO || objects[i]->isFinish || objects[i]->type == OBJECT_TYPE_ENVIRONMENT || objects[i]->type == OBJECT_TYPE_BOARD)
				continue;
			coObjects.push_back(objects[i]);
		}
		for (size_t i = 0; i < objects.size(); i++)
		{
			if (objects[i]->type == OBJECT_TYPE_MARIO)
				objects[i]->Update(dt, &coObjects);
		}
		return;
	}

	if (game->GetTime() < 0)
		player->SetState(MARIO_STATE_DIE);

	if (player->fireball)						// Draw fireball
	{
		player->fireball -= 1;
		objects.push_back(player->NewFireBall());
	}

	if (player->tail)						// Tail Attack
	{
		player->tail -= 1;
		objects.push_back(player->TailAttack());
	}

	if (gameclearboard->GetState() == BOARD_STATE_EMPTY)
	{
		if (item != NULL)
			if (item->sparkling == 1)
				gameclearboard->SetState(BOARD_STATE_STAR);
			else if (item->sparkling == 2)
				gameclearboard->SetState(BOARD_STATE_MUSHROOM);
			else if (item->sparkling == 3)
				gameclearboard->SetState(BOARD_STATE_FLOWER);
			else;
	}

	if (bros != NULL)
	{
		if (bros->boomerang)
		{
			bros->boomerang -= 1;
			objects.push_back(bros->NewBoomerang());
		}
	}

	for (int i = 0; i < int(plant.size()); i++)
	{
		if (plant[i]->fireball)						// Draw fireball
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
			CGameObject* obj;
			obj = qbrick[i]->ShowItem();
			objects.push_back(obj);
			if (obj->type == OBJECT_TYPE_PBUTTON)
			{
				button = (CPButton*)obj;
			}
		}
	}

	for (int i = 0; i < int(bbrick.size()); i++)
	{
		if (bbrick[i]->trigger)						// Draw fragment
		{
			bbrick[i]->trigger -= 1;
			vector<CGameObject*> temp = bbrick[i]->Broken();
			objects.insert(objects.end(), temp.begin(), temp.end());
		}
	}

	if (button != NULL)
	{
		if (button->trigger)
		{
			button->trigger -= 1;
			for (int i = 0; i < int(bbrick.size()); i++)
			{
				bbrick[i]->SetState(BROKENBRICK_STATE_COIN); // turn brokenbrick into coin
				bbrick[i]->type = OBJECT_TYPE_COIN;
			}
		}
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->type == OBJECT_TYPE_MARIO || objects[i]->isFinish || objects[i]->type == OBJECT_TYPE_ENVIRONMENT || objects[i]->type == OBJECT_TYPE_BOARD)
			continue;
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->type == OBJECT_TYPE_ENVIRONMENT)
			continue;
		objects[i]->Update(dt, &coObjects);
	}

	coObjects.clear();

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	if (player->GetState() == MARIO_STATE_DIE)
		return;


	float cx, cy;
	float camx, camy;
	player->GetPosition(cx, cy);
	UpdateCamera(cx, cy, id);			// Update camera
	game->GetCamPos(camx, camy);
	board->Update(camx, camy);		// Update Board follow mario
	UpdateBoardInfo(camx);			// Update BoardInfo

	if (player->x < camx) 
		player->x = camx;

	if (player->x > camx + game->GetScreenWidth() - 30.0f && player->GetState() != MARIO_STATE_ENDGAME) 
		player->x = camx + game->GetScreenWidth() - 30.0f;
}

void CPlayScene::Render()
{
	if (isWaiting)
		return;
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
	board = NULL;
	button = NULL;
	gameclearboard = NULL;
	item = NULL;
	plant.clear();
	qbrick.clear();
	bbrick.clear();
	objects.clear();
	numCoin.clear();
	numLive.clear();
	numScore.clear();
	numTime.clear();
	itemList.clear();

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	if (mario->GetState() == MARIO_STATE_DIE || mario->GetState() == MARIO_STATE_ENDGAME || mario->transform || mario->transformRacoon) return;
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
	if (mario->GetState() == MARIO_STATE_DIE || mario->GetState() == MARIO_STATE_ENDGAME || mario->transform || mario->transformRacoon) return;

	if (game->IsKeyDown(DIK_LSHIFT))
	{
		mario->isRunning = 1;
		if (abs(mario->GetVx()) >= MARIO_RUNNING_SPEED)
		{
			mario->SetState(MARIO_STATE_SLIDE);
		}

	}
	else {
		mario->isRunning = 0;
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

vector<int> CPlayScene::getNum(int number)
{
	vector<int> result;
	int odd;
	if (number == 0)
	{
		result.push_back(0);
	}
	else
	{
		while (number > 0)
		{
			odd = number % 10;
			result.push_back(odd);
			number = number / 10;
		}
	}
	return result;
}

void CPlayScene::UpdateBoardInfo(float camx)
{
	CGame* game = CGame::GetInstance();
	float xCoin = camx + 180.0f;
	vector<int> temp;
	temp = getNum(game->GetCoin());
	for (int i = 0, j = 0; i < int(numCoin.size()); i++, j++)
	{
		if (j < int(temp.size()))
			numCoin[i]->Update(xCoin, board->y + 17, temp[j]);
		else
			numCoin[i]->Update(xCoin, board->y + 17, 0);
		xCoin -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetTime());
	float xTime = camx + 180.0f;
	for (int i = numTime.size() - 1, j = 0; i >= 0; i--, j++)
	{
		if (j < int(temp.size()))
			numTime[i]->Update(xTime, board->y + 25, temp[j]);
		else
			numTime[i]->Update(xTime, board->y + 25, 0);
		xTime -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetLive());
	float xLive = camx + 77.0f;
	for (int i = numLive.size() - 1, j = 0; i >= 0; i--, j++)
	{
		if (j < int(temp.size()))
			numLive[i]->Update(xLive, board->y + 25, temp[j]);
		else
			numLive[i]->Update(xLive, board->y + 25, 0);
		xLive -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetScore());
	float xScore = camx + 140.0f;
	for (int i = 0, j = 0; i < int(numScore.size()); i++, j++)
	{
		if (j < int(temp.size()))
			numScore[i]->Update(xScore, board->y + 25, temp[j]);
		else
			numScore[i]->Update(xScore, board->y + 25, 0);
		xScore -= NUMBER_WIDTH;
	}

	float xSpeedBar = camx + 92.0f;
	speedBar->Update(xSpeedBar, board->y + 17, player->speedStack);

	temp.clear();
	temp = game->GetItemList();
	float xItemList = camx + 207.0f;
	for (int i = 0; i < int(itemList.size()); i++)
	{
		itemList[i]->SetState(temp[i]);
		itemList[i]->SetPosition(xItemList, board->y + 16);
		xItemList += 24.0f;
	}
}

void CPlayScene::TimeLapse()
{
	CGame* game = CGame::GetInstance();
	if (GetTickCount64() - Dtime > ONE_SEC)
	{
		Itime = 0;
		Dtime = 0;
		game->SubTime();
		Timing();
	}

	if (GetTickCount64() - TimeWaitToScene > ONE_SEC * 3 )
	{
		TimeWaitToScene = 0;
		isWaiting = 0;
	}
}

void CPlayScene::UpdateCamera(float cx, float cy, int id)
{
	CGame* game = CGame::GetInstance();
	float camx, camy;
	game->GetCamPos(camx, camy);
	if (id == MAP_1)
	{
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
		else if (cx > 2661.0f)
		{
			if (cy < 50.0f)
			{
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(2508.0f, round(cy));
			}
			else
				CGame::GetInstance()->SetCamPos(2508.0f, 0.0f); //set Cam when game end
		}
		else
		{
			if (cy < 50.0f)
			{
				cx -= game->GetScreenWidth() / 2;
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(round(cx), round(cy));
			}
			else if (cy > 0 && cy < 240)
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
	}
	else if (id == MAP_2)
	{
		if (player->x > 2034.0f)
		{
			if (cx < 2304.0f + game->GetScreenWidth() / 2)
			{
				if (cy < 50.0f)
				{
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(2304.0f, round(cy));
				}
				else if (cy > 0 && cy < 240)
					CGame::GetInstance()->SetCamPos(2304.0f, 0.0f); // set Cam when game start
			}
			else if (cx > 2661.0f)
			{
				if (cy < 50.0f)
				{
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(2508.0f, round(cy));
				}
				else
					CGame::GetInstance()->SetCamPos(2508.0f, 0.0f); //set Cam when game end
			}
			else
			{
				if (cy < 50.0f)
				{
					cx -= game->GetScreenWidth() / 2;
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(round(cx), round(cy));
				}
				else if (cy > 0 && cy < 240)
				{
					cx -= game->GetScreenWidth() / 2;
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(round(cx), 0.0f); // set Cam Focus
				}
			}
		}
		else
		{
			if (temp == 0)
				temp = 1.0f;
			else
				temp = 0;

			if (camx < 1728.0f)
				CGame::GetInstance()->SetCamPos(round(camx + temp), 0.0f);
		}
	}
}