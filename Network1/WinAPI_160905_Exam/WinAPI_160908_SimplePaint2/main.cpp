#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
#include"resource.h"
#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib, "ws2_32")

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("KeyDown");
typedef struct Point
{
	int x, y;
}Point;
typedef struct Line
{
	Point pos;
	COLORREF Color;
	int Width;
	BOOL move, IsDraw;
	double time;
}Line;

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

enum { ID_R1 = 10001, ID_R2, ID_R3, ID_C1, ID_C2, ID_C3 };
HWND R1, R2, R3, C1, C2, C3;
COLORREF Color;
int PenWidth;


void DrawLine(HWND &hWnd, Line* pArray, int &count);
void err_quit(char *msg);
void err_display(char *msg);

BOOL CALLBACK DrawSetDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RED:
			Color = RGB(255, 0, 0);
			break;
		case IDC_BLUE:
			Color = RGB(0, 0, 255);
			break;
		case IDC_GREEN:
			Color = RGB(0, 255, 0);
			break;
		case IDC_THIN:
			PenWidth = 1;
			break;
		case IDC_NORMAR:
			PenWidth = 3;
			break;
		case IDC_THICK:
			PenWidth = 5;
			break;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

HWND Clear, Save, Load, Send;
static SYSTEMTIME hSt, oldSt;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN MyPen, OldPen;
	static Point Start, Old;
	static Line PointArray[1000];
	static int ArrayCount;
	static bool bNowLineDraw = FALSE, bNowDraw = FALSE, check;
	static TCHAR str[256];
	FILE* file = NULL;
	////////////****Network*********//////////////
	static int retval;
	// 윈속 초기화
	static WSADATA wsa;
	// socket()
	static SOCKET sock;
	// connect()
	static SOCKADDR_IN serveraddr;

	static char* buf;
	static char *testdata[] = {
		"안녕하세요",
		"반가워요",
		"오늘따라 할 이야기가 많을 것 같네요",
		"저도 그렇네요",
	};
	static size_t len;
	/*
	LARGE_INTEGER st, ed, freq;
	double gap;
	*/
	static double before, after, gap;
	////////////////////////////////////////////////
	//////////////////////////////////////
	switch (iMessage)
	{
	case WM_CREATE:
		GetLocalTime(&hSt);
		before = hSt.wHour * 3600 + hSt.wMinute * 60 + hSt.wSecond + hSt.wMilliseconds * 0.001;
		//memset(PointArray, 0, sizeof(PointArray));
		Color = RGB(255, 0, 0);
		PenWidth = 1;
		ArrayCount = 0;
		CreateWindow("button", "Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 400, 50, 100, 25, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow("button", "Save", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 50, 100, 25, hWnd, (HMENU)1, g_hInst, NULL);
		CreateWindow("button", "Load", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 600, 50, 100, 25, hWnd, (HMENU)2, g_hInst, NULL);
		CreateWindow("button", "Send", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 50, 100, 25, hWnd, (HMENU)3, g_hInst, NULL);
		////////////////////////////////////
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			return 1;
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) err_quit("socket()");

		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
		serveraddr.sin_port = htons(SERVERPORT);
		retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connect()");

		SetTimer(hWnd, 0, 16, NULL);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0:
			for (int i = 0; i < 1000; i++)
				memset(&PointArray[i], 0, sizeof(PointArray[i]));
			ArrayCount = 0;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 1:
			file = fopen("Data.txt", "wt");

			for (int i = 0; i < ArrayCount; i++)
			{
				fprintf(file, "%d %d %d %d %d %lf\n",
					PointArray[i].pos.x, PointArray[i].pos.y, PointArray[i].Color, PointArray[i].Width, PointArray[i].move, PointArray[i].time);
			}

			fclose(file);
			memset(PointArray, 0, sizeof(PointArray));
			break;
		case 2:
			GetLocalTime(&hSt);
			before = hSt.wHour * 3600 + hSt.wMinute * 60 + hSt.wSecond + hSt.wMilliseconds * 0.001;
			DrawLine(hWnd, PointArray, ArrayCount);
			SetTimer(hWnd, 1, 16, NULL);
			bNowLineDraw = TRUE;
			break;
		case 3:
			// 서버와 데이터 통신
			for (int i = 0; i < ArrayCount; i++) {
				// 데이터 입력(시뮬레이션)
				len = sizeof(PointArray[i]);
				buf = new char[len];
				//printf("%d번째 len : %d\n", i, len);
				ZeroMemory(buf, len);
				memcpy(buf, (char*)&PointArray[i], sizeof(PointArray[i]));
				//memcpy(buf, (char*)&temp[0], sizeof(Line));
				printf("%d번째 len : %d sizeof(PointArray[i]) : %d\n", i, len, sizeof(PointArray[i]));

				/*
				memcpy(buf, (char*)&temp[0], 32);
				memcpy(&dest, buf, 32);
				*/

				// 데이터 보내기(고정 길이)
				retval = send(sock, (char *)&len, sizeof(int), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}

				// 데이터 보내기(가변 길이)
				retval = send(sock, buf, len, 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
			}
			printf("\n\n\n%d\n\n\n", ArrayCount);
			break;
		}
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
			case 0:
				break;
			case 1:
				GetLocalTime(&oldSt);
				after = (oldSt.wHour * 3600 + oldSt.wMinute * 60 + oldSt.wSecond + oldSt.wMilliseconds * 0.001);
				gap = after - before;
				for (int i = 0; i < ArrayCount; i++)
				{
					if (PointArray[i].time <= gap)
					{
						//printf("%lf >= %lf\n", PointArray[i].time, gap);
						PointArray[i].IsDraw = TRUE;
					}
				}

				if (PointArray[ArrayCount].time > gap)
				{
					KillTimer(hWnd, 1);
					bNowLineDraw = FALSE;
				}
				InvalidateRect(hWnd, NULL, TRUE);
				break;
		}
		return 0;
	case WM_LBUTTONDOWN:
		Start.x = LOWORD(lParam);
		Start.y = HIWORD(lParam);

		GetLocalTime(&oldSt);
		PointArray[ArrayCount].pos = Start;
		PointArray[ArrayCount].Color = Color;
		PointArray[ArrayCount].Width = PenWidth;
		PointArray[ArrayCount].move = TRUE;
		after = oldSt.wHour * 3600 + oldSt.wMinute * 60 + oldSt.wSecond + oldSt.wMilliseconds * 0.001;
		//printf("저장시간 : %lf\n", after);
		PointArray[ArrayCount].time = after - before;
		ArrayCount++;

		bNowDraw = TRUE;
		return 0;
	case WM_MOUSEMOVE:
		GetLocalTime(&oldSt);
		if (bNowDraw)
		{
			hdc = GetDC(hWnd);
			MyPen = CreatePen(PS_SOLID, PenWidth, Color);
			OldPen = (HPEN)SelectObject(hdc, MyPen);

			Old.x = LOWORD(lParam);
			Old.y = HIWORD(lParam);

			MoveToEx(hdc, Start.x, Start.y, NULL);


			Start.x = LOWORD(lParam);
			Start.y = HIWORD(lParam);

			PointArray[ArrayCount].pos = Start;
			PointArray[ArrayCount].Color = Color;
			PointArray[ArrayCount].Width = PenWidth;
			PointArray[ArrayCount].move = FALSE;
			after = oldSt.wHour * 3600 + oldSt.wMinute * 60 + oldSt.wSecond + oldSt.wMilliseconds * 0.001;
		
			//printf("저장시간 : %lf\n", after);
			PointArray[ArrayCount].time = after - before;

			LineTo(hdc, Old.x, Old.y);

			ArrayCount++;
			SelectObject(hdc, OldPen);
			DeleteObject(MyPen);
			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		bNowDraw = FALSE;

		return 0;
	case WM_RBUTTONDOWN:
		if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DrawSetDlgProc) == IDOK)
		{
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (bNowLineDraw)
		{
			for (int i = 0; i < ArrayCount - 1; i++)
			{
				if (PointArray[i].IsDraw)
				{
					//printf("Move? -> %d\n", PointArray[i].move);
					MyPen = CreatePen(PS_SOLID, PointArray[i].Width, PointArray[i].Color);
					OldPen = (HPEN)SelectObject(hdc, MyPen);
					if (PointArray[i].move == TRUE)
					{
						MoveToEx(hdc, PointArray[i].pos.x, PointArray[i].pos.y, NULL);
					}
					else
					{
						LineTo(hdc, PointArray[i].pos.x, PointArray[i].pos.y);
					}
					SelectObject(hdc, OldPen);
					DeleteObject(MyPen);
				}
			}
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		// closesocket()
		closesocket(sock);
		// 윈속 종료
		WSACleanup();
		PostQuitMessage(0);
		return 0;

	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawLine(HWND &hWnd, Line* pArray, int &count)
{
	HDC hdc = GetDC(hWnd);
	HPEN MyPen, OldPen;
	Line LoadArray[1000];
	int SaveCount = 0;

	FILE* file;
	file = fopen("Data.txt", "rt");
	GetLocalTime(&oldSt);

	for (int i = 0; !feof(file); i++)
	{
		fscanf(file, "%d %d %d %d %d %lf",
			&pArray[i].pos.x, &pArray[i].pos.y, &pArray[i].Color, &pArray[i].Width, &pArray[i].move, &pArray[i].time);
		SaveCount++;
	}
	count = SaveCount;
	fclose(file);

	ReleaseDC(hWnd, hdc);
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}