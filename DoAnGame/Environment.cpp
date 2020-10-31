#include "Environment.h"

void CEnvironment::Render()
{
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}

void CEnvironment::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}