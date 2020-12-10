#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Fragment.h"


class CBrokenBrick : public CGameObject
{
public:
	int isFinish = 0;
	int trigger = 0;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	vector<CGameObject*> Broken();
};