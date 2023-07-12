#pragma once
#include <vector>
using namespace std;

#include "framework.h"
BOOL                InitInstance(HINSTANCE, int);

class Csnake
{
private:
	POINT ptSnakePos;
	enum { RIGHT, LEFT, UP, DOWN };

public:
	Csnake();
	Csnake(POINT ptSnakePos);
	~Csnake();
	void Draw(HDC hdc, bool chkHead);
	void DrawMap(HDC hdc, RECT rectView);
	void HeadUpdate(int ckey, int *hkey);
	void BodyUpdate(POINT ptSnakePos);
	POINT SnakePos();
	BOOL FeedCollision(POINT ptFeedPos);
	BOOL MapCollision(RECT rectView);
	BOOL BodyCollision(POINT ptBodyPos);
};

