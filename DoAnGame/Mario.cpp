#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#define OBJECT_TYPE_FIREBALL			8


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
		CGame::GetInstance()->SwitchScene(1);
		game->SetTime(0);
		game->SubLive();
		return;
	}

	AnimationTime();	// Time for all animation of mario

	if (transform || transformRacoon)
		return;



	// Calculate dx, dy 

	CGameObject::Update(dt, coObjects);
	vy += MARIO_GRAVITY * dt;

	if (vy > 0.04f) isFlying = 1; // if falling then cant jump

	if (switching)
	{
		if (switchType == 0)
			y += 0.5f;
		else
			y -= 0.5f;
	}
	else
	{

		if (falling)					// racoon falling 
			vy = MARIO_RACOON_FALL_VY;

		if (flying)						// racoon flying
			vy = MARIO_RACOON_FLY_VY;


		if (vx > MARIO_RUNNING_SPEED)
			vx = MARIO_RUNNING_SPEED;
		else if (vx < -MARIO_RUNNING_SPEED)
			vx = -MARIO_RUNNING_SPEED;

		if (!flying)
		{
			if (abs(vx) <= MARIO_WALKING_SPEED)
				speedStack = 0;
			else if (abs(vx) > MARIO_WALKING_SPEED && abs(vx) < MARIO_WALKING_SPEED + 0.01f)
				speedStack = 1;
			else if (abs(vx) > MARIO_WALKING_SPEED + 0.01f && abs(vx) < MARIO_WALKING_SPEED + 0.02f)
				speedStack = 2;
			else if (abs(vx) > MARIO_WALKING_SPEED + 0.02f && abs(vx) < MARIO_WALKING_SPEED + 0.03f)
				speedStack = 3;
			else if (abs(vx) > MARIO_WALKING_SPEED + 0.03f && abs(vx) < MARIO_WALKING_SPEED + 0.05f)
				speedStack = 4;
			else if (abs(vx) > MARIO_WALKING_SPEED + 0.05f && abs(vx) < MARIO_WALKING_SPEED + 0.07f)
				speedStack = 4;
			else if (abs(vx) > MARIO_WALKING_SPEED + 0.07f && abs(vx) < MARIO_WALKING_SPEED + 0.09f)
				speedStack = 5;
			else if (abs(vx) > MARIO_WALKING_SPEED + 0.09f && abs(vx) < MARIO_WALKING_SPEED + 0.11f)
				speedStack = 6;
			else if (abs(vx) == MARIO_RUNNING_SPEED)
				speedStack = 7;
		}

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

		if (vx == 0)
		{
			sliding = 0;
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

			if (untouchable == 0)
			{
				if (rdx != 0 && rdx != dx)
					x += nx * abs(rdx);
			}

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
							game->AddScore(100);
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									StartTransform_Racoon();
									level = MARIO_LEVEL_BIG;
									ResetState();
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									CMario::ToSmall(this->y);
									ResetState();
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
							game->AddScore(100);
						}
						else if (goomba->GetState() == FLYGOOMBA_STATE_WALKING)
						{
							goomba->SetState(FLYGOOMBA_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							game->AddScore(200);
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (goomba->GetState() != FLYGOOMBA_STATE_DIE)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									StartTransform_Racoon();
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
							game->AddScore(200);
						}
						else if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx != 0) {
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							koopas->vx = 0;		// being stomped when spining then still
							game->AddScore(100);
						}
						else
						{
							koopas->SetState(KOOPAS_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							game->AddScore(100);
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (koopas->vx != 0)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									StartTransform_Racoon();
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
							game->AddScore(100);
						}
						else if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx == 0) {
							if (this->nx > 0)  // direction of koopas spin when being stomped 
								koopas->vx = KOOPAS_SPIN_SPEED;
							else
								koopas->vx = -KOOPAS_SPIN_SPEED;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							game->AddScore(200);
						}
						else if ((koopas->GetState() == KOOPAS_STATE_DIE || koopas->GetState() == KOOPAS_STATE_DIE_DEFLECT) && koopas->vx != 0) {
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							koopas->vx = 0;		// being stomped when spining then still
							game->AddScore(100);
						}
						else
						{
							koopas->SetState(KOOPAS_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							game->AddScore(100);
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (koopas->vx != 0)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									StartTransform_Racoon();
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
				else if (dynamic_cast<CBoomerangBros*>(e->obj)) // if e->obj is Goomba 
				{
					CBoomerangBros* bros = dynamic_cast<CBoomerangBros*>(e->obj);
					// jump on top >> kill boomerang bros and deflect a bit 
					if (e->ny < 0)
					{
						if (bros->GetState() != BROS_STATE_DIE)
						{
							bros->SetState(BROS_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							game->AddScore(1000);
						}
					}
					else
					{
						if (untouchable == 0)
						{
							if (bros->GetState() != GOOMBA_STATE_DIE)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									StartTransform_Racoon();
									level = MARIO_LEVEL_BIG;
									ResetState();
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									CMario::ToSmall(this->y);
									ResetState();
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
						}
					}
				}
				else if (dynamic_cast<CPlant*>(e->obj))
				{
					CPlant* plant = dynamic_cast<CPlant*>(e->obj);
					if (untouchable == 0) {
						if (!plant->isUnderPipe)
						{
							if (level > MARIO_LEVEL_BIG)
							{
								StartTransform_Racoon();
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
							StartTransform_Racoon();
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
				else if (dynamic_cast<CBoomerang*>(e->obj))
				{
					CBoomerang* boomerang = dynamic_cast<CBoomerang*>(e->obj);
					if (e->ny > 0)
					{
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dy + ny * 0.4f;
					}
					if (untouchable == 0) {
						if (level > MARIO_LEVEL_BIG)
						{
							StartTransform_Racoon();
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
				}  // if boomerang
				else if (dynamic_cast<CPiranhaPlant*>(e->obj))
				{
					CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
					if (untouchable == 0) {
						if (!plant->isUnderPipe)
						{
							if (level > MARIO_LEVEL_BIG)
							{
								StartTransform_Racoon();
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
				else if (dynamic_cast<CMoveBar*>(e->obj))
				{
					CMoveBar* bar = dynamic_cast<CMoveBar*>(e->obj);
					if (e->ny < 0) // jump on top brick then can jumping again
					{
						isFlying = 0;
						falling = 0;		//	racoon mario cant fall slowly
						bar->SetState(BAR_STATE_FALLING);
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
					if (mushroom->color == 0)
					{
						if (level < MARIO_LEVEL_BIG)
						{
							CMario::ToBig(y);
							level = MARIO_LEVEL_BIG;
							StartTransform();
						}
					}
					else
					{
						game->AddLive(1);
					}

					vy = temp;							//Mario went through the mushroom
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
					game->AddScore(1000);
				}
				else if (dynamic_cast<CLeaf*>(e->obj))
				{
					CLeaf* leaf = dynamic_cast<CLeaf*>(e->obj);
					leaf->isFinish = 1;
					if (level == MARIO_LEVEL_SMALL)
					{
						CMario::ToBig(y);
						level = MARIO_LEVEL_BIG;
						StartTransform();
					}
					else if (level != MARIO_LEVEL_RACOON)
					{
						StartTransform_Racoon();
						level = MARIO_LEVEL_RACOON;
					}
					vy = temp;							//Mario went through the mushroom
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
					game->AddScore(1000);
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
							qbrick->stack--;
							if (qbrick->stack == 0)
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
							if (level > MARIO_LEVEL_SMALL)
							{
								bbrick->trigger = 1;
								bbrick->isFinish = 1;
							}
							else;
						}
					}
					else
					{
						bbrick->isFinish = 1;					// Make coin disappear
						vy = temp;							//Mario went through the coin
						x -= min_tx * dx + nx * 0.4f;
						//y -= min_ty * dy + ny * 0.4f;
						x += dx;
						if (flying)
						{
							//x += dx;
							y += dy;
						}
						game->AddScore(100);
						game->AddCoin();
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
					//y -= min_ty * dy + ny * 0.4f;
					x += dx;
					if (flying)
					{
						//x += dx;
						y += dy;
					}
					game->AddScore(100);
					game->AddCoin();
				}
				else if (dynamic_cast<CEndPointItem*>(e->obj)) // if e->obj is Coin 
					{
					CEndPointItem* item = dynamic_cast<CEndPointItem*>(e->obj);
					vy = temp;							//Mario went through the coin
					x -= min_tx * dx + nx * 0.4f;
					y -= min_ty * dy + ny * 0.4f;
					x += dx;
					y += dy;
					item->randomitem();
					game->PushItem(item->GetItem());
					SetState(MARIO_STATE_ENDGAME);
				}
				else if (dynamic_cast<CPortal*>(e->obj))
				{
					CPortal* p = dynamic_cast<CPortal*>(e->obj);
					CGame::GetInstance()->SwitchScene(p->GetSceneId());
					game->SetTime(0);
					obj = NULL;
					return;
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

		float camx, camy;
		game->GetCamPos(camx, camy);

		//if (vx > 0 && x > camx + game->GetScreenWidth() && state != MARIO_STATE_ENDGAME) x = camx + game->GetScreenWidth() - 30.0f;
		//if (x < camx) x = camx;


		//DebugOut(L"\tsliding = %f\n", sliding);

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
	else if (transformRacoon)
	{
		ani = MARIO_ANI_TRANSFORM_RACOON;
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
	{
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
				else if (sliding)
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
				else if (sliding )
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
				else if (sliding )
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
				else if (sliding)
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
				else if (sliding )
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
				else if (sliding)
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
				else if (sliding )
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
				else if (sliding)
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
	}
		
	if (untouchable)
	{
		if (alpha == 200) 
			alpha = 100;
		else
			alpha = 200;
	}
	else 
	{
		alpha = 255;
	}

	animation_set->at(ani)->Render(round(x),round(y), alpha);
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
			{
				if (vx < MARIO_WALKING_SPEED)
					vx = MARIO_WALKING_SPEED;
				if (vx < MARIO_RUNNING_SPEED && isFlying == 0)
					vx += 0.001f;
			}
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
			{
				if (vx > -MARIO_WALKING_SPEED)
					vx = -MARIO_WALKING_SPEED;
				if (vx > -MARIO_RUNNING_SPEED && isFlying == 0)
					vx -= 0.001f;
			}
		}
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		if (isFlying == 0 && isDucking == 0)
		{
			isFlying = 1;
			if (sliding)
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
		//StartSliding();
		sliding = 1;
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
		this->tail += 1;
		break;
	case MARIO_STATE_ENDGAME:
		ResetState();
		vx = MARIO_WALKING_SPEED - 0.02f;
		nx = 1;
		isRunning = 1;
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

void CMario::AnimationTime()
{
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

	if (GetTickCount64() - transRacoon_start > MARIO_TRANSFORM_TIME)	// mario transfomr to RACOON form time
	{
		transRacoon_start = 0;
		transformRacoon = 0;
	}
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
}

CGameObject* CMario::NewFireBall()		// create fireball function
{
	int ani_set_id = MARIO_FIREBALL_ANI_SET_ID;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;
	obj = new CFireBall(this->nx);
	obj->type = OBJECT_TYPE_FIREBALL;
	obj->SetPosition(this->x + MARIO_BIG_BBOX_WIDTH/2, this->y + MARIO_BIG_BBOX_HEIGHT/3);
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);
	return obj;
}

CGameObject* CMario::TailAttack()		// create fireball function
{
	int ani_set_id = MARIO_TAIL_ANI_SET_ID;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;
	float maxRange;
	if (this->nx == 1)
		maxRange = this->x + 25.0f;
	else
		maxRange = this->x - 10.0f;
	obj = new CTail(this->nx, maxRange);
	obj->type = OBJECT_TYPE_FIREBALL;
	obj->SetPosition(this->x + MARIO_BIG_BBOX_WIDTH / 2, this->y + MARIO_BIG_BBOX_HEIGHT / 2);
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);
	return obj;
}
