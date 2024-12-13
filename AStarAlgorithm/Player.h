#pragma once
#include "framework.h"

class Player
{
	int radius = 7;
	POINT pos{ 0, 0 };
	POINT targetPos{ 0, 0 };
public:
	Player();
	bool IsTargetArrive();
	void SpawnPlayer(POINT pos);
	void SetTargetPos(POINT targetPos);
	void Update();
	void Draw(HDC& hdc);
};