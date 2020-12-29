#pragma once
#include "GameObject.h"
#include "Game.h"

#define BOARD_HEIGHT 52


class CBoard : public CGameObject
{
public:
	virtual void Render();
	virtual void Update();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};