#include "Plant.h"
#include "Utils.h"

CPlant::CPlant(CGameObject*player, float y)
{
	this->player = player;
	min = y;
	max = y - PLANT_BBOX_HEIGHT;
	StartRising();
}

void CPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + PLANT_BBOX_WIDTH;
	b = y + PLANT_BBOX_HEIGHT;
}

void CPlant::Render()
{
	if (isFinish)
		return;
	animation_set->at(0)->Render(x, y);
	//RenderBoundingBox();
}

void CPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish)
		return;
	CGameObject::Update(dt);

	if (GetTickCount64() - rise_start > PLANT_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
		if (player->y >= this->y - 30 && player->x + 25 > this->x && player->x < this->x + 25);
		else 
			StartRising();
	}

	if (rising)
	{
		isUnderPipe = 0;
		if (GetTickCount64() - rise_start >= 0 && GetTickCount64() - rise_start <= 1500)
			y -= 0.25;
		else if (GetTickCount64() - rise_start >= 1500 && GetTickCount64() - rise_start <= 2500)
		{
			y = max;
		}
		else if (GetTickCount64() - rise_start >= 2500 && GetTickCount64() - rise_start <= 4000)
			y += 0.25;
		else {
			isUnderPipe = 1;
			y = min;
		}
	}
}

