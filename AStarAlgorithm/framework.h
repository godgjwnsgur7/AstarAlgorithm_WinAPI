// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <windowsx.h>
// C 런타임 헤더 파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commdlg.h>
#include <stdio.h>
// C++ 런타임 헤더 파일
#include <vector>
#include <string>
#include <iostream>
#include <numbers>
#include <random>
#include <algorithm>
#include <stack>

#pragma comment(lib, "Msimg32.lib")

using namespace std;

#undef max
#undef min

const int g_screenX = 1020;
const int g_screenY = 910;

enum EGameState
{
	Game_Setting = 0,
	Game_FindTarget = 1,
	Game_MoveTarget = 2,
	Game_End = 3,
};

enum EBlockState
{
	Block_None = 0,		// 기본
	Block_Wall = 1,		// 벽
	Block_Start = 2,	// 출발지
	Block_End = 3,		// 도착지
	Block_Mine = 4,		// 지나온 길
	Block_Queue = 5,	// 탐색 대상
	Block_Move = 6,		// 최단 거리
};

struct Block
{
public:
	POINT pos;
	int gCost = 0;
	int hCost = 0;
	int fCost = 0;

	EBlockState blockState = Block_None;
};