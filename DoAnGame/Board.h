#pragma once
#include "GameObject.h"
#include "Game.h"


class CBoard : public CGameObject
{
public:
	virtual void Render();
	virtual void Update();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};