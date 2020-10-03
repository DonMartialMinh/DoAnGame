#pragma once
#include "GameObject.h"

#define MICS_WIDTH 16

class CMics : public CGameObject
{
public:
	CMics(float x, float y);
	void Update(DWORD dt);
	void Render();
};