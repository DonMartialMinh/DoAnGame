#include "Board.h"

void CBoard::Render()
{
	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CBoard::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}

void CBoard::Update()
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	game->GetCamPos(camx, camy);
	float scrh = float(game->GetScreenHeight());
	this->x = camx;
	this->y = camy + scrh - BOARD_HEIGHT;


}