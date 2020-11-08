#include "FireBall.h"

CFireBall::CFireBall(int nx)
{
	//SetState(FIREBALL_STATE_SPIN);
	vx = FIREBALL_SPIN_SPEED * nx;
}

void CFireBall::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)	//turn of boundingbox
	{
		l = NULL;
		t = NULL;
		r = NULL;
		b = NULL;
	}
	else
	{

		l = x;
		t = y;
		r = x + FIREBALL_BBOX_WIDTH;
		b = y + FIREBALL_BBOX_HEIGHT;
	}
}

void CFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = game->GetScreenWidth();
	float scrh = game->GetScreenHeight();
	game->GetCamPos(camx, camy);

	if (x < camx || x > camx + scrw)						// delete fireball if out map
		isFinish = 1;
	if (y < camy || y > camy + scrh)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);
	x += dx;
	y += dy;
	vy += FIREBALL_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);
	float min_tx, min_ty, nx = 0, ny;
	float rdx = 0;
	float rdy = 0;
	FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
	for (int i = 0; i < coEventsResult.size(); i++)
	{
		LPCOLLISIONEVENT e = coEventsResult[i];
		if (dynamic_cast<CBrick*>(e->obj))
		{
			if (e->nx != 0)
			{
				isFinish = 1;			//delete fireball when collide with wall
				vx = 0;
				vy = 0;
				return;
			}
			else
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
				if (abs(ny) > 0.0001f)
					vy = -vy;
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CFireBall::Render()
{
	if (isFinish)
		return;
	int ani = FIREBALL_SPIN_RIGHT;
	if (vx > 0) ani = FIREBALL_SPIN_RIGHT;
	else if (vx <= 0) ani = FIREBALL_SPIN_LEFT;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}


void CFireBall::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FIREBALL_STATE_SPIN:
		vx = FIREBALL_SPIN_SPEED;
		break;
	}
}