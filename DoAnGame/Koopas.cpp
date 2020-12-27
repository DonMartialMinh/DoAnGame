#include "Koopas.h"
#include "Brick.h"
#include "UpsideBrick.h"
#include "Utils.h"

CKoopas::CKoopas(float max, float min)
{
	xMax = round(max);
	xMin = round(min);
	SetState(KOOPAS_STATE_WALKING);

}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;

	if (state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT)
			bottom = y + KOOPAS_BBOX_HEIGHT_DIE;
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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

	vy += KOOPAS_GRAVITY * dt;

	if (vx != 0)
		isHolded = 0;

	if (isHolded)
		vy = 0;


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != KOOPAS_STATE_DIE_DEFLECT_OUT)
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


		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0 )
				{
					goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
				}
				else
				{
					vx = -vx;
					goomba->vx = -goomba->vx;
				}
			}
			else if (dynamic_cast<CFlyGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
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
			else if (dynamic_cast<CPlant*>(e->obj))					// obj is plant
			{
				CPlant* plant = dynamic_cast<CPlant*>(e->obj);
				plant->isFinish = 1;
			}
			else if (dynamic_cast<CPiranhaPlant*>(e->obj))			 // obj is PiranhaPlant
			{
				CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
				plant->isFinish = 1;
			}
			else if (dynamic_cast<CKoopas*>(e->obj))	// if e->obj is koopas
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(KOOPAS_STATE_DIE_DEFLECT_OUT);
					//koopas->vx = 0.05f * this->nx;
				}
				else if (state == KOOPAS_STATE_WALKING)
				{
					vx = -vx;
					koopas->vx = -koopas->vx;
				}
			}
			else if (dynamic_cast<CFlyKoopas*>(e->obj))	// if e->obj is koopas
			{
				CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
					//koopas->vx = 0.05f * this->nx;
				}
				else if (state == KOOPAS_STATE_WALKING)
				{
					vx = -vx;
					koopas->vx = -koopas->vx;
				}
			}
			else if (dynamic_cast<CUpsideBrick*>(e->obj))	// if e->obj is UpsideBrick 
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);
				if (state == KOOPAS_STATE_WALKING)
				{
					if (e->ny > 0 || this->y + KOOPAS_BBOX_HEIGHT > Upsidebrick->y)
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
					if (e->ny > 0 || this->y + KOOPAS_BBOX_HEIGHT_DIE > Upsidebrick->y)
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
				if (abs(nx) > 0.0001f)
					vx = -vx;
				CQBrick* qbrick = dynamic_cast<CQBrick*>(e->obj);
				if (e->nx != 0 &&((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0))
				{
					if (qbrick->GetState() != BRICK_STATE_EMP)
					{
						qbrick->SetState(BRICK_STATE_EMP);
						qbrick->trigger = 1;
						qbrick->StartRinging();
					}
				}
			}
			else if (dynamic_cast<CBrokenBrick*>(e->obj))		//question brick
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
				CBrokenBrick* bbrick = dynamic_cast<CBrokenBrick*>(e->obj);
				if (e->nx != 0 && ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0))
				{
					if (bbrick->GetState() == BROKENBRICK_STATE_BRICK)
					{
						bbrick->trigger = 1;
						bbrick->isFinish = 1;
					}
				}

			}
			else 
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (state == KOOPAS_STATE_WALKING)
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



void CKoopas::Render()
{
	int ani = KOOPAS_ANI_WALKING_LEFT;
	if (state == KOOPAS_STATE_DIE && vx == 0)
		ani = KOOPAS_ANI_DIE;
	else if ((state == KOOPAS_STATE_DIE_DEFLECT && vx == 0) || state == KOOPAS_STATE_DIE_DEFLECT_OUT)
		ani = KOOPAS_ANI_DIE_DEFLECT;
	else if (state == KOOPAS_STATE_DIE && vx > 0)
		ani = KOOPAS_ANI_SPIN_RIGHT;
	else if (state == KOOPAS_STATE_DIE && vx < 0)
		ani = KOOPAS_ANI_SPIN_LEFT;
	else if (state == KOOPAS_STATE_DIE_DEFLECT && vx > 0)
		ani = KOOPAS_ANI_SPIN_RIGHT_DEFLECT;
	else if (state == KOOPAS_STATE_DIE_DEFLECT && vx < 0)
		ani = KOOPAS_ANI_SPIN_LEFT_DEFLECT;
	else if (vx > 0) ani = KOOPAS_ANI_WALKING_RIGHT;
	else if (vx <= 0) ani = KOOPAS_ANI_WALKING_LEFT;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
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
	case KOOPAS_STATE_DIE_DEFLECT:
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		vx = 0;
		break;
	case KOOPAS_STATE_DIE_DEFLECT_OUT:
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		isFinish = 1;
		break;
	case KOOPAS_STATE_WALKING:
		vx = -KOOPAS_WALKING_SPEED;
		break;
	}

}
 