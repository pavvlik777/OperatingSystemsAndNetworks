#include <iostream>
#include <string.h>
#include <windows.h>
#include "resource.h"

using namespace std;

#define BUTTON_DRAW_ID 1
#define BUTTON_CLEAR_ID 2

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitControls(HWND hWnd, HINSTANCE hInst);

int width = 700;
int height = 500;

/////////////////////////

HANDLE bmp[4];
int currBmp = 0;

int currentX = 0, currentY = 0, targetX = 0, targetY = 0;
int orientation = 1;

#define STEP 5

/////////////////////////

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nCommandShow)
{
	if (!RegMainWindowClass(hInstance))
	{
		LPSTR errorMessage;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0,
			(LPSTR)&errorMessage,
			16,
			NULL);
		MessageBox(NULL,
			errorMessage,
			"Žøèáêà", MB_OK);
		return NULL;
	}

	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect);
	int x = screen_rect.right / 2 - width / 2;
	int y = screen_rect.bottom / 2 - height / 2;

	hMainWnd = CreateWindow("MainWndClass", "Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, width, height, NULL, NULL,
		hInstance, NULL);
	if (!hMainWnd) return 2;

	bmp[0] = LoadImage(NULL, "0.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE); //400x400
	bmp[1] = LoadImage(NULL, "1.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
	bmp[2] = LoadImage(NULL, "2.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
	bmp[3] = LoadImage(NULL, "3.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);

	MSG msg = { 0 };
	int iGetOk = 0;
	while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (iGetOk == -1) return 3;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		break;
	case WM_LBUTTONDOWN:
		targetX = LOWORD(lParam) - 50;
		targetY = HIWORD(lParam) - 50;
		SetTimer(hWnd, NULL, 16, NULL);
		break;
	case WM_TIMER:
	{
		bool needRedraw = false;
		if (abs(currentX - targetX) >= STEP) {
			needRedraw = true;
			if (targetX > currentX) {
				currentX += STEP;
				orientation = 1;
			}
			else {
				currentX -= STEP;
				orientation = -1;
			}
		}
		if (abs(currentY - targetY) >= STEP) {
			needRedraw = true;
			if (targetY > currentY)
				currentY += STEP;
			else
				currentY -= STEP;
		}
		if (needRedraw) {
			currBmp = (currBmp + 1) % 4;
			InvalidateRect(hWnd, NULL, true);
			SetTimer(hWnd, NULL, 16, NULL);
		}
	}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		HDC hCompatibleDC;
		hCompatibleDC = CreateCompatibleDC(hdc);
		SelectObject(hCompatibleDC, bmp[currBmp]);
		if (orientation == 1)
			StretchBlt(hdc, currentX, currentY, 100, 100, hCompatibleDC, 0, 0, 100, 100, SRCCOPY);
		else
			StretchBlt(hdc, currentX, currentY, 100, 100, hCompatibleDC, 100, 0, -100, 100, SRCCOPY);
		DeleteDC(hCompatibleDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM RegMainWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	//wc.hCursor = LoadCursor(wc.hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wc.hCursor = LoadCursor(wc.hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	wc.lpszMenuName = lpzMenuName;
	wc.lpszClassName = "MainWndClass";
	wc.hIconSm = wc.hIcon;

	return RegisterClassEx(&wc);
}
