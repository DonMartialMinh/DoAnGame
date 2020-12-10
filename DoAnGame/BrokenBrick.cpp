#include "BrokenBrick.h"

void CBrokenBrick::Render()
{
	if (isFinish)
		return;
	animation_set->at(0)->Render(x, y);
	//RenderBoundingBox();
}

void CBrokenBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}

vector<CGameObject*>CBrokenBrick::Broken()
{
	int ani_set_id = 106;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	vector<CGameObject*> VCGameObject;
	for (int i = 0; i < 4; i++)
	{
		CGameObject* obj = NULL;
		obj = new CFragment(i);
		obj->SetPosition(this->x, this->y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
		VCGameObject.push_back(obj);
	}
	return VCGameObject;
}