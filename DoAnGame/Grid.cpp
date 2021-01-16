﻿#include "Grid.h"

Grid::Grid()
{
}

Grid::~Grid()
{
	for (int i = 0; i < GRID_CELL_MAX_ROW; i++)
		for (int j = 0; j < GRID_CELL_MAX_COLUMN; j++)
		{
			cells[i][j].clear();
		}
}

void Grid::SetFile(char* str)
{
	filepath = str;
}

void Grid::loadGrid()
{
	for (int i = 0; i < GRID_CELL_MAX_ROW; i++)
		for (int j = 0; j < GRID_CELL_MAX_COLUMN; j++)
		{
			cells[i][j].clear();
		}



}

CGameObject* Grid::GetNewObject(int id, float x, float y, int ani, int h, int Model)
{
	//switch (type)
	//{
	//case eType::BRICK:
	//	return new Brick(x, y, w, h, Model);

	//case eType::TORCH:
	//	return new Torch(x, y);

	//case eType::OBJECT_HIDDEN:
	//	return new ObjectHidden(x, y, w, h);

	//case eType::CANDLE:
	//	return new Candle(x, y);

	//case eType::STAIR_TOP:
	//	return new StairTop(x, y);

	//case eType::STAIR_BOTTOM:
	//	return new StairBottom(x, y);

	//case eType::GATE:
	//	return new Gate(x, y);
	//}
	return NULL;
}

void Grid::GetListObject(vector<CGameObject*>& ListObj)
{
	ListObj.clear();

	unordered_map<int, CGameObject*> mapObject;
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	game->GetCamPos(camx, camy);

	int bottom = (int)((camy + SCREEN_HEIGHT) / GRID_CELL_HEIGHT);
	int top = (int)((camy) / GRID_CELL_HEIGHT);

	int left = (int)((camx) / GRID_CELL_WIDTH);
	int right = (int)((camx + SCREEN_WIDTH) / GRID_CELL_WIDTH);

	for (int i = top; i <= bottom; i++)
		for (int j = left; j <= right; j++)
			for (UINT k = 0; k < cells[i][j].size(); k++)
			{
				ListObj.insert(ListObj.end(), cells[i][j].begin(), cells[i][j].end());
			}
}
