#include "FireBall.h"

CFireBall::CFireBall(int nx)
{
	//SetState(FIREBALL_STATE_SPIN);
	vx = FIREBALL_SPIN_SPEED * nx;
}

void CFireBall::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	//l = x;
	//t = y;
	//r = x + FIREBALL_BBOX_WIDTH;
	//b = y + FIREBALL_BBOX_HEIGHT;
}

void CFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	x += dx;
	//vy = -0.05f;
	//y += dy;
}

void CFireBall::Render()
{
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