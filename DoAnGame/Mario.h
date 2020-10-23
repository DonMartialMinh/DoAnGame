#pragma once
#include "GameObject.h"

#define MARIO_WALKING_SPEED		0.15f 
//0.1f
#define MARIO_JUMP_SPEED_Y		0.52f
#define MARIO_JUMP_DEFLECT_SPEED 0.2f
#define MARIO_GRAVITY			0.002f
#define MARIO_DIE_DEFLECT_SPEED	 0.5f

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200
#define MARIO_STATE_JUMP			300
#define MARIO_STATE_DIE				400

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
#define MARIO_ANI_FIRE_DUCK_right			21
#define MARIO_ANI_FIRE_DUCK_LEFT			22

#define MARIO_ANI_RACOOON_IDLE_RIGHT			30
#define MARIO_ANI_RACOOON_IDLE_LEFT				31
#define MARIO_ANI_RACOOON_WALKING_RIGHT			32
#define MARIO_ANI_RACOOON_WALKING_LEFT			33
#define MARIO_ANI_RACOOON_JUMP					34
#define MARIO_ANI_RACOOON_DUCK					35
	


#define MARIO_ANI_DIE				8

#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2
#define MARIO_LEVEL_RACOON 3
#define MARIO_LEVEL_FIRE 4

#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 27

#define MARIO_SMALL_BBOX_WIDTH  12
#define MARIO_SMALL_BBOX_HEIGHT 15

#define MARIO_UNTOUCHABLE_TIME 5000


class CMario : public CGameObject
{
	int level;
	int untouchable;
	int isJumping = 0;
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y;
public:
	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }

	void Reset();

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};