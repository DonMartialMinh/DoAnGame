#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16

#define BRICK_STATE_QUES 100
#define BRICK_STATE_EMP 200

#define BRICK_ANI_QUES 0
#define BRICK_ANI_EMP 1

#define BRICK_RINGING_TIME 100

class CQBrick : public CGameObject
{
	DWORD ring_start;
public:
	int ringing = 0;
	CQBrick();
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void StartRinging() { ringing = 1; ring_start = GetTickCount(); }
};