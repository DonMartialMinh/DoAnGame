#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scence.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Environment.h"
#include "Koopas.h"
#include "UpsideBrick.h"
#include "Coin.h"
#include "QBrick.h"
#include "FlyGoomba.h"
#include "FireBall.h"
#include "FlyKoopas.h"
#include "Plant.h"
#include "PiranhaPlant.h"
#include "Switch.h"
#include "BrokenBrick.h"
#include "PButton.h"
#include "Board.h"
#include "EndPointItem.h"
#include "GameClearBoard.h"
#include "Number.h"
#include "SpeedBar.h"
#include "Tail.h"
#include "Item.h"

#define ONE_SEC 1000

#define MAP_1	2
#define MAP_2	3


class CPlayScene : public CScene
{
protected:
	CMario* player;					// A play scene has to have player, right? 
	CBoard* board;
	CPButton* button ;
	CGameClearBoard* gameclearboard ;
	CEndPointItem* item ;
	vector<CPiranhaPlant*> plant;
	vector<CQBrick*> qbrick;
	vector<CBrokenBrick*> bbrick;
	vector<LPGAMEOBJECT> objects;
	DWORD Dtime;
	DWORD TimeWaitToScene;
	int Itime = 0;
	int isWaiting;
	// game play
	vector<CNumber*> numCoin;
	vector<CNumber*> numTime;
	vector<CNumber*> numScore;
	vector<CNumber*> numLive;
	CSpeedBar* speedBar;
	vector<CItem*> itemList;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);

public:

	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	CMario* GetPlayer() { return player; }
	vector<int> getNum(int number);
	void UpdateBoardInfo(float camX);
	void TimeLapse();
	void UpdateCamera(float cx, float cy);
	void Timing() { Itime = 1; Dtime = DWORD(GetTickCount64()); }			// reduce gameplay time one sec
	//friend class CPlayScenceKeyHandler;
};

class CPlayScenceKeyHandler : public CScenceKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlayScenceKeyHandler(CScene* s) :CScenceKeyHandler(s) {};
};

