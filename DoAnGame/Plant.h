#pragma once
#include "GameObject.h"
#include "Mario.h"

#define PLANT_BBOX_WIDTH  16
#define PLANT_BBOX_HEIGHT 24

#define PLANT_RISING_TIME 6000

class CPlant : public CGameObject
{
	DWORD rise_start;
	int rising = 0;
	CGameObject *player = NULL;
	float min;
	float max;
public:
	int isUnderPipe = 0;
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CPlant(CGameObject* player, float y);
	void StartRising() { rising = 1; rise_start = GetTickCount(); }
};