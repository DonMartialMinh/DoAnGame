#include "BoomerangBros.h"
#include "Utils.h"

CBoomerangBros::CBoomerangBros(float max, float min)
{
	xMax = round(max);
	xMin = round(min);
	SetState(BROS_STATE_WALKING);

}

void CBoomerangBros::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x + BROS_BOOMERANG_BBOX_WIDTH - BROS_BBOX_WIDTH;
	top = y + BROS_BOOMERANG_BBOX_HEIGHT - BROS_BBOX_HEIGHT;
	right = left + BROS_BBOX_WIDTH;
	bottom = top + BROS_BBOX_HEIGHT;
}

void CBoomerangBros::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);
	if (y < camy || y > camy + scrh)		// out screen height then delete
		return;


	CGameObject::Update(dt);

	//
	// TO-DO: make sure Koopas can interact with the world and to each of them too!
	// 
	vy += BROS_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != BROS_STATE_DIE)
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

		// todo: this is a very ugly designed function!!!!
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

	if (state == BROS_STATE_WALKING)
	{
		if (vx < 0 && x < xMax)
		{
			x = xMax; vx = -vx;
		}
		else if (vx > 0 && x > xMin)
		{
			x = xMin; vx = -vx;
		}
	}

}



void CBoomerangBros::Render()
{
	int ani = BROS_ANI_WALKING;
	if (state == BROS_STATE_DIE && vx == 0)
		ani = BROS_ANI_DIE;
	animation_set->at(ani)->Render(round(x), round(y));
}

void CBoomerangBros::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BROS_STATE_DIE:
		vx = 0;
		vy = 0;
		break;
	case BROS_STATE_WALKING:
		vx = BROS_WALKING_SPEED;
		break;
	}

}
