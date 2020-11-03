#include "Koopas.h"
#include "Brick.h"
#include "UpsideBrick.h"

CKoopas::CKoopas()
{
	SetState(KOOPAS_STATE_WALKING);
}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;

	if (state == KOOPAS_STATE_DIE)
		bottom = y + KOOPAS_BBOX_HEIGHT_DIE;
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Koopas can interact with the world and to each of them too!
	// 

	vy += KOOPAS_GRAVITY * dt;

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

		// todo: this is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block every object first!
		x += min_tx * dx + nx * 0.4f;
		//y += min_ty * dy + ny * 0.4f;

		float temp = vy;
		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		for (int i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is goomba 
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				//koopas->vx = -koopas->vx;
				vx = -vx;
					/*vx = -vx;
					koopas->vx = -koopas->vx;*/ // 2 Goombas change direction if they collide
			}
			else if (dynamic_cast<CUpsideBrick*>(e->obj))
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);

				if (e->ny >= 0) 
				{
					//If wrong side then go through
					vy = temp;
					x -= min_tx * dx + nx * 0.4f;
					x += dx;
					y += dy;
				}
			}
			else if (dynamic_cast<CBrick*>(e->obj))
			{

				if (abs(nx) > 0.0001f)
					vx = -vx;
				if (abs(ny) > 0.0001f)
					vy = -vy;
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}
}

void CKoopas::Render()
{
	int ani = KOOPAS_ANI_WALKING_LEFT;
	if (state == KOOPAS_STATE_DIE && vx == 0) {
		ani = KOOPAS_ANI_DIE;
	}
	else if (state == KOOPAS_STATE_DIE && vx > 0)
		ani = KOOPAS_ANI_SPIN_RIGHT;
	else if (state == KOOPAS_STATE_DIE && vx < 0)
		ani = KOOPAS_ANI_SPIN_LEFT;
	else if (vx > 0) ani = KOOPAS_ANI_WALKING_RIGHT;
	else if (vx <= 0) ani = KOOPAS_ANI_WALKING_LEFT;
	animation_set->at(ani)->Render(x, y);

	RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPAS_STATE_DIE:
		y += KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_DIE;
		vx = 0;
		vy = 0;
		break;
	case KOOPAS_STATE_WALKING:
		vx = -KOOPAS_WALKING_SPEED;
		break;
	}

}