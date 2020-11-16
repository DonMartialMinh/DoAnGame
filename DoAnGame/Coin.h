#pragma once
#include "GameObject.h"

#define COIN_BBOX_WIDTH  14
#define COIN_BBOX_HEIGHT 16

#define COIN_STATE_COIN 100

#define COIN_ANI_COIN 0

class CCoin : public CGameObject
{
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	int isFinish = 0;
	CCoin();


};