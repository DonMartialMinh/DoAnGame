#pragma once
#include "GameObject.h"

#define BUTTON_BBOX_WIDTH  16
#define BUTTON_BBOX_HEIGHT 16

#define BUTTON_BBOX_HEIGHT_STOMPED 7

#define BUTTON_STATE_BRICK 100
#define BUTTON_STATE_BUTTON 200
#define BUTTON_STATE_STOMPED 300

#define BUTTON_ANI_BRICK 0
#define BUTTON_ANI_BUTTON 1
#define BUTTON_ANI_STOMPED 2

class CPButton : public CGameObject
{
public:
	int trigger = 0;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CPButton();
	virtual void SetState(int state);
};