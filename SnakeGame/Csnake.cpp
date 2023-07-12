#include "Csnake.h"

Csnake::Csnake()
{
    ptSnakePos = { 0, 0 };
}

Csnake::Csnake(POINT ptSnakePos)
{
    this->ptSnakePos = { ptSnakePos.x, ptSnakePos.y };
}

Csnake::~Csnake()
{
}

void Csnake::Draw(HDC hdc, bool chkHead)
{
    HPEN hPen, oldPen;
    HBRUSH hBrush, oldBrush;

    if (chkHead)
    {
        hPen = CreatePen(PS_ALTERNATE, 1, RGB(75, 132, 69));
        hBrush = CreateSolidBrush(RGB(75, 132, 69));
    }
    else
    {
        hPen = CreatePen(PS_ALTERNATE, 1, RGB(93, 150, 87));
        hBrush = CreateSolidBrush(RGB(93, 150, 87));
    }
    oldPen = (HPEN)SelectObject(hdc, hPen);
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Rectangle(hdc, 115 + ptSnakePos.x, 120 + ptSnakePos.y, 150 + ptSnakePos.x, 155 + ptSnakePos.y);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

// ¸Ê - 230707
void Csnake::DrawMap(HDC hdc, RECT rectView)
{
    HPEN hPen = CreatePen(PS_ALTERNATE, 1, RGB(237, 250, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hBrush = CreateSolidBrush(RGB(243, 252, 255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // 0 0 35 35 / 35 35 70 70
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Rectangle(hdc, 10 + (70 * j), 50 + (70 * i), 45 + (70 * j), 85 + (70 * i));
            Rectangle(hdc, 45 + (70 * j), 85 + (70 * i), 80 + (70 * j), 120 + (70 * i));
        }
    }

    hPen = CreatePen(PS_ALTERNATE, 2, RGB(219, 236, 242));
    oldPen = (HPEN)SelectObject(hdc, hPen);
    hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Rectangle(hdc, rectView.left, rectView.top, rectView.right, rectView.bottom);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(hBrush);
}

void Csnake::HeadUpdate(int ckey, int* hkey)
{
    switch (ckey)
    {
    case RIGHT:
        *hkey = RIGHT;
        ptSnakePos.x += 35;
        break;
    case LEFT:
        *hkey = LEFT;
        ptSnakePos.x -= 35;
        break;
    case UP:
        *hkey = UP;
        ptSnakePos.y -= 35;
        break;
    case DOWN:
        *hkey = DOWN;
        ptSnakePos.y += 35;
        break;
    }
}

void Csnake::BodyUpdate(POINT ptSnakePos)
{
    this->ptSnakePos = { ptSnakePos };
}

POINT Csnake::SnakePos()
{
    return ptSnakePos;

}
BOOL Csnake::FeedCollision(POINT ptFeedPos)
{
    if (ptFeedPos.x == ptSnakePos.x + 105 && ptFeedPos.y == ptSnakePos.y + 70)
    {
        return 1;
    }
    return 0;
}

BOOL Csnake::MapCollision(RECT rectView)
{
    if (rectView.left > 115 + ptSnakePos.x || rectView.top > 120  + ptSnakePos.y
        || rectView.right < 150 + ptSnakePos.x || rectView.bottom < 155 + ptSnakePos.y)
    {
        return 1;
    }
    return 0;
}

BOOL Csnake::BodyCollision(POINT ptBodyPos)
{
    if (ptSnakePos.x == ptBodyPos.x && ptSnakePos.y == ptBodyPos.y)
    {
        return 1;
    }
    return 0;
}
