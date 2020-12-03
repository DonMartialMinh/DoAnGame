#include "FlyKoopas.h"
#include "Brick.h"
#include "UpsideBrick.h"
#include "Koopas.h"

CFlyKoopas::CFlyKoopas()
{
	SetState(FLYKOOPAS_STATE_FLYING);
}

void CFlyKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x;
	top = y;
	right = x + FLYKOOPAS_BBOX_WIDTH;
	if (state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT)
		bottom = y + FLYKOOPAS_BBOX_HEIGHT_DIE;
	else
		bottom = y + FLYKOOPAS_BBOX_HEIGHT;
}

void CFlyKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);

	//
	// TO-DO: make sure Koopas can interact with the world and to each of them too!
	// 

	vy += FLYKOOPAS_GRAVITY * dt;

	if (vx != 0)
		isHolded = 0;

	if (isHolded)
		vy = 0;


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != FLYKOOPAS_STATE_DIE_DEFLECT_OUT)
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
		y += min_ty * dy + ny * 0.4f;
		if (ny != 0)
		{
			if (state == FLYKOOPAS_STATE_FLYING)
				vy = -FLYKOOPAS_DEFLECT_SPEED;
			else
				vy = 0;
		}
		float temp = vy;
		//if (nx != 0) vx = 0;


		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
				}
				else
				{
					if (e->nx)
					{
						vx = -vx;
						goomba->vx = -goomba->vx;
					}
				}
			}
			else if (dynamic_cast<CFlyGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					goomba->SetState(FLYGOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
				}
				else
				{
					if (e->nx)
					{
						vx = -vx;
						goomba->vx = -goomba->vx;
					}
				}
			}
			else if (dynamic_cast<CKoopas*>(e->obj))	// if e->obj is koopas
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
					//koopas->vx = 0.05f * this->nx;
				}
				else if (state == FLYKOOPAS_STATE_WALKING)
				{
					vx = -vx;
					koopas->vx = -koopas->vx;
				}
			}
			else if (dynamic_cast<CFlyKoopas*>(e->obj))	// if e->obj is Flykoopas
			{
				CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
					//koopas->vx = 0.05f * this->nx;
				}
				else if (state == FLYKOOPAS_STATE_WALKING)
				{
					vx = -vx;
					koopas->vx = -koopas->vx;
				}
			}
			else if (dynamic_cast<CUpsideBrick*>(e->obj))	// if e->obj is UpsideBrick 
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);
				if (state == FLYKOOPAS_STATE_WALKING)
				{
					if (e->ny > 0 || this->y + FLYKOOPAS_BBOX_HEIGHT > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						x += dx;
						y += dy;
					}
				}
				else
				{
					if (e->ny > 0 || this->y + FLYKOOPAS_BBOX_HEIGHT_DIE > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						x += dx;
						y += dy;
					}
				}
			}
			else if (dynamic_cast<CQBrick*>(e->obj))		//question brick
			{
				CQBrick* qbrick = dynamic_cast<CQBrick*>(e->obj);
				if (e->nx != 0 && ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0))
				{
					if (qbrick->GetState() != BRICK_STATE_EMP)
					{
						qbrick->SetState(BRICK_STATE_EMP);
						qbrick->StartRinging();
					}
				}
				if (abs(nx) > 0.0001f)
					vx = -vx;
				if (abs(ny) > 0.0001f)
					vy = -vy;
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

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}
}



void CFlyKoopas::Render()
{
	int ani = FLYKOOPAS_ANI_WALKING_LEFT;
	if (state == FLYKOOPAS_STATE_DIE && vx == 0)
		ani = FLYKOOPAS_ANI_DIE;
	else if ((state == FLYKOOPAS_STATE_DIE_DEFLECT && vx == 0) || state == FLYKOOPAS_STATE_DIE_DEFLECT_OUT)
		ani = FLYKOOPAS_ANI_DIE_DEFLECT;
	else if (state == KOOPAS_STATE_DIE && vx > 0)
		ani = FLYKOOPAS_ANI_SPIN_RIGHT;
	else if (state == FLYKOOPAS_STATE_DIE && vx < 0)
		ani = FLYKOOPAS_ANI_SPIN_LEFT;
	else if (state == FLYKOOPAS_STATE_DIE_DEFLECT && vx > 0)
		ani = FLYKOOPAS_ANI_SPIN_RIGHT_DEFLECT;
	else if (state == FLYKOOPAS_STATE_DIE_DEFLECT && vx < 0)
		ani = FLYKOOPAS_ANI_SPIN_LEFT_DEFLECT;
	else if (vx > 0 && state == FLYKOOPAS_STATE_FLYING) ani = FLYKOOPAS_ANI_FLY_RIGHT;
	else if (vx <= 0 && state == FLYKOOPAS_STATE_FLYING) ani = FLYKOOPAS_ANI_FLY_LEFT;
	else if (vx > 0) ani = FLYKOOPAS_ANI_WALKING_RIGHT;
	else if (vx <= 0) ani = FLYKOOPAS_ANI_WALKING_LEFT;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void CFlyKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FLYKOOPAS_STATE_DIE:
		y += FLYKOOPAS_BBOX_HEIGHT - FLYKOOPAS_BBOX_HEIGHT_DIE;
		vx = 0;
		vy = 0;
		break;
	case FLYKOOPAS_STATE_DIE_DEFLECT:
		vy = -FLYKOOPAS_DIE_DEFLECT_SPEED;
		vx = 0;
		break;
	case FLYKOOPAS_STATE_DIE_DEFLECT_OUT:
		vy = -FLYKOOPAS_DIE_DEFLECT_SPEED;
		isFinish = 1;
		break;
	case FLYKOOPAS_STATE_WALKING:
		vy = 0;
		break;
	case FLYKOOPAS_STATE_FLYING:
		vx = -FLYKOOPAS_WALKING_SPEED;
		break;
	}

}
