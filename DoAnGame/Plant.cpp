#include "Plant.h"

CPlant::CPlant()
{
	StartRising();
}

void CPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + PLANT_BBOX_WIDTH;
	b = y + PLANT_BBOX_HEIGHT;
}

void CPlant::Render()
{
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}

void CPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);

	if (GetTickCount64() - rise_start > PLANT_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
		StartRising();
	}

	if (rising)
	{
		if (GetTickCount64() - rise_start >= 3000)
			y += 0.3;
		else if (GetTickCount64() - rise_start > 1500 || GetTickCount64() - rise_start < 3000);
		else
			y -= 0.3;
	}
}

