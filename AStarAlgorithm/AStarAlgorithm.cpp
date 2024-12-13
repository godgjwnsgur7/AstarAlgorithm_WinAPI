// AStarAlgorithm.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "AStarAlgorithm.h"
#include "BlockPriorityQueue.h"
#include "Player.h"

#define MAX_LOADSTRING 100
#define BLOCKSIZE 50

#pragma warning(disable : 4996)

HINSTANCE hInst;                   
WCHAR szTitle[MAX_LOADSTRING];       
WCHAR szWindowClass[MAX_LOADSTRING]; 

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND winhWnd;
RECT rectView;
Player player;

Block* startBlock = nullptr;
bool isSetStartPoint = false;

Block* endBlock = nullptr;
bool isSetEndPoint = false;

Block* findBlock = nullptr;
bool isFindTarget = false;

vector<vector<Block>> blockArrays;
BlockPriorityQueue blockQueue;
stack<Block*> blockMoveStack;

EGameState gameState = Game_Setting;

int dirX[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int dirY[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

void Start()
{
    for (int y = 0; y <= 800 / BLOCKSIZE; y++)
    {
        vector<Block> vec(1000 / BLOCKSIZE);
        for (int x = 0; x < vec.size(); x++)
            vec[x].pos = POINT{ x * BLOCKSIZE, y * BLOCKSIZE };
        blockArrays.push_back(vec);
    }
}

Block* GetBestNearBlock(Block* target)
{
    Block* bestBlock = nullptr;
    for (int i = 0; i < 8; i++)
    {
        int tempY = target->pos.y / BLOCKSIZE + dirY[i];
        int tempX = target->pos.x / BLOCKSIZE + dirX[i];

        if (tempY < 0 || tempX < 0 || tempY >= blockArrays.size() || tempX >= blockArrays[0].size())
            continue;

        Block* tempBlock = &blockArrays[tempY][tempX];
        if (tempBlock->blockState != Block_Mine &&
            tempBlock->blockState != Block_Start)
            continue;

        if (bestBlock == nullptr || bestBlock->gCost > tempBlock->gCost)
            bestBlock = tempBlock;
    }

    return bestBlock;
}

#pragma region MoveTarget
void SetBlockMoveStack()
{
    findBlock = endBlock;
    
    // blockMoveStack Clear
    while (!blockMoveStack.empty())
        blockMoveStack.pop();
    
    while (true)
    {
        blockMoveStack.push(&(*findBlock));
        Block* moveBlock = GetBestNearBlock(findBlock);
        if (moveBlock == nullptr || moveBlock->blockState == Block_Start)
            break;
        
        moveBlock->blockState = Block_Move;
        findBlock = moveBlock;
    }
}

void MoveToPlayer()
{
    if (player.IsTargetArrive())
    {
        if (blockMoveStack.size() > 0)
        {
            Block* temp = blockMoveStack.top();
            blockMoveStack.pop();
            player.SetTargetPos({ temp->pos.x + (BLOCKSIZE / 2), temp->pos.y + (BLOCKSIZE / 2) });
        }
        else
        {
            gameState = Game_End;
            return;
        }
    }

    player.Update();
}
#pragma endregion

#pragma region FindTarget
void SetNearBlockCost(Block* target)
{
    if (target->blockState == Block_None)
    {
        Block* nearBlock = GetBestNearBlock(target);

        if (nearBlock == nullptr)
            return;

        int nearX = nearBlock->pos.x / BLOCKSIZE;
        int nearY = nearBlock->pos.y / BLOCKSIZE;
        int targetX = target->pos.x / BLOCKSIZE;
        int targetY = target->pos.y / BLOCKSIZE;

        int cost = (nearX == targetX) || (nearY == targetY) ? 10 : 14;
        target->gCost = cost + nearBlock->gCost;
        target->fCost = target->gCost + target->hCost;
    }
}

void FindNearBlock(int y, int x)
{
    int maxRangeX = blockArrays[0].size();
    int maxRangeY = blockArrays.size();

    for (int i = 0; i < 8; i++)
    {
        int movePosY = y / BLOCKSIZE + dirY[i];
        int movePosX = x / BLOCKSIZE + dirX[i];

        if (movePosY < 0 || movePosY >= maxRangeY || movePosX < 0 || movePosX >= maxRangeX)
            continue;

        if (blockArrays[movePosY][movePosX].blockState == Block_End)
        {
            SetBlockMoveStack();
            player.SpawnPlayer({ startBlock->pos.x + (BLOCKSIZE / 2), startBlock->pos.y + (BLOCKSIZE / 2) });
            player.SetTargetPos({ startBlock->pos.x + (BLOCKSIZE / 2), startBlock->pos.y + (BLOCKSIZE / 2) });
            gameState = Game_MoveTarget;
        }
        else if (blockArrays[movePosY][movePosX].blockState == Block_None)
        {
            SetNearBlockCost(&blockArrays[movePosY][movePosX]);
            blockArrays[movePosY][movePosX].blockState = Block_Queue;
            blockQueue.Enqueue(&blockArrays[movePosY][movePosX]);
        }
    }
}

void MoveToBestBlock()
{
    if (blockQueue.GetCount() == 0)
        return;

    findBlock = blockQueue.Dequeue();
    findBlock->blockState = Block_Mine;

    FindNearBlock(findBlock->pos.y, findBlock->pos.x);
}
#pragma endregion

DWORD previousTime = GetTickCount64();
float deltaTime = 0.0f;
void Update()
{
    if (isSetEndPoint == false)
        return;

    DWORD currentTime = GetTickCount64();
    deltaTime += (currentTime - previousTime);
    previousTime = currentTime;

    float waitTime = 0;
    switch (gameState)
    {
    case Game_FindTarget: waitTime = 100; break;
    default: waitTime = 0; break;
    }

    if (deltaTime >= waitTime)
    {
        switch (gameState)
        {
        case Game_FindTarget: 
            MoveToBestBlock();
            break;
        case Game_MoveTarget:
            MoveToPlayer();
            break;
        }
        deltaTime = 0;
    }
}

int GetCost(int posY, int posX, int targetPosY, int targetPosX)
{
    int cost = 0;
    int diffPosX = abs(posX - targetPosX);
    int diffPosY = abs(posY - targetPosY);
    while (diffPosX > 0 && diffPosY > 0)
    {
        diffPosX--;
        diffPosY--;
        cost += 14;
    }
    cost += diffPosX * 10;
    cost += diffPosY * 10;
    return cost;
}

void SetBlockCostAll()
{
    for (int y = 0; y < blockArrays.size(); y++)
        for (int x = 0; x < blockArrays[y].size(); x++)
            blockArrays[y][x].hCost = GetCost(y, x, endBlock->pos.y / BLOCKSIZE, endBlock->pos.x / BLOCKSIZE);
}

#pragma region MouseEvent
void OnMouseLeftClickEvent(LPARAM lParam)
{
    switch (gameState)
    {
    case Game_Setting:
    {
        int posY = GET_Y_LPARAM(lParam) / BLOCKSIZE;
        int posX = GET_X_LPARAM(lParam) / BLOCKSIZE;

        if (isSetStartPoint == false)
        {
            blockArrays[posY][posX].blockState = Block_Start;
            startBlock = &blockArrays[posY][posX];
            findBlock = &blockArrays[posY][posX];
            isSetStartPoint = true;
        }
        else if (isSetEndPoint == false)
        {
            gameState = Game_FindTarget;
            blockArrays[posY][posX].blockState = Block_End;
            endBlock = &blockArrays[posY][posX];
            isSetEndPoint = true;
            SetBlockCostAll();
            FindNearBlock(startBlock->pos.y, startBlock->pos.x);
        }
    }
        break;
    case Game_End:
        // ResetGame 해야 함
        gameState = Game_Setting;
        break;
    default:
        break;
    }
   
}

void OnMouseRightClickEvent(LPARAM lParam)
{
    switch (gameState)
    {
    case Game_Setting:
    {
        int posX = GET_X_LPARAM(lParam) / BLOCKSIZE; 
        int posY = GET_Y_LPARAM(lParam) / BLOCKSIZE;

        if (blockArrays[posY][posX].blockState == Block_Start)
            return;

        if (blockArrays[posY][posX].blockState == Block_Wall)
            blockArrays[posY][posX].blockState = Block_None;
        else
            blockArrays[posY][posX].blockState = Block_Wall;
    }
        break;
    case Game_End:
        break;
    default:
        break;
    }
}
#pragma endregion

#pragma region Draw
void Draw(HWND& hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
    SelectObject(memDC, memBitmap);
    FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    for (int y = 0; y < blockArrays.size(); y++)
    {
        for (int x = 0; x < blockArrays[y].size(); x++)
        {
            int posY = blockArrays[y][x].pos.y;
            int posX = blockArrays[y][x].pos.x;
            
            HBRUSH hBrush;
            switch (blockArrays[y][x].blockState)
            {
            case Block_Wall:    hBrush = CreateSolidBrush(RGB(0, 0, 0)); break; // 검정색
            case Block_Start:   hBrush = CreateSolidBrush(RGB(0, 191, 255)); break; // 하늘색
            case Block_End:     hBrush = CreateSolidBrush(RGB(0, 191, 255)); break; // 하늘색
            case Block_Mine:    hBrush = CreateSolidBrush(RGB(220, 20, 60)); break; // 빨강색
            case Block_Queue:   hBrush = CreateSolidBrush(RGB(144, 238, 0)); break; // 초록색
            case Block_Move:    hBrush = CreateSolidBrush(RGB(255, 140, 0)); break; // 주황색
            default:            hBrush = CreateSolidBrush(RGB(255, 255, 255)); break; // 흰색
            }

            HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, hBrush);

            Rectangle(memDC, posX, posY, posX + BLOCKSIZE, posY + BLOCKSIZE);
            SetBkMode(memDC, TRANSPARENT);

            if (blockArrays[y][x].blockState == Block_Start)
            {
                TextOut(memDC, posX + 2, posY + 17, _T("START"), 5);
            }
            else if (blockArrays[y][x].blockState == Block_End)
            {
                TextOut(memDC, posX + 10, posY + 17, _T("END"), 3);
            }
            else if (isSetEndPoint && blockArrays[y][x].fCost != 0)
            {
                TCHAR gCostText[200], hCostText[200], fCostText[200];
                _swprintf(gCostText, TEXT("G:%d"), blockArrays[y][x].gCost);
                _swprintf(hCostText, TEXT("H:%d"), blockArrays[y][x].hCost);
                _swprintf(fCostText, TEXT("F:%d"), blockArrays[y][x].fCost);

                TextOut(memDC, posX + 1, posY + 3, gCostText, strlen(to_string(blockArrays[y][x].gCost).c_str()) + 2);
                TextOut(memDC, posX + 1, posY + 17, hCostText, strlen(to_string(blockArrays[y][x].hCost).c_str()) + 2);
                TextOut(memDC, posX + 1, posY + 32, fCostText, strlen(to_string(blockArrays[y][x].fCost).c_str()) + 2);
            }
            
            SelectObject(memDC, oldBrush);
            DeleteObject(hBrush);
        }
    }

    if(gameState == Game_MoveTarget)
        player.Draw(memDC);

    BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    EndPaint(hWnd, &ps);
}
#pragma endregion

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ASTARALGORITHM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTARALGORITHM));

    MSG msg;

    DWORD previousTime = GetTickCount64();
    float deltaTime = 0.0f;

    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return (int)msg.wParam;
            }

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        DWORD currentTime = GetTickCount64();
        deltaTime += (currentTime - previousTime);
        previousTime = currentTime;

        if (deltaTime > 10)
        {
            Update();
            deltaTime = 0;
            InvalidateRect(winhWnd, NULL, FALSE);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTARALGORITHM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASTARALGORITHM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, _T("A*알고리즘 - 허준혁"), WS_OVERLAPPEDWINDOW,
       500, 0, g_screenX, g_screenY, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   winhWnd = hWnd;
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        GetClientRect(hWnd, &rectView);
        Start();
        break;
    case WM_LBUTTONDOWN:
        OnMouseLeftClickEvent(lParam);
        break;
    case WM_RBUTTONDOWN:
        OnMouseRightClickEvent(lParam);
        break;
    case WM_PAINT:
        Draw(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}