#include "MushRoom.h"
#include "Utils.h"

CMushRoom::CMushRoom(float y)
{
	max = y - MUSHROOM_BBOX_HEIGHT - 1;
	StartRising();
}

void CMushRoom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + MUSHROOM_BBOX_WIDTH;
	b = y + MUSHROOM_BBOX_HEIGHT;
}

void CMushRoom::Render()
{
	if (isFinish)
		return;
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}

void CMushRoom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish)
		return;
	CGameObject::Update(dt);


	if (GetTickCount64() - rise_start > MUSHROOM_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
	}

	if (rising)
		y -= 0.5;
	else
		state = MUSHROOM_STATE_MOVING;

	if (state == MUSHROOM_STATE_MOVING)	// Out of QBrick
	{
		vx = -MUSHROOM_MOVING_SPEED;
		vy += MUSHROOM_GRAVITY * dt;

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();

		CalcPotentialCollisions(coObjects, coEvents);

		if (coEvents.size() == 0)
		{
			x += dx;
			y += dy;
		}
		else
		{

			float min_tx, min_ty, nx = 0, ny;
			float rdx = 0;
			float rdy = 0;

			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

			x += min_tx * dx + nx * 0.4f;
			//y += min_ty * dy + ny * 0.4f;

			if (ny != 0) vy = 0;

			for (int i = 0; i < int(coEventsResult.size()); i++)
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
			}

		}
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	}
}

void CMushRoom::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case MUSHROOM_STATE_MOVING:
		vx = -MUSHROOM_MOVING_SPEED;
		break;
	}
}
