#pragma once
#include "GameObject.h"
#include "FireBall.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Portal.h"
#include "Brick.h"
#include "UpsideBrick.h"
#include "Coin.h"
#include "Environment.h"
#include "QBrick.h"


#define MARIO_WALKING_SPEED		0.13f 
#define MARIO_RUNNING_SPEED		0.2f 
//0.1f
#define MARIO_JUMP_SPEED_Y		0.52f
#define MARIO_JUMP_DEFLECT_SPEED 0.3f
#define MARIO_GRAVITY			0.002f
#define MARIO_DIE_DEFLECT_SPEED	 0.5f

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200
#define MARIO_STATE_JUMP			300
#define MARIO_STATE_DIE				400
#define MARIO_STATE_DUCK			500
#define MARIO_RACOON_STATE_FALL		600
#define MARIO_RACOON_STATE_FLY		700
#define MARIO_RACOON_STATE_TURN		800
#define MARIO_RACOON_STATE_TAIL		900
#define MARIO_STATE_HOLD			1000
#define MARIO_STATE_SLIDE			1100
#define MARIO_FIRE_STATE_THROW		1200

#define MARIO_ANI_BIG_IDLE_RIGHT			0
#define MARIO_ANI_BIG_IDLE_LEFT				1
#define MARIO_ANI_SMALL_IDLE_RIGHT			2
#define MARIO_ANI_SMALL_IDLE_LEFT			3

#define MARIO_ANI_BIG_WALKING_RIGHT			4
#define MARIO_ANI_BIG_WALKING_LEFT			5
#define MARIO_ANI_SMALL_WALKING_RIGHT		6
#define MARIO_ANI_SMALL_WALKING_LEFT		7

#define MARIO_ANI_BIG_JUMP_RIGHT			9
#define MARIO_ANI_BIG_JUMP_LEFT				10
#define MARIO_ANI_SMALL_JUMP_RIGHT			11
#define MARIO_ANI_SMALL_JUMP_LEFT			12
#define MARIO_ANI_BIG_DUCK_RIGHT			13
#define MARIO_ANI_BIG_DUCK_LEFT				14

#define MARIO_ANI_FIRE_IDLE_RIGHT			15
#define MARIO_ANI_FIRE_IDLE_LEFT			16
#define MARIO_ANI_FIRE_WALKING_RIGHT		17
#define MARIO_ANI_FIRE_WALKING_LEFT			18
#define MARIO_ANI_FIRE_JUMP_RIGHT			19
#define MARIO_ANI_FIRE_JUMP_LEFT			20
#define MARIO_ANI_FIRE_DUCK_RIGHT			21
#define MARIO_ANI_FIRE_DUCK_LEFT			22

#define MARIO_ANI_RACOON_IDLE_RIGHT			23
#define MARIO_ANI_RACOON_IDLE_LEFT			24
#define MARIO_ANI_RACOON_WALKING_RIGHT		25
#define MARIO_ANI_RACOON_WALKING_LEFT		26
#define MARIO_ANI_RACOON_JUMP_RIGHT			27
#define MARIO_ANI_RACOON_JUMP_LEFT			28
#define MARIO_ANI_RACOON_DUCK_RIGHT			29
#define MARIO_ANI_RACOON_DUCK_LEFT			30

#define MARIO_ANI_BIG_FALL_RIGHT			31
#define MARIO_ANI_BIG_FALL_LEFT				32
#define MARIO_ANI_FIRE_FALL_RIGHT			33
#define MARIO_ANI_FIRE_FALL_LEFT			34
#define MARIO_ANI_RACOON_FALL_RIGHT			35
#define MARIO_ANI_RACOON_FALL_LEFT			36
#define MARIO_ANI_RACOON_FALL_RIGHT_1		37
#define MARIO_ANI_RACOON_FALL_LEFT_1		38
#define MARIO_ANI_RACOON_FLYING_RIGHT		39
#define MARIO_ANI_RACOON_FLYING_LEFT		40

#define MARIO_ANI_SMALL_RUNNING_RIGHT		41
#define MARIO_ANI_SMALL_RUNNING_LEFT		42
#define MARIO_ANI_BIG_RUNNING_RIGHT			43
#define MARIO_ANI_BIG_RUNNING_LEFT			44
#define MARIO_ANI_FIRE_RUNNING_RIGHT		45
#define MARIO_ANI_FIRE_RUNNING_LEFT			46
#define MARIO_ANI_RACOON_RUNNING_RIGHT		47
#define MARIO_ANI_RACOON_RUNNING_LEFT		48

#define MARIO_ANI_SMALL_TURNING_RIGHT		49
#define MARIO_ANI_SMALL_TURNING_LEFT		50
#define MARIO_ANI_BIG_TURNING_RIGHT			51
#define MARIO_ANI_BIG_TURNING_LEFT			52
#define MARIO_ANI_FIRE_TURNING_RIGHT		53
#define MARIO_ANI_FIRE_TURNING_LEFT			54
#define MARIO_ANI_RACOON_TURNING_RIGHT		55
#define MARIO_ANI_RACOON_TURNING_LEFT		56

#define MARIO_ANI_RACOON_TAIL_RIGHT			57
#define MARIO_ANI_RACOON_TAIL_LEFT			58

#define MARIO_ANI_SMALL_KICK_LEFT			59
#define MARIO_ANI_SMALL_KICK_RIGHT			60
#define MARIO_ANI_BIG_KICK_LEFT				61
#define MARIO_ANI_BIG_KICK_RIGHT			62
#define MARIO_ANI_FIRE_KICK_LEFT			63
#define MARIO_ANI_FIRE_KICK_RIGHT			64
#define MARIO_ANI_RACOON_KICK_LEFT			65
#define MARIO_ANI_RACOON_KICK_RIGHT			66	

#define MARIO_ANI_BIG_HOLD_STILL_RIGHT		67
#define MARIO_ANI_BIG_HOLD_WALK_RIGHT		68
#define MARIO_ANI_BIG_HOLD_STILL_LEFT		69
#define MARIO_ANI_BIG_HOLD_WALK_LEFT		70
#define MARIO_ANI_SMALL_HOLD_STILL_RIGHT	71
#define MARIO_ANI_SMALL_HOLD_WALK_RIGHT		72
#define MARIO_ANI_SMALL_HOLD_STILL_LEFT		73
#define MARIO_ANI_SMALL_HOLD_WALK_LEFT		74
#define MARIO_ANI_FIRE_HOLD_STILL_RIGHT		75
#define MARIO_ANI_FIRE_HOLD_WALK_RIGHT		76
#define MARIO_ANI_FIRE_HOLD_STILL_LEFT		77
#define MARIO_ANI_FIRE_HOLD_WALK_LEFT		78
#define MARIO_ANI_RACOON_HOLD_STILL_RIGHT	79
#define MARIO_ANI_RACOON_HOLD_WALK_RIGHT	80
#define MARIO_ANI_RACOON_HOLD_STILL_LEFT	81
#define MARIO_ANI_RACOON_HOLD_WALK_LEFT		82

#define MARIO_ANI_SMALL_SLIDE_RIGHT			83
#define MARIO_ANI_SMALL_SLIDE_LEFT			84
#define MARIO_ANI_BIG_SLIDE_RIGHT			85
#define MARIO_ANI_BIG_SLIDE_LEFT			86
#define MARIO_ANI_FIRE_SLIDE_RIGHT			87
#define MARIO_ANI_FIRE_SLIDE_LEFT			88
#define MARIO_ANI_RACOON_SLIDE_RIGHT		89
#define MARIO_ANI_RACOON_SLIDE_LEFT			90

#define MARIO_ANI_FIRE_THROW_RIGHT			91
#define MARIO_ANI_FIRE_THROW_LEFT			92
#define MARIO_ANI_FIRE_THROW2_RIGHT			93
#define MARIO_ANI_FIRE_THROW2_LEFT			94

#define MARIO_ANI_SMALL_FLY_RIGHT			95
#define MARIO_ANI_SMALL_FLY_LEFT			96
#define MARIO_ANI_BIG_FLY_RIGHT				97
#define MARIO_ANI_BIG_FLY_LEFT				98
#define MARIO_ANI_FIRE_FLY_RIGHT			99
#define MARIO_ANI_FIRE_FLY_LEFT				100
#define MARIO_ANI_RACOON_FLY_RIGHT			101
#define MARIO_ANI_RACOON_FLY_LEFT			102
#define MARIO_ANI_RACOON_FLY_RIGHT1			103
#define MARIO_ANI_RACOON_FLY_LEFT1			104


#define MARIO_ANI_DIE				8

#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2
#define MARIO_LEVEL_RACOON 3
#define MARIO_LEVEL_FIRE 4

#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 27

#define MARIO_BIG_DUCK_BBOX_HEIGHT 18

#define MARIO_RACOON_BBOX_WIDTH  21
#define MARIO_RACOON_BBOX_HEIGHT 27

#define MARIO_SMALL_BBOX_WIDTH  12
#define MARIO_SMALL_BBOX_HEIGHT 15

#define MARIO_UNTOUCHABLE_TIME 2000
#define MARIO_FALLING_TIME 300
#define MARIO_FLYING_TIME 300
#define MARIO_TURNING_TIME 200
#define MARIO_TAILING_TIME 300
#define MARIO_KICKING_TIME 200
#define MARIO_SLIDING_TIME	1000
#define MARIO_THROWING_TIME	300

class CMario : public CGameObject
{
	int level;
	int untouchable;
	int falling;


	DWORD untouchable_start;
	DWORD fall_start;
	DWORD fly_start;
	DWORD turn_start;
	DWORD tail_start;
	DWORD kick_start;
	DWORD slide_start;
	DWORD throw_start;
	float start_x;			// initial position of Mario at scene
	float start_y;

public:
	CGameObject* obj = NULL;
	int fireball = 0;
	int flying;
	int isFlying = 0;
	int isDucking = 0;
	int isRunning = 0;
	int turning = 0;
	int tailing = 0;
	int kicking = 0;
	int holding = 0;
	int canHold = 0;
	int canSlide = 0;
	int sliding = 0;
	int throwing = 0;
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	CGameObject* NewFireBall();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	void StartFalling() { falling = 1; fall_start = GetTickCount(); }
	void StartFlying() { flying = 1; fly_start = GetTickCount(); }
	void StartTurning() { turning = 1; turn_start = GetTickCount(); }
	void StartTailing() { tailing = 1; tail_start = GetTickCount(); }
	void StartKicking() { kicking = 1; kick_start = GetTickCount(); }
	void StartSliding() { if(slide_start == 0) slide_start = GetTickCount(); }
	void StartThrowing() { throwing = 1; throw_start = GetTickCount(); }

	void ResetState()
	{
		 isDucking = 0;
		 isRunning = 0;
		 turning = 0;
		 tailing = 0;
		 kicking = 0;
		 holding = 0;
		 canHold = 0;
		 canSlide = 0;
		 sliding = 0;
		 throwing = 0;
	}

	static void ToSmall(float& y) { y += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT); };
	static void ToBig(float& y) { y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT); };

	void Reset();

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	int getLevel();

};