#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"



CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float scrh = float(game->GetScreenHeight());
	if (this->y < scrh - 20.0f  && this->y > scrh - 52.0f)	// mario out of map then die
	{
		SetState(MARIO_STATE_DIE);	
		return;
	}

	// Calculate dx, dy 
	CGameObject::Update(dt, coObjects);

	if (GetTickCount64() - switch_start > MARIO_SWITCHING_TIME)
	{
		switch_start = 0;
		switching = 0;
		if (teleport != 0)
		{
			SetPosition(toX, toY);
			ResetState();
			teleport = 0;
		}

	}
	if (switching)
	{
		if (switchType == 0)
			y += 0.5f;
		else
			y -= 0.5f;
	}
	else
	{
		if (vy > 0.0f) isFlying = 1; // if falling then cant jump

		// Simple fall down
		vy += MARIO_GRAVITY * dt;

		if (falling)					// racoon falling 
			vy = MARIO_RACOON_FALL_VY;

		if (flying)						// racoon flying
			vy = MARIO_RACOON_FLY_VY;

		if (canHold == 0 && obj != NULL)	// if Mario release Object
		{
			holding = 0;
			if (this->nx > 0)
				obj->vx = KOOPAS_SPIN_SPEED;
			else
				obj->vx = -KOOPAS_SPIN_SPEED;
			StartKicking();
			obj = NULL;
		}


		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();

		// turn off collision when die 
		if (state != MARIO_STATE_DIE)
			CalcPotentialCollisions(coObjects, coEvents);

		// reset untouchable timer if untouchable time has passed
		if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
		{
			untouchable_start = 0;
			untouchable = 0;
		}

		if (GetTickCount64() - fall_start > MARIO_FALLING_TIME)		//racoon falling time
		{
			fall_start = 0;
			falling = 0;
		}

		if (GetTickCount64() - fly_start > MARIO_FLYING_TIME)			//racoon flying time
		{
			fly_start = 0;
			flying = 0;
		}

		if (GetTickCount64() - turn_start > MARIO_TURNING_TIME)		// mario turning time
		{
			turn_start = 0;
			turning = 0;
		}

		if (GetTickCount64() - tail_start > MARIO_TAILING_TIME)		// mario tail attack time
		{
			tail_start = 0;
			tailing = 0;
		}

		if (GetTickCount64() - kick_start > MARIO_KICKING_TIME)		// mario kick object time
		{
			kick_start = 0;
			kicking = 0;
		}

		if (GetTickCount64() - slide_start > MARIO_SLIDING_TIME)	// mario slide time countdown
		{
			slide_start = 0;
			if (canSlide == 1)
				sliding = 1;
		}

		if (GetTickCount64() - throw_start > MARIO_THROWING_TIME)	// fire mario throw fireball time
		{
			throw_start = 0;
			throwing = 0;
		}

		if (GetTickCount64() - trans_start > MARIO_TRANSFORM_TIME)	// mario transfomr to other form time
		{
			trans_start = 0;
			transform = 0;
		}


		if (vx == 0)
		{
			sliding = 0; canSlide = 0;
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

			//if (rdx != 0 && rdx != dx)
			//	x += nx * abs(rdx);

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
					if (e->ny > 0)
					{
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;
					}
					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							goomba->SetState(GOOMBA_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								if (level == MARIO_LEVEL_RACOON && tailing == 1)
								{
									goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
									goomba->vx = 0.05f * this->nx;
								}
								else if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									ResetState();
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									ResetState();
									CMario::ToSmall(this->y);
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
						}
					}
				}
				if (dynamic_cast<CFlyGoomba*>(e->obj)) // if e->obj is Goomba 
				{
					CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
					if (e->ny > 0)
					{
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;
					}
					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (goomba->GetState() == FLYGOOMBA_STATE_FLYING)
						{
							goomba->SetState(FLYGOOMBA_STATE_WALKING);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
						else if (goomba->GetState() == FLYGOOMBA_STATE_WALKING)
						{
							goomba->SetState(FLYGOOMBA_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (goomba->GetState() != FLYGOOMBA_STATE_DIE)
							{
								if (level == MARIO_LEVEL_RACOON && tailing == 1)
								{
									goomba->SetState(FLYGOOMBA_STATE_DIE_DEFLECT);
									goomba->vx = 0.05f * this->nx;
								}
								else if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									ResetState();
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									ResetState();
									CMario::ToSmall(this->y);
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
					if (e->ny > 0)
					{
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;
					}
					// jump on top >> kill Koopas and deflect a bit 
					if (e->ny < 0)
					{
						if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx == 0) {
							if (this->nx > 0)  // direction of koopas spin when being stomped 
								koopas->vx = KOOPAS_SPIN_SPEED;
							else
								koopas->vx = -KOOPAS_SPIN_SPEED;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
						else if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx != 0) {
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							koopas->vx = 0;		// being stomped when spining then still
						}
						else
						{
							koopas->SetState(KOOPAS_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (koopas->vx != 0)
							{
								if (level == MARIO_LEVEL_RACOON && tailing == 1)
									koopas->SetState(KOOPAS_STATE_DIE_DEFLECT);
								else if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									ResetState();
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									ResetState();
									CMario::ToSmall(this->y);
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
							else {
								if (canHold == 0)								// Kicking an object
								{
									if (this->nx > 0)
										koopas->vx = KOOPAS_SPIN_SPEED;
									else
										koopas->vx = -KOOPAS_SPIN_SPEED;
									StartKicking();
								}
								else
								{
									this->SetState(MARIO_STATE_HOLD);				// Holding an object
									obj = koopas;
									koopas->isHolded = 1;
								}

							}
						}
					}
				}  // if Koopas
				else if (dynamic_cast<CFlyKoopas*>(e->obj)) // if e->obj is Koopas 
				{
					CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
					if (e->ny > 0)
					{
						vy = temp;							
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;
					}
					// jump on top >> kill Koopas and deflect a bit 

					if (e->ny < 0)
					{
						if (koopas->GetState() == FLYKOOPAS_STATE_FLYING)
						{
							koopas->SetState(FLYKOOPAS_STATE_WALKING);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
						else if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx == 0) {
							if (this->nx > 0)  // direction of koopas spin when being stomped 
								koopas->vx = KOOPAS_SPIN_SPEED;
							else
								koopas->vx = -KOOPAS_SPIN_SPEED;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
						else if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx != 0) {
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							koopas->vx = 0;		// being stomped when spining then still
						}
						else
						{
							koopas->SetState(KOOPAS_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (koopas->vx != 0)
							{
								if (level == MARIO_LEVEL_RACOON && tailing == 1)
									koopas->SetState(KOOPAS_STATE_DIE_DEFLECT);
								else if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									ResetState();
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									ResetState();
									CMario::ToSmall(this->y);
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
							else {
								if (canHold == 0)								// Kicking an object
								{
									if (this->nx > 0)
										koopas->vx = KOOPAS_SPIN_SPEED;
									else
										koopas->vx = -KOOPAS_SPIN_SPEED;
									StartKicking();
								}
								else
								{
									this->SetState(MARIO_STATE_HOLD);				// Holding an object
									obj = koopas;
									koopas->isHolded = 1;
								}

							}
						}
					}
				}  // if Koopas
				else if (dynamic_cast<CPlant*>(e->obj))
				{
					CPlant* plant = dynamic_cast<CPlant*>(e->obj);
					if (tailing)
					{
						plant->isFinish = 1;
					}
					else if (untouchable == 0) {
						if (!plant->isUnderPipe)
						{
							if (level > MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_BIG;
								ResetState();
								StartUntouchable();
							}
							else if (level == MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_SMALL;
								ResetState();
								CMario::ToSmall(this->y);
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}  // if Plant
				else if (dynamic_cast<CPlantFireBall*>(e->obj))
				{
					CPlantFireBall* fireball = dynamic_cast<CPlantFireBall*>(e->obj);
					if (e->ny > 0)
					{
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;
					}
					if (untouchable == 0) {
						if (level > MARIO_LEVEL_BIG)
						{
							level = MARIO_LEVEL_BIG;
							ResetState();
							StartUntouchable();
						}
						else if (level == MARIO_LEVEL_BIG)
						{
							level = MARIO_LEVEL_SMALL;
							ResetState();
							CMario::ToSmall(this->y);
							StartUntouchable();
						}
						else
							SetState(MARIO_STATE_DIE);
					}
				}  // if Plant
				else if (dynamic_cast<CPiranhaPlant*>(e->obj))
				{
					CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
					if (tailing)
					{
						plant->isFinish = 1;
					}
					else if (untouchable == 0) {
						if (!plant->isUnderPipe)
						{
							if (level > MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_BIG;
								ResetState();
								StartUntouchable();
							}
							else if (level == MARIO_LEVEL_BIG)
							{
								level = MARIO_LEVEL_SMALL;
								ResetState();
								CMario::ToSmall(this->y);
								StartUntouchable();

							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				} // if Plant
				else if (dynamic_cast<CBrick*>(e->obj))
				{
					CBrick* brick = dynamic_cast<CBrick*>(e->obj);
					if (e->ny < 0) // jump on top brick then can jumping again
					{
						isFlying = 0;
						falling = 0;		//	racoon mario cant fall slowly
					}
				}
				else if (dynamic_cast<CSwitch*>(e->obj))
				{
					CSwitch* sw = dynamic_cast<CSwitch*>(e->obj);
					if (e->ny < 0)
					{
						if (KeyDownPressed)
						{
							StartSwitching(sw->toX, sw->toY);
							teleport = 1;
							switchType = 0;
						}
					}
					else if (e->ny > 0)
					{
						if (KeyUpPressed)
						{
							StartSwitching(sw->toX, sw->toY);
							teleport = 1;
							switchType = 1;
						}
					}

				}
				else if (dynamic_cast<CMushRoom*>(e->obj))
				{
					CMushRoom* mushroom = dynamic_cast<CMushRoom*>(e->obj);
					mushroom->isFinish = 1;
					if (level < MARIO_LEVEL_BIG)
					{
						CMario::ToBig(y);
						level = MARIO_LEVEL_BIG;
						StartTransform();
					}
					vy = temp;							//Mario went through the mushroom
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
				}
				else if (dynamic_cast<CLeaf*>(e->obj))
				{
					CLeaf* leaf = dynamic_cast<CLeaf*>(e->obj);
					leaf->isFinish = 1;
					if (level == MARIO_LEVEL_SMALL)
						CMario::ToBig(y);
					level = MARIO_LEVEL_RACOON;
					vy = temp;							//Mario went through the mushroom
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
				}
				else if (dynamic_cast<CQBrick*>(e->obj))		//question brick
				{
					CQBrick* qbrick = dynamic_cast<CQBrick*>(e->obj);
					if (e->ny < 0) // jump on top brick then can jumping again
					{
						isFlying = 0;
						falling = 0;		//	racoon mario cant fall slowly
					}
					else if (e->ny > 0)
					{
						if (qbrick->GetState() != BRICK_STATE_EMP)
						{
							qbrick->StartRinging();
							qbrick->trigger = 1;
							qbrick->SetState(BRICK_STATE_EMP);
						}
					}
					else if (e->nx != 0 && tailing)
					{
						if (qbrick->GetState() != BRICK_STATE_EMP)
						{
							qbrick->StartRinging();
							qbrick->trigger = 1;
							qbrick->SetState(BRICK_STATE_EMP);
						}
					}
				}
				else if (dynamic_cast<CBrokenBrick*>(e->obj))		//Broken brick
				{
					CBrokenBrick* bbrick = dynamic_cast<CBrokenBrick*>(e->obj);
					if (bbrick->GetState() == BROKENBRICK_STATE_BRICK)
					{
						if (e->ny < 0) // jump on top brick then can jumping again
						{
							isFlying = 0;
							falling = 0;		//	racoon mario cant fall slowly
						}
						else if (e->ny > 0)
						{
							bbrick->trigger = 1;
							bbrick->isFinish = 1;
						}
						else if (e->nx != 0 && tailing)
						{
							bbrick->trigger = 1;
							bbrick->isFinish = 1;
						}
					}
					else
					{
						bbrick->isFinish = 1;					// Make coin disappear
						vy = temp;							//Mario went through the coin
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;

						if (flying)
						{
							x += dx;
							y += dy;
						}
					}

				}
				else if (dynamic_cast<CPButton*>(e->obj))		//P Button
				{
					CPButton* button = dynamic_cast<CPButton*>(e->obj);
					if (e->ny < 0) // jump on top button then can jumping again
					{
						isFlying = 0;
						falling = 0;		//	racoon mario cant fall slowly
						if (button->GetState() == BUTTON_STATE_BUTTON)
						{
							button->trigger = 1;
							button->SetState(BUTTON_STATE_STOMPED);
						}
					}
					else if (e->ny > 0)
					{
						if (button->GetState() == BUTTON_STATE_BRICK)
							button->SetState(BUTTON_STATE_BUTTON);
					}
				}
				else if (dynamic_cast<CUpsideBrick*>(e->obj))
				{
					CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);		// Upside brick

					if (this->level >= MARIO_LEVEL_BIG)
					{
						if (this->state == MARIO_STATE_DUCK)
						{
							if (e->ny > 0 || this->y + MARIO_BIG_DUCK_BBOX_HEIGHT > Upsidebrick->y)
							{
								vy = temp;							//If wrong side then go through
								x -= min_tx * dx + nx * 0.4f;
								y -= min_ty * dy + ny * 0.4f;
								x += dx;
								y += dy;
							}
							else {
								isFlying = 0; // jump on top brick then can jumping again
							}
						}
						else if (this->state != MARIO_STATE_DUCK)
						{
							if (e->ny > 0 || this->y + MARIO_BIG_BBOX_HEIGHT > Upsidebrick->y)
							{
								vy = temp;							//If wrong side then go through
								x -= min_tx * dx + nx * 0.4f;
								y -= min_ty * dy + ny * 0.4f;
								x += dx;
								y += dy;
							}
							else {
								falling = 0;	//	racoon mario cant fall slowly
								isFlying = 0;	 // jump on top brick then can jumping again
							}
						}
					}
					else if (this->level >= MARIO_LEVEL_SMALL)
					{
						if (e->ny > 0 || this->y + MARIO_SMALL_BBOX_HEIGHT > Upsidebrick->y)
						{
							vy = temp;							//If wrong side then go through
							x -= min_tx * dx + nx * 0.4f;
							y -= min_ty * dy + ny * 0.4f;
							x += dx;
							y += dy;
						}
						else {
							isFlying = 0;	 // jump on top brick then can jumping again
						}
					}


				}
				else if (dynamic_cast<CCoin*>(e->obj)) // if e->obj is Coin 
				{
					CCoin* coin = dynamic_cast<CCoin*>(e->obj);
					coin->isFinish = 1;					// Make coin disappear
					vy = temp;							//Mario went through the coin
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
					x += dx;
					if (flying)
					{
						x += dx;
						y += dy;
					}
				}
				else if (dynamic_cast<CPortal*>(e->obj))
				{
					CPortal* p = dynamic_cast<CPortal*>(e->obj);
					CGame::GetInstance()->SwitchScene(p->GetSceneId());
				}
			}

		}
		if (obj != NULL)	// set position of obj when being holded
		{
			if (nx > 0)
			{
				if (level == MARIO_LEVEL_SMALL)
				{
					obj->x = x + 12;
					obj->y = y - 4;
				}
				else if (level == MARIO_LEVEL_RACOON)
				{
					obj->x = x + 19;
					obj->y = y + 7;
				}
				else
				{
					obj->x = x + 12;
					obj->y = y + 7;
				}
			}
			else
			{
				if (level == MARIO_LEVEL_SMALL)
				{
					obj->x = x - 13;
					obj->y = y - 4;
				}
				else if (level == MARIO_LEVEL_RACOON)
				{
					obj->x = x - 14;
					obj->y = y + 7;
				}
				else
				{
					obj->x = x - 13;
					obj->y = y + 7;
				}

			}
		}


		// clean up collision events
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

		if (vx > 0 && x > 2812) x = 2812;
		if (vx < 0 && x < 3) x = 3;


		//DebugOut(L"\tv = %f\n", vy);
	}
}

void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else if (falling)
	{
		if (nx > 0)
			ani = MARIO_ANI_RACOON_FALL_RIGHT_1;
		else
			ani = MARIO_ANI_RACOON_FALL_LEFT_1;
	}
	else if (flying)
	{
		if (nx > 0)
			ani = MARIO_ANI_RACOON_FLYING_RIGHT;
		else
			ani = MARIO_ANI_RACOON_FLYING_LEFT;
	}
	else if (turning && isFlying == 0 && holding == 0)
	{
		if (nx > 0)
		{
			if (level == MARIO_LEVEL_BIG)
				ani = MARIO_ANI_BIG_TURNING_RIGHT;
			else if (level == MARIO_LEVEL_SMALL)
				ani = MARIO_ANI_SMALL_TURNING_RIGHT;
			else if (level == MARIO_LEVEL_FIRE)
				ani = MARIO_ANI_FIRE_TURNING_RIGHT;
			else
				ani = MARIO_ANI_RACOON_TURNING_RIGHT;
		}
		else
		{
			if (level == MARIO_LEVEL_BIG)
				ani = MARIO_ANI_BIG_TURNING_LEFT;
			else if (level == MARIO_LEVEL_SMALL)
				ani = MARIO_ANI_SMALL_TURNING_LEFT;
			else if (level == MARIO_LEVEL_FIRE)
				ani = MARIO_ANI_FIRE_TURNING_LEFT;
			else
				ani = MARIO_ANI_RACOON_TURNING_LEFT;
		}
	}
	else if (tailing)
	{
		if (nx > 0)
			ani = MARIO_ANI_RACOON_TAIL_RIGHT;
		else
			ani = MARIO_ANI_RACOON_TAIL_LEFT;
	}
	else if (transform)
	{
		if (nx > 0)
			ani = MARIO_ANI_TRANSFORM_RIGHT;
		else
			ani = MARIO_ANI_TRANSFORM_LEFT;
	}
	else if (throwing)
	{
		if (nx > 0)
		{
			if (isFlying)
				ani = MARIO_ANI_FIRE_THROW2_RIGHT;
			else 
				ani = MARIO_ANI_FIRE_THROW_RIGHT;
		}
		else
		{
			if (isFlying)
				ani = MARIO_ANI_FIRE_THROW2_LEFT;
			else
				ani = MARIO_ANI_FIRE_THROW_LEFT;
		}
	}
	else if (kicking)
	{
		if (nx > 0)
		{
			if (level == MARIO_LEVEL_BIG)
				ani = MARIO_ANI_BIG_KICK_LEFT;
			else if (level == MARIO_LEVEL_SMALL)
				ani = MARIO_ANI_SMALL_KICK_LEFT;
			else if (level == MARIO_LEVEL_FIRE)
				ani = MARIO_ANI_FIRE_KICK_LEFT;
			else
				ani = MARIO_ANI_RACOON_KICK_LEFT;
		}
		else
		{
			if (level == MARIO_LEVEL_BIG)
				ani = MARIO_ANI_BIG_KICK_RIGHT;
			else if (level == MARIO_LEVEL_SMALL)
				ani = MARIO_ANI_SMALL_KICK_RIGHT;
			else if (level == MARIO_LEVEL_FIRE)
				ani = MARIO_ANI_FIRE_KICK_RIGHT;
			else
				ani = MARIO_ANI_RACOON_KICK_RIGHT;
		}
	}
	else if (switching)
	{
		if (level == MARIO_LEVEL_BIG)
			ani = MARIO_ANI_TELE_BIG;
		else if (level == MARIO_LEVEL_SMALL)
			ani = MARIO_ANI_TELE_SMALL;
		else if (level == MARIO_LEVEL_FIRE)
			ani = MARIO_ANI_TELE_FIRE;
		else
			ani = MARIO_ANI_TELE_RACOON;
	}
	else
		if (level == MARIO_LEVEL_BIG)
		{
			if (vx == 0)
			{
				if (nx > 0)
				{
					if (holding)
						ani = MARIO_ANI_BIG_HOLD_STILL_RIGHT;
					else if (isFlying)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_BIG_JUMP_RIGHT;
						else 
							ani = MARIO_ANI_BIG_FALL_RIGHT;
							
					}
					else if (isDucking)
						ani = MARIO_ANI_BIG_DUCK_RIGHT;
					else
						ani = MARIO_ANI_BIG_IDLE_RIGHT;
				}
				else
				{
					if (holding)
						ani = MARIO_ANI_BIG_HOLD_STILL_LEFT;
					else if (isFlying)
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
				if (holding)
					ani = MARIO_ANI_BIG_HOLD_WALK_RIGHT;
				else if (sliding && canSlide)
				{
					if (isFlying)
						ani = MARIO_ANI_BIG_FLY_RIGHT;
					else
						ani = MARIO_ANI_BIG_SLIDE_RIGHT;
				}
				else if (isFlying)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_BIG_JUMP_RIGHT;
					else 
						ani = MARIO_ANI_BIG_FALL_RIGHT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_BIG_DUCK_RIGHT;
				else if (isRunning == 1)
					ani = MARIO_ANI_BIG_RUNNING_RIGHT;
				else
					ani = MARIO_ANI_BIG_WALKING_RIGHT;
			}
			else
			{
				if (holding)
					ani = MARIO_ANI_BIG_HOLD_WALK_LEFT;
				else if (sliding && canSlide)
				{
					if (isFlying)
						ani = MARIO_ANI_BIG_FLY_LEFT;
					else
						ani = MARIO_ANI_BIG_SLIDE_LEFT;
				}

				else if (isFlying)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_BIG_JUMP_LEFT;
					else 
						ani = MARIO_ANI_BIG_FALL_LEFT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_BIG_DUCK_LEFT;
				else if (isRunning == 1)
					ani = MARIO_ANI_BIG_RUNNING_LEFT;
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
					if (holding)
						ani = MARIO_ANI_SMALL_HOLD_STILL_RIGHT;
					else if (isFlying)
						ani = MARIO_ANI_SMALL_JUMP_RIGHT;
					else
						ani = MARIO_ANI_SMALL_IDLE_RIGHT;

				}
				else
				{
					if (holding)
						ani = MARIO_ANI_SMALL_HOLD_STILL_LEFT;
					else if (isFlying)
						ani = MARIO_ANI_SMALL_JUMP_LEFT;
					else
						ani = MARIO_ANI_SMALL_IDLE_LEFT;
				}
			}
			else if (vx > 0)
			{
				if (holding)
					ani = MARIO_ANI_SMALL_HOLD_WALK_RIGHT;
				else if (sliding && canSlide)
				{
					if (isFlying)
						ani = MARIO_ANI_SMALL_FLY_RIGHT;
					else
						ani = MARIO_ANI_SMALL_SLIDE_RIGHT;
				}

				else if (isFlying)
					ani = MARIO_ANI_SMALL_JUMP_RIGHT;
				else if (isRunning == 1)
					ani = MARIO_ANI_SMALL_RUNNING_RIGHT;
				else
					ani = MARIO_ANI_SMALL_WALKING_RIGHT;
			}
			else
			{
				if (holding)
					ani = MARIO_ANI_SMALL_HOLD_WALK_LEFT;
				else if (sliding && canSlide)
				{
					if (isFlying)
						ani = MARIO_ANI_SMALL_FLY_LEFT;
					else
						ani = MARIO_ANI_SMALL_SLIDE_LEFT;
				}

				else if (isFlying)
					ani = MARIO_ANI_SMALL_JUMP_LEFT;
				else if (isRunning == 1)
					ani = MARIO_ANI_SMALL_RUNNING_LEFT;
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
					if (holding)
						ani = MARIO_ANI_FIRE_HOLD_STILL_RIGHT;
					else if (isFlying)
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
					if (holding)
						ani = MARIO_ANI_FIRE_HOLD_STILL_LEFT;
					else if (isFlying)
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

				if (holding)
					ani = MARIO_ANI_FIRE_HOLD_WALK_RIGHT;
				else if (sliding && canSlide)
				{
					if (isFlying)
						ani = MARIO_ANI_FIRE_FLY_RIGHT;
					else
						ani = MARIO_ANI_FIRE_SLIDE_RIGHT;
				}
				else if (isFlying)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_FIRE_JUMP_RIGHT;
					else
						ani = MARIO_ANI_FIRE_FALL_RIGHT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_FIRE_DUCK_RIGHT;
				else if (isRunning == 1)
					ani = MARIO_ANI_FIRE_RUNNING_RIGHT;
				else
					ani = MARIO_ANI_FIRE_WALKING_RIGHT;
			}
			else
			{
				if (holding)
					ani = MARIO_ANI_FIRE_HOLD_WALK_LEFT;
				else if (sliding && canSlide)
				{
					if (isFlying)
						ani = MARIO_ANI_FIRE_FLY_LEFT;
					else
						ani = MARIO_ANI_FIRE_SLIDE_LEFT;
				}
				else if (isFlying)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_FIRE_JUMP_LEFT;
					else
						ani = MARIO_ANI_FIRE_FALL_LEFT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_FIRE_DUCK_LEFT;
				else if (isRunning == 1)
					ani = MARIO_ANI_FIRE_RUNNING_LEFT;
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
					if (holding)
						ani = MARIO_ANI_RACOON_HOLD_STILL_RIGHT;
					else if (isFlying)
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
					if (holding)
						ani = MARIO_ANI_RACOON_HOLD_STILL_LEFT;
					else if (isFlying)
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
				if (holding)
					ani = MARIO_ANI_RACOON_HOLD_WALK_RIGHT;
				else if (sliding && canSlide)
				{
					if (isFlying)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_RACOON_FLY_RIGHT;
						else
							ani = MARIO_ANI_RACOON_FLY_RIGHT1;
					}
					else
						ani = MARIO_ANI_RACOON_SLIDE_RIGHT;
				}
				else if (isFlying)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_RACOON_JUMP_RIGHT;
					else
						ani = MARIO_ANI_RACOON_FALL_RIGHT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_RACOON_DUCK_RIGHT;
				else if (isRunning == 1)
					ani = MARIO_ANI_RACOON_RUNNING_RIGHT;
				else
					ani = MARIO_ANI_RACOON_WALKING_RIGHT;
			}
			else
			{
				if (holding)
					ani = MARIO_ANI_RACOON_HOLD_WALK_LEFT;
				else if (sliding && canSlide)
				{
					if (isFlying)
					{
						if (vy < 0.0)
							ani = MARIO_ANI_RACOON_FLY_LEFT;
						else
							ani = MARIO_ANI_RACOON_FLY_LEFT1;
					}
					else
						ani = MARIO_ANI_RACOON_SLIDE_LEFT;
				}
				else if (isFlying)
				{
					if (vy < 0.0)
						ani = MARIO_ANI_RACOON_JUMP_LEFT;
					else
						ani = MARIO_ANI_RACOON_FALL_LEFT;
				}
				else if (isDucking == 1)
					ani = MARIO_ANI_RACOON_DUCK_LEFT;
				else if (isRunning == 1)
					ani = MARIO_ANI_RACOON_RUNNING_LEFT;
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
			if (turning)
				vx = MARIO_WALKING_SPEED / 2;
			else if (isRunning == 0)
				vx = MARIO_WALKING_SPEED;

			else
				vx = MARIO_RUNNING_SPEED;
		}
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isDucking == 0)
		{
			if (turning)
				vx = -MARIO_WALKING_SPEED / 2;
			else if (isRunning == 0)
				vx = -MARIO_WALKING_SPEED;
			else
				vx = -MARIO_RUNNING_SPEED;
		}
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (isFlying == 0 && isDucking == 0)
		{
			isFlying = 1;
			if (sliding && canSlide)
				vy = -(MARIO_JUMP_SPEED_Y + 0.08f);		//vy when slide
			else
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
	case MARIO_STATE_HOLD:
		holding = 1;
		break;
	case MARIO_STATE_SLIDE:
		StartSliding();
		canSlide = 1;
		break;
	case MARIO_FIRE_STATE_THROW:
		StartThrowing();
		this->fireball += 1;	//Stack fireball
		break;
	case MARIO_RACOON_STATE_FALL:
		StartFalling();
		break;
	case MARIO_RACOON_STATE_FLY:
		StartFlying();
		break;
	case MARIO_RACOON_STATE_TURN:
		StartTurning();
		break;
	case MARIO_RACOON_STATE_TAIL:
		StartTailing();
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		vx = 0;
		break;
	}
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_FIRE)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		if (isDucking == 1)
			bottom = y + MARIO_BIG_DUCK_BBOX_HEIGHT;
		else
			bottom = y + MARIO_BIG_BBOX_HEIGHT;	  
	}
	else if (level == MARIO_LEVEL_RACOON)
	{
		if (this->nx > 0)
		{
			left = x + MARIO_RACOON_BBOX_WIDTH - MARIO_BIG_BBOX_WIDTH;
			top = y;
			right = left + MARIO_BIG_BBOX_WIDTH;
		}
		else {
			left = x;
			top = y;
			right = x + MARIO_BIG_BBOX_WIDTH;
		}

		if (isDucking == 1)
			bottom = y + MARIO_BIG_DUCK_BBOX_HEIGHT;
		else
			bottom = y + MARIO_RACOON_BBOX_HEIGHT;

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

CGameObject* CMario::NewFireBall()		// create fireball function
{
	int ani_set_id = MARIO_FIREBALL_ANI_SET_ID;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;
	obj = new CFireBall(this->nx);
	obj->SetPosition(this->x + MARIO_BIG_BBOX_WIDTH/2, this->y + MARIO_BIG_BBOX_HEIGHT/3);
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);
	return obj;
}

