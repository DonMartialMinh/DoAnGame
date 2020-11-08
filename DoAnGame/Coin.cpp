#include "Coin.h"

CCoin::CCoin()
{
	SetState(COIN_STATE_COIN);
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
	{
		l = NULL;
		t = NULL;
		r = NULL;
		b = NULL;
	}
	else {

		l = x;
		t = y;
		r = x + COIN_BBOX_WIDTH;
		b = y + COIN_BBOX_HEIGHT;
	}
}

void CCoin::Render()
{
	if (isFinish)		//delete coin 
		return;
	int ani = COIN_ANI_COIN;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

