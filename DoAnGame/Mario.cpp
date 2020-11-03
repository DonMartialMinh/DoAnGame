#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Koopas.h"
#include "Portal.h"
#include "Brick.h"
#include "UpsideBrick.h"
#include "Coin.h"
#include "Environment.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_BIG;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy += MARIO_GRAVITY * dt;
	if (vy > 0.04f) isFlying = 1; // if falling then cant jump

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
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

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?

		/*if (rdx != 0 && rdx != dx)
			x += nx * abs(rdx);*/

		// block every object first!
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		float temp = vy;
		//if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			}if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Koopas 
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);

				// jump on top >> kill Koopas and deflect a bit 
				if (e->ny < 0)
				{
					if (koopas->GetState() != KOOPAS_STATE_DIE)
					{
						koopas->SetState(KOOPAS_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
					else if (koopas->GetState() == KOOPAS_STATE_DIE && koopas->vx == 0 ) {
						if (this->nx > 0)  // direction of koopas spin when being stomped
							koopas->vx = 0.2f;
						else
							koopas->vx = -0.2f;
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
					else if (koopas->GetState() == KOOPAS_STATE_DIE && koopas->vx != 0) {
						vy = -MARIO_JUMP_DEFLECT_SPEED;
						koopas->vx = 0;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (koopas->GetState() != KOOPAS_STATE_DIE || ((koopas->GetState() == KOOPAS_STATE_DIE )&& koopas->vx != 0))
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
						else{
							if (e->nx < 0) 
								koopas->vx = 0.2f;
							else
								koopas->vx = -0.2f;
						}
					}
				}
			}  // if Koopas
			else if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				if (e->ny < 0) // jump on top brick then can jumping again
				{
					isFlying = 0;
				}
				else if (e->nx != 0  && e->ny == 0)
				{
					/*float magnitude = sqrt((vx * vx + vy * vy)) * (1 - min_tx) ;
					float dotprod = vx * ny + vy * nx;
					if (dotprod > 0.0f)
						dotprod = 1.0f;
					else if (dotprod < 0.0f)
						dotprod = -1.0f;
					vx = dotprod * ny * magnitude;
					vy = dotprod * nx * magnitude;*/
					// slide
					/*float dotprod = (vx * ny + vy * nx) * (1- min_tx);
					vx = dotprod * ny;
					vy = dotprod * nx;*/
				}
			}
			else if (dynamic_cast<CUpsideBrick*>(e->obj))
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);

				if (e->ny >= 0) // jump on top brick then can jumping again
				{
					vy = temp;							//If wrong side then go through
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
					x += dx;
					y += dy;
				}
				else {
					isFlying = 0;
				}
			}
			else if (dynamic_cast<CCoin*>(e->obj)) // if e->obj is Coin 
			{
				vy = temp;							//Mario went through the coin
				x -= min_tx * dx + nx * 0.4f;
				y -= min_ty * dy + ny * 0.4f;
				x += dx;
				y += dy;
			}
			else if (dynamic_cast<CPortal*>(e->obj))
			{
				CPortal* p = dynamic_cast<CPortal*>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}

	}



	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (vx > 0 && x > 2812) x = 2812;
	if (vx < 0 && x < 3) x = 3;


	DebugOut(L"\nvx = %f", vx);
	DebugOut(L"\tvy = %f\n", vy);
}

void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
		if (level == MARIO_LEVEL_BIG)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (isFlying == 1)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_BIG_JUMP_RIGHT;
						else 
							ani = MARIO_ANI_BIG_FALL_RIGHT;
							
					}
					else if (isDucking == 1)
						ani = MARIO_ANI_BIG_DUCK_RIGHT;
					else
						ani = MARIO_ANI_BIG_IDLE_RIGHT;
				}
				else
				{
					if (isFlying == 1)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_BIG_JUMP_LEFT;
						else 
							ani = MARIO_ANI_BIG_FALL_LEFT;
					}
					else if (isDucking == 1)
						ani = MARIO_ANI_BIG_DUCK_LEFT;
					else
						ani = MARIO_ANI_BIG_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (isFlying == 1)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_BIG_JUMP_RIGHT;
					else 
						ani = MARIO_ANI_BIG_FALL_RIGHT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_BIG_DUCK_RIGHT;
				else
					ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}
			else
			{
				if (isFlying == 1)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_BIG_JUMP_LEFT;
					else 
						ani = MARIO_ANI_BIG_FALL_LEFT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_BIG_DUCK_LEFT;
				else
					ani = MARIO_ANI_BIG_WALKING_LEFT;
			}
		}
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (isFlying == 1)
						ani = MARIO_ANI_SMALL_JUMP_RIGHT;
					else
						ani = MARIO_ANI_SMALL_IDLE_RIGHT;

				}
				else
				{
					if (isFlying == 1)
						ani = MARIO_ANI_SMALL_JUMP_LEFT;
					else
						ani = MARIO_ANI_SMALL_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (isFlying == 1)
					ani = MARIO_ANI_SMALL_JUMP_RIGHT;
				else
					ani = MARIO_ANI_SMALL_WALKING_RIGHT;
			}
			else
			{
				if (isFlying == 1)
					ani = MARIO_ANI_SMALL_JUMP_LEFT;
				else
					ani = MARIO_ANI_SMALL_WALKING_LEFT;
			}
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (isFlying == 1)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_FIRE_JUMP_RIGHT;
						else
							ani = MARIO_ANI_FIRE_FALL_RIGHT;

					}
					else if (isDucking == 1)
						ani = MARIO_ANI_FIRE_DUCK_RIGHT;
					else
						ani = MARIO_ANI_FIRE_IDLE_RIGHT;
				}
				else
				{
					if (isFlying == 1)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_FIRE_JUMP_LEFT;
						else
							ani = MARIO_ANI_FIRE_FALL_LEFT;
					}
					else if (isDucking == 1)
						ani = MARIO_ANI_FIRE_DUCK_LEFT;
					else
						ani = MARIO_ANI_FIRE_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (isFlying == 1)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_FIRE_JUMP_RIGHT;
					else
						ani = MARIO_ANI_FIRE_FALL_RIGHT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_FIRE_DUCK_RIGHT;
				else
					ani = MARIO_ANI_FIRE_WALKING_RIGHT;
			}
			else
			{
				if (isFlying == 1)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_FIRE_JUMP_LEFT;
					else
						ani = MARIO_ANI_FIRE_FALL_LEFT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_FIRE_DUCK_LEFT;
				else
					ani = MARIO_ANI_FIRE_WALKING_LEFT;
			}
		}
		else if (level == MARIO_LEVEL_RACOON)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (isFlying == 1)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_RACOON_JUMP_RIGHT;
						else
							ani = MARIO_ANI_RACOON_FALL_RIGHT;

					}
					else if (isDucking == 1)
						ani = MARIO_ANI_RACOON_DUCK_RIGHT;
					else
						ani = MARIO_ANI_RACOON_IDLE_RIGHT;
				}
				else
				{
					if (isFlying == 1)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_RACOON_JUMP_LEFT;
						else
							ani = MARIO_ANI_RACOON_FALL_LEFT;
					}
					else if (isDucking == 1)
						ani = MARIO_ANI_RACOON_DUCK_LEFT;
					else
						ani = MARIO_ANI_RACOON_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (isFlying == 1)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_RACOON_JUMP_RIGHT;
					else
						ani = MARIO_ANI_RACOON_FALL_RIGHT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_RACOON_DUCK_RIGHT;
				else
					ani = MARIO_ANI_RACOON_WALKING_RIGHT;
			}
			else
			{
				if (isFlying == 1)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_RACOON_JUMP_LEFT;
					else
						ani = MARIO_ANI_RACOON_FALL_LEFT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_RACOON_DUCK_LEFT;
				else
					ani = MARIO_ANI_RACOON_WALKING_LEFT;
			}
		}

	int alpha = 255;
	if (untouchable) alpha = 128;
	animation_set->at(ani)->Render(x, y, alpha);

	RenderBoundingBox();
}

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (isDucking == 0)
		{
			vx = MARIO_WALKING_SPEED;
		}
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isDucking == 0)
		{
			vx = -MARIO_WALKING_SPEED;
		}
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		if (isFlying == 0 && isDucking == 0)
		{
			isFlying = 1;
			vy = -MARIO_JUMP_SPEED_Y;
		}
		break;
	case MARIO_STATE_DUCK:
		if (isFlying == 0 && isDucking == 0)
		{
			isDucking = 1;
			y += MARIO_BIG_BBOX_HEIGHT - MARIO_BIG_DUCK_BBOX_HEIGHT;
			vx = 0;
		}
		break;
	case MARIO_STATE_IDLE:
		vx = 0;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	}
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_FIRE)
	{
		if (isDucking == 1)
			bottom = y + MARIO_BIG_DUCK_BBOX_HEIGHT;
		else
			bottom = y + MARIO_BIG_BBOX_HEIGHT;

		right = x + MARIO_BIG_BBOX_WIDTH;
		
	}
	else if (level == MARIO_LEVEL_RACOON)
	{
		left = x;
		top = y;
		if (isDucking == 1)
			bottom = y + MARIO_BIG_DUCK_BBOX_HEIGHT;
		else
			bottom = y + MARIO_RACOON_BBOX_HEIGHT;
		right = x + MARIO_RACOON_BBOX_WIDTH;
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

int CMario::getLevel()
{
	return this->level;
}

