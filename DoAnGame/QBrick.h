#pragma once
#include "GameObject.h"
#include "Coin.h"
#include "MushRoom.h"
#include "Mario.h"
#include "Leaf.h"
#include "PButton.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16

#define BRICK_STATE_QUES 100
#define BRICK_STATE_EMP 200

#define BRICK_ANI_QUES 0
#define BRICK_ANI_EMP 1

#define BRICK_RINGING_TIME 200

class CQBrick : public CGameObject
{
	DWORD ring_start;
	CGameObject* player = NULL; 
	float min = 0;

public:
	int setting = 0;
	int ringing = 0;
	int trigger = 0;
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void StartRinging() { ringing = 1; ring_start = DWORD(GetTickCount64()); }
	CQBrick(CGameObject* player, int setting, float y);
	CGameObject* ShowItem();
};