#include "QBrick.h"

CQBrick::CQBrick(int setting)
{
	this->setting = setting;
	ring_start = NULL;
	SetState(BRICK_STATE_QUES);
}

void CQBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);

	if (GetTickCount64() - ring_start > BRICK_RINGING_TIME)
	{
		ring_start = 0;
		ringing = 0;
	}

	if (ringing)
	{
		if (GetTickCount64() - ring_start >= BRICK_RINGING_TIME / 2)
			y += 1;
		else
			y -= 1;
	}

}

void CQBrick::Render()
{
	int ani = BRICK_ANI_QUES;
	if (state == BRICK_STATE_EMP) {
		ani = BRICK_ANI_EMP;
	}
	animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CQBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}

CGameObject* CQBrick::ShowItem()
{
	CGameObject* obj = NULL;
	if (setting == 0)
	{
		int ani_set_id = 12;
		int isSparkle = 1;
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		obj = new CCoin(isSparkle);
		obj->SetPosition(this->x, this->y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
	}
	return obj;
}

