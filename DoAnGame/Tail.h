#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Game.h"
#include "Goomba.h"
#include "FlyGoomba.h"
#include "Koopas.h"
#include "FlyKoopas.h"
#include "Plant.h"
#include "PiranhaPlant.h"
#include "QBrick.h"
#include "BrokenBrick.h"

#define TAIL_BBOX_WIDTH  8
#define TAIL_BBOX_HEIGHT 9

#define TAIL_SPEED		0.25f 


class CTail : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	int maxX = 0;
	CTail(int nx, float maxX);
};