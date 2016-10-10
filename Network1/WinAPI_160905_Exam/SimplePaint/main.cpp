#include<Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("KeyDown");

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
	WndClass.lpszMenuName = NULL;
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

enum { ID_R1 = 101, ID_R2, ID_R3, ID_C1, ID_C2, ID_C3 };
HWND R1, R2, R3, C1, C2, C3;
COLORREF Color = RGB( 255,0,0 );
int PenWidth = 1;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN MyPen, OldPen;
	
	static int StartX, StartY, OldX, OldY;
	static bool bNowDraw = FALSE;
	switch (iMessage)
	{
	case WM_CREATE:
		CreateWindow("button", "Color", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			5, 5, 120, 110, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow("button", "Color", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			145, 5, 120, 110, hWnd, (HMENU)0, g_hInst, NULL);

		R1 = CreateWindow("button", "Red", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
			10, 20, 100, 30, hWnd, (HMENU)ID_R1, g_hInst, NULL);
		R2 = CreateWindow("button", "Blue", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			10, 50, 100, 30, hWnd, (HMENU)ID_R2, g_hInst, NULL);
		R3 = CreateWindow("button", "Green", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			10, 80, 100, 30, hWnd, (HMENU)ID_R3, g_hInst, NULL);
		CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R1);

		C1 = CreateWindow("button", "¾ã°Ô", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
			150, 20, 100, 30, hWnd, (HMENU)ID_C1, g_hInst, NULL);
		C2 = CreateWindow("button", "Áß°£", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			150, 50, 100, 30, hWnd, (HMENU)ID_C2, g_hInst, NULL);
		C3 = CreateWindow("button", "µÎ²®°Ô", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			150, 80, 100, 30, hWnd, (HMENU)ID_C3, g_hInst, NULL);
		CheckRadioButton(hWnd, ID_C1, ID_C3, ID_C1);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case ID_R1:
				Color = RGB(255, 0, 0);
				break;
			case ID_R2:
				Color = RGB(0, 0, 255);
				break;
			case ID_R3:
				Color = RGB(0, 255, 0);
				break;
			case ID_C1:
				PenWidth = 1;
				break;
			case ID_C2:
				PenWidth = 3;
				break;
			case ID_C3:
				PenWidth = 5;
				break;
		}
		return 0;
	case WM_LBUTTONDOWN:
		OldX = StartX = LOWORD(lParam);
		OldY = StartY = HIWORD(lParam);
		bNowDraw = TRUE;
		return 0;
	case WM_MOUSEMOVE:
		if (bNowDraw)
		{
			hdc = GetDC(hWnd);
			MyPen = CreatePen(PS_SOLID, PenWidth, Color);
			OldPen = (HPEN)SelectObject(hdc, MyPen);

			OldX = LOWORD(lParam);
			OldY = HIWORD(lParam);
			
			MoveToEx(hdc, StartX, StartY, NULL);
			LineTo(hdc, OldX, OldY);
			
			StartX = LOWORD(lParam);
			StartY = HIWORD(lParam);

			SelectObject(hdc, OldPen);
			DeleteObject(MyPen);
			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		bNowDraw = FALSE;

		//hdc = GetDC(hWnd);
		//MoveToEx(hdc, StartX, StartY, NULL);
		//LineTo(hdc, OldX, OldY);
		//ReleaseDC(hWnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}