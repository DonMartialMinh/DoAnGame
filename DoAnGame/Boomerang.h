#pragma once
#include "GameObject.h"
#include "Game.h"


#define BOOMERANG_SPIN_SPEED_VY		0.01f 
#define BOOMERANG_SPIN_SPEED_VX		0.23f 

#define BOOMERANG_BBOX_WIDTH	16
#define BOOMERANG_BBOX_HEIGHT	15

#define BOOMERANG_GRAVITY		0.0002f


class CBoomerang : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	int nx;
	int	ny;
	CBoomerang();
};