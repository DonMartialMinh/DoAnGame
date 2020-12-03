#pragma once
#include "GameObject.h"
#include "Mario.h"

#define PIRANHAPLANT_BBOX_WIDTH  16
#define PIRANHAPLANT_BBOX_HEIGHT 32

#define PIRANHAPLANT_ANI_BOTLEFT_STILL		0
#define PIRANHAPLANT_ANI_BOTLEFT			1
#define PIRANHAPLANT_ANI_TOPLEFT_STILL		2
#define PIRANHAPLANT_ANI_TOPLEFT			3
#define PIRANHAPLANT_ANI_BOTRIGHT_STILL		4
#define PIRANHAPLANT_ANI_BOTRIGHT			5
#define PIRANHAPLANT_ANI_TOPRIGHT_STILL		6
#define PIRANHAPLANT_ANI_TOPRIGHT			7

#define PIRANHAPLANT_RISING_TIME 7000

class CPiranhaPlant : public CGameObject
{
	DWORD rise_start;
	int rising = 0;
	CGameObject* player = NULL;
	float min;
	float max;
public:
	int isUnderPipe = 0;
	int climax = 0;
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CPiranhaPlant(CGameObject* player, float y);
	void StartRising() { rising = 1; rise_start = GetTickCount(); }
};