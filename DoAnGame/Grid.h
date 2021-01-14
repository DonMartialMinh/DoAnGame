#pragma once
#include "Game.h" 
#include "GameObject.h"
#include "Game.h"
#include "Brick.h"


using namespace std;

#define SCREEN_WIDTH 332 
#define SCREEN_HEIGHT 292

#define GRID_CELL_WIDTH (SCREEN_WIDTH/4.0f)
#define GRID_CELL_HEIGHT (SCREEN_HEIGHT/4.0f)

#define GRID_CELL_MAX_ROW 20
#define GRID_CELL_MAX_COLUMN 100 


class Grid
{
private:
	vector<CGameObject*> cells[GRID_CELL_MAX_ROW][GRID_CELL_MAX_COLUMN];
	char* filepath;

public:
	Grid();
	~Grid();

	void SetFile(char* str);
	void ReloadGrid();


	CGameObject* GetNewObject(int type, float x, float y, int w, int h, int Model);
	void Insert(int id, int type, int direction, float x, float y, int w, int h, int Model);
	void GetListObject(vector<CGameObject*>& ListObj);

};
