#include "Coin.h"

CCoin::CCoin()
{
	SetState(COIN_STATE_COIN);
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (state == COIN_STATE_TRANS)
	{
		l = 0;
		t = 0;
		r = 0;
		b = 0;
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
	int ani = COIN_ANI_COIN;
	if (state == COIN_STATE_TRANS) {
		ani = COIN_ANI_TRANS;
	}

	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

