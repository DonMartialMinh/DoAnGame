#pragma once
#include "GameObject.h"
#include "Game.h"


#define FRAGMENT_SPIN_SPEED_VX	0.05f 
#define FRAGMENT_SPIN_SPEED_VY	0.4f 

#define FRAGMENT_SPIN_SPEED_VX1	0.05f 
#define FRAGMENT_SPIN_SPEED_VY1	0.2f 
#define FRAGMENT_GRAVITY		0.001f


class CFragment : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	int isFinish = 0;
	int nx = 0;
	int ny = 0;
	CFragment(int side);

};