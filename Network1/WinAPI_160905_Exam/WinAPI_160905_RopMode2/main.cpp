#include<Windows.h>
#include"resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("KeyDown");

enum { LINE = 1000, CURVE, CIRCLE, RECTANGLE };

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);

	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int sx, sy, oldx, oldy;
	static BOOL bNowDraw = FALSE;
	static TCHAR str[100];
	static RECT ShapeRt;
	static int Choice = LINE;
	switch (iMessage)
	{
	case WM_CREATE:
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_SHAPE_1:
			Choice = LINE;
			break;
		case ID_SHAPE_2:
			Choice = CURVE;
			break;
		case ID_SHAPE_3:
			Choice = CIRCLE;
			break;
		case ID_SHAPE_4:
			Choice = RECTANGLE;
			break;

		}
		return 0;

	case WM_LBUTTONDOWN:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		oldx = sx;
		oldy = sy;
		bNowDraw = TRUE;
		return 0;

	case WM_MOUSEMOVE:
		
		if (bNowDraw)
		{
			hdc = GetDC(hWnd);
			switch (Choice)
			{
			case LINE:
				SetROP2(hdc, R2_NOT);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				break;
			case CURVE:
				sx = LOWORD(lParam);
				sy = HIWORD(lParam);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				
				break;
			case CIRCLE:
				ShapeRt.left = sx;
				ShapeRt.top = sy;
				ShapeRt.right = oldx;
				ShapeRt.bottom = oldy;

				
				SetROP2(hdc, R2_NOTXORPEN);
				Ellipse(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				ShapeRt.right = oldx;
				ShapeRt.bottom = oldy;
				Ellipse(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				
				//Ellipse(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				/*
				SetROP2(hdc, R2_NOT);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				*/

				break;
			case RECTANGLE:
				ShapeRt.left = sx;
				ShapeRt.top = sy;
				ShapeRt.right = oldx;
				ShapeRt.bottom = oldy;

				SetROP2(hdc, R2_NOTXORPEN);
				Rectangle(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				ShapeRt.right = oldx;
				ShapeRt.bottom = oldy;
				Rectangle(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				break;
			}
			ReleaseDC(hWnd, hdc);
		}
		
		//oldx = LOWORD(lParam);
		//oldy = HIWORD(lParam);
		//InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_LBUTTONUP:
		bNowDraw = FALSE;
		hdc = GetDC(hWnd);

		//wsprintf(str, "start : %d, %d\nend : %d, %d", sx, sy, oldx, oldy);
		
		

		switch (Choice)
		{
			case LINE:
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				break;
			case CURVE:
				break;
			case CIRCLE:
				Ellipse(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				break;
			case RECTANGLE:
				Rectangle(hdc, ShapeRt.left, ShapeRt.top, ShapeRt.right, ShapeRt.bottom);
				break;
		}
		ReleaseDC(hWnd, hdc);
		//InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//TextOut(hdc, 100, 100, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}