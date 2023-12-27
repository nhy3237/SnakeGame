// SnakeGame.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "SnakeGame.h"
#include "Csnake.h"
#include <ctime>
#include <string>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 230709
void DrawFeed(HDC hdc, POINT ptFeedPos);

// 230710
BOOL CALLBACK Dialog_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

// 230711
void Init(HWND hWnd);
void Start(HDC hdc);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNAKEGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNAKEGAME));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKEGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SNAKEGAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 738, 840, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 230707
#define timer_ID_1 11

// 230707
static int ckey, hkey;
enum { RIGHT, LEFT, UP, DOWN };
static POINT ptSnakePos;

static vector<Csnake*> wSnake;
static Csnake* uSnake;
static bool chkHead = false;

static vector<POINT> Feed;
static bool chkFeed = false;

static int score;
string strScore = "";

static bool start = false;
static bool game = false;

static HDC hdcMem;
static HBITMAP hBitmap, hBitmapOld;
static RECT rectClient;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rectView = { 10,50,710,750 };

    switch (message)
    {
    case WM_CREATE:
    {
        for (int i = 4; i > 0; i--)
        {
            ptSnakePos = { 35 * i, 35 };
            uSnake = new Csnake(ptSnakePos);
            wSnake.push_back(uSnake);
        }

        // 먹이(0~19) - 230709
        srand(time(NULL));

        // 먹이 벡터로 구현 - 230710
        Feed.push_back({ 35 * (rand() % 20), 35 * (rand() % 20) });

        // 점수 - 230711
        score = 0;

        // 더블 버퍼링 - 230711
        hdcMem = CreateCompatibleDC(NULL);
        hBitmap = CreateCompatibleBitmap(GetDC(hWnd), rectClient.right, rectClient.bottom);
        hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
    }
        break;

    case WM_SIZE:
    {
        GetClientRect(hWnd, &rectClient);

        SelectObject(hdcMem, hBitmapOld);
        DeleteObject(hBitmap);
        hBitmap = CreateCompatibleBitmap(GetDC(hWnd), rectClient.right, rectClient.bottom);
        hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
    }

    // 230707
    case WM_TIMER:
        if (wParam == timer_ID_1)
        {
            if (start && !game)
            {
                game = true;
            }

            // 머리랑 몸통이랑 나눠서 update - 230709
            for (int i = wSnake.size() - 1; i > 0; i--)
            {
                wSnake[i]->BodyUpdate(wSnake[i - 1]->getSnakePos());
            }

            wSnake[0]->HeadUpdate(ckey, &hkey);

            if (chkFeed)
            {
                srand(time(NULL));
                Feed.push_back({ 35 * (rand() % 20), 35 * (rand() % 20) });

                chkFeed = false;
            }

            // 먹이 충돌 - 230710
            if (wSnake[0]->FeedCollision(Feed[0]))
            {
                Feed.clear();
                chkFeed = true;

                uSnake = new Csnake(wSnake[wSnake.size() - 1]->getSnakePos());
                wSnake.push_back(uSnake);
                
                score++;
            }

            // 맵 충돌 - 230710
            if (wSnake[0]->MapCollision(rectView))
            {
                KillTimer(hWnd, timer_ID_1);
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), hWnd, Dialog_Proc);
                break;
            }

            // 몸통 충돌 - 230711
            for (int i = wSnake.size() - 1; i > 0; i--)
            {
                if (wSnake[0]->BodyCollision(wSnake[i]->getSnakePos()))
                {
                    KillTimer(hWnd, timer_ID_1);
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), hWnd, Dialog_Proc);
                    break;
                }
            }
         
            InvalidateRgn(hWnd, NULL, FALSE);
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
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

    case WM_KEYDOWN:
    
        if (!start)
        {
            start = true;
            SetTimer(hWnd, timer_ID_1, 80, NULL);
            InvalidateRgn(hWnd, NULL, FALSE);
            break;
        }

        // 230707
        if (wParam == VK_RIGHT)
        {
            // 서로 반대 방향으로는 바뀔 수 없게 - 230709
            if (hkey != RIGHT && hkey != LEFT)
            {
                hkey = ckey;
                ckey = RIGHT;
            }
        }
        else if (wParam == VK_LEFT)
        {
            if (hkey != LEFT && hkey != RIGHT)
            {
                hkey = ckey;
                ckey = LEFT;
            }
        }
        else if (wParam == VK_UP)
        {
            if (hkey != UP && hkey != DOWN)
            {
                hkey = ckey;
                ckey = UP;
            }
        }
        else if (wParam == VK_DOWN)
        {
            if (hkey != DOWN && hkey != UP)
            {
                hkey = ckey;
                ckey = DOWN;
            }
        }

        InvalidateRgn(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (!start) // 시작화면
            {
                Start(hdc);
            }
            else
            {
                HBRUSH hbrBackground = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                FillRect(hdcMem, &rectClient, hbrBackground);
                DeleteObject(hbrBackground);

                // 230709
                wSnake[0]->DrawMap(hdcMem, rectView);
                for (int i = 0; i < wSnake.size(); i++)
                {
                    if (i == 0) chkHead = true;
                    else chkHead = false;
                    wSnake[i]->Draw(hdcMem, chkHead);
                }

                // 먹이 - 230709
                if (Feed.size() > 0)
                {
                    DrawFeed(hdcMem, Feed[0]);
                }

                strScore = "SCORE : " + to_string(score);
                TextOutA(hdcMem, 330, 18, strScore.c_str(), strScore.length());

                BitBlt(hdc, 0, 0, rectClient.right, rectClient.bottom, hdcMem, 0, 0, SRCCOPY);
            }
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
      
        for (int i = 0; i < wSnake.size(); i++) 
        {
            delete wSnake[i];
        }
        wSnake.clear();

        SelectObject(hdcMem, hBitmapOld);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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

void DrawFeed(HDC hdc, POINT ptFeedPos)
{
    HPEN hPen = CreatePen(PS_ALTERNATE, 2, RGB(198, 210, 248));
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hBrush = CreateSolidBrush(RGB(201, 213, 254));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Ellipse(hdc, 10 + ptFeedPos.x, 50 + ptFeedPos.y, 45 + ptFeedPos.x, 85 + ptFeedPos.y);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

// 230710
BOOL CALLBACK Dialog_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    TCHAR output[200];
    switch (iMsg)
    {
    case WM_INITDIALOG:
        _stprintf_s(output, _T("SCORE : %d"), score);
        SetDlgItemText(hDlg, IDC_STATIC2, output);
        return 1;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_PLAY:
            Init(GetParent(hDlg));
            EndDialog(hDlg, 0);
            break;
        case ID_EXIT:
            DestroyWindow(GetParent(hDlg));
            break;
        }
    }
    return 0;
}

void Init(HWND hWnd)
{
    ckey = 0; hkey = 0;

    wSnake.clear();
    for (int i = 4; i > 0; i--)
    {
        ptSnakePos = { 35 * i, 35 };
        uSnake = new Csnake(ptSnakePos);
        wSnake.push_back(uSnake);
    }
    chkHead = false;

    Feed.clear();
    srand(time(NULL));
    Feed.push_back({ 35 * (rand() % 20), 35 * (rand() % 20) });
    chkFeed = false;

    score = 0;

    SetTimer(hWnd, timer_ID_1, 80, NULL);
    InvalidateRgn(hWnd, NULL, FALSE);
}

void Start(HDC hdc)
{
    SetTextColor(hdc, RGB(75, 132, 69));
    HFONT hFont = CreateFont(70, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    TextOut(hdc, 150, 300, _T("SNAKE GAME"), 10);

    SetTextColor(hdc, RGB(93, 150, 87));
    hFont = CreateFont(25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    oldFont = (HFONT)SelectObject(hdc, hFont);
    TextOut(hdc, 260, 450, _T("아무 키나 눌러주세요.."), 13);

    hFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    oldFont = (HFONT)SelectObject(hdc, hFont);
    TextOut(hdc, 660, 750, _T("남희영"), 3);

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}