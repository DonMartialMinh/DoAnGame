#pragma once
#include "GameObject.h"

#define MARIO_WALKING_SPEED		0.15f 
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
//#define MARIO_ANI_BIG_CHANGE_DIR_RIGHT		33
//#define MARIO_ANI_BIG_CHANGE_DIR_LEFT		34
#define MARIO_ANI_FIRE_FALL_RIGHT			33
#define MARIO_ANI_FIRE_FALL_LEFT			34
//#define MARIO_ANI_FIRE_CHANGE_DIR_RIGHT		37
//#define MARIO_ANI_FIRE_CHANGE_DIR_LEFT		38
#define MARIO_ANI_RACOON_FALL_RIGHT			35
#define MARIO_ANI_RACOON_FALL_LEFT			36
#define MARIO_ANI_RACOON_FALL_RIGHT_1		37
#define MARIO_ANI_RACOON_FALL_LEFT_1		38
//#define MARIO_ANI_RACOON_CHANGE_DIR_RIGHT	41
//#define MARIO_ANI_RACOON_CHANGE_DIR_LEFT	42
//#define MARIO_ANI_SMALL_CHANGE_DIR_RIGHT	43
//#define MARIO_ANI_SMALL_CHANGE_DIR_RIGHT	44

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

#define MARIO_UNTOUCHABLE_TIME 5000
#define MARIO_FALLING_TIME 300

class CMario : public CGameObject
{
	int level;
	int untouchable;
	int falling;

	DWORD untouchable_start;
	DWORD fall_start;

	float start_x;			// initial position of Mario at scene
	float start_y;
public:
	int isFlying = 0;
	int isDucking = 0;
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	void StartFalling() { falling = 1; fall_start = GetTickCount(); }

	void Reset();

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	int getLevel();

};