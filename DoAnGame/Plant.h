#pragma once
#include "GameObject.h"

#define PLANT_BBOX_WIDTH  16
#define PLANT_BBOX_HEIGHT 24

#define PLANT_RISING_TIME 4500

class CPlant : public CGameObject
{
	DWORD rise_start;
	int rising = 0;
public:
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CPlant();
	void StartRising() { rising = 1; rise_start = GetTickCount(); }
};