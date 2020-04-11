#include <iostream>
#include <string.h>
#include <windows.h>
#include "resource.h"

using namespace std;

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int width = 700;
int height = 500;

#define DRAW_MESSAGE "DRAWMESSAGE"
#define COLOR_MESSAGE "COLORMESSAGE"
#define SHAPE_MESSAGE "SHAPEMESSAGE"

UINT WM_DRAW;
UINT WM_COLOR;
UINT WM_SHAPE;

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
			"Œ¯Ë·Í‡", MB_OK);
		return NULL;
	}

	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect);
	int x = screen_rect.right / 2 - width / 2;
	int y = screen_rect.bottom / 2 - height / 2;

	hMainWnd = CreateWindow("MainWndClass", "Window_2",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, width, height, NULL, NULL,
		hInstance, NULL);
	if (!hMainWnd) return 2;

	WM_DRAW = RegisterWindowMessage(DRAW_MESSAGE);
	WM_COLOR = RegisterWindowMessage(COLOR_MESSAGE);
	WM_SHAPE = RegisterWindowMessage(SHAPE_MESSAGE);

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

bool draw = false;
int color = 0, shape = 0;
int tempColor = 0, tempShape = 0;
int drawX = -1, drawY = -1;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	if (message == WM_DRAW) {
		draw = lParam;
	}
	else if (message == WM_COLOR) {
			tempColor = lParam;
			InvalidateRect(hWnd, NULL, true);
		}
	else if (message == WM_SHAPE) {
			tempShape = lParam;
			InvalidateRect(hWnd, NULL, true);
		}
	else
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		break;
	case WM_LBUTTONDOWN:
		drawX = LOWORD(lParam);
		drawY = HIWORD(lParam);
		color = tempColor;
		shape = tempShape;
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (draw && drawX != -1 && drawY != -1) {
			HBRUSH brush = NULL;
			switch (color)
			{
			case 0:
				brush = CreateSolidBrush(RGB(255, 0, 0));
				break;
			case 1:
				brush = CreateSolidBrush(RGB(0, 255, 0));
				break;
			case 2:
				brush = CreateSolidBrush(RGB(0, 0, 255));
				break;
			default:
				brush = CreateSolidBrush(RGB(0, 0, 255));
				break;
			}
			SelectObject(hdc, brush);

			switch (shape)
			{
			case 0:
			{
				POINT p[4] = { drawX + 20, drawY + 0, drawX + 40, drawY + 10,
					drawX + 20, drawY + 20, drawX + 0, drawY + 10 };
				Polygon(hdc, p, 4);
			}
			break;
			case 1:
				Rectangle(hdc, drawX, drawY, drawX + 40, drawY + 40);
				break;
			case 2:
				Ellipse(hdc, drawX, drawY, drawX + 40, drawY + 20);
				break;
			case 3:
			{
				POINT p[10] = { 15, 10, 20, 3, 25, 10, 35, 13,
								27, 18, 30, 25, 20, 20, 10, 25,
								13, 17, 5, 13 };
				for (int i = 0; i < 10; i++) {
					p[i].x += drawX;
					p[i].y += drawY;
				}
				Polygon(hdc, p, 10);
			}
			break;
			default:
				break;
			}
		}
		EndPaint(hWnd, &ps);
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
