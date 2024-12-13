#include "Player.h"

Player::Player() {}

bool Player::IsTargetArrive()
{
	return (pos.x == targetPos.x && pos.y == targetPos.y);
}

void Player::SpawnPlayer(POINT pos)
{
	this->pos = pos;
}

void Player::SetTargetPos(POINT targetPos)
{
	this->targetPos = targetPos;
}

void Player::Update()
{
	if (pos.x > targetPos.x)
		pos.x--;
	else if (pos.x < targetPos.x)
		pos.x++;

	if (pos.y > targetPos.y)
		pos.y--;
	else if (pos.y < targetPos.y)
		pos.y++;
}

void Player::Draw(HDC& hdc)
{
	HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	HPEN pen = CreatePen(1, 2.0f, RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	Ellipse(hdc, pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius);

	SelectObject(hdc, oldPen);
	DeleteObject(pen);
	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);
	DeleteObject(brush);
}