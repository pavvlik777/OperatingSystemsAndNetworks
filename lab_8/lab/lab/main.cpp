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
int height = 700;

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
			"øèáêà", MB_OK);
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

	InitControls(hMainWnd, hInstance);

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

HWND hButtonDraw, hButtonClear;
void InitControls(HWND hWnd, HINSTANCE hInst)
{
	hButtonDraw = CreateWindow("BUTTON", NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		40, 40, 60, 60, hWnd, (HMENU)BUTTON_DRAW_ID, hInst, NULL);

	hButtonClear = CreateWindow("BUTTON", NULL, BS_OWNERDRAW | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		160, 40, 60, 60, hWnd, (HMENU)BUTTON_CLEAR_ID, hInst, NULL);
}

bool isPictureVisible = false;
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
		switch (wmId)
		{
		case BUTTON_DRAW_ID:
			isPictureVisible = true;
			InvalidateRect(hWnd, NULL, true);
			break;
		case BUTTON_CLEAR_ID:
			isPictureVisible = false;
			InvalidateRect(hWnd, NULL, true);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DRAWITEM:
	{
		DRAWITEMSTRUCT* draw = (DRAWITEMSTRUCT*)lParam;
		HBRUSH brush;
		HPEN pen;

		switch (draw->CtlID)
		{
		case BUTTON_DRAW_ID:
		{
			HDC hdc = draw->hDC;
			if (draw->itemAction == ODA_SELECT) {
				brush = CreateSolidBrush(RGB(0, 0, 255));
			}
			else {
				brush = CreateSolidBrush(RGB(0, 255, 0));
			}
			pen = CreatePen(BS_SOLID, 2, RGB(0, 0, 0));

			SelectObject(hdc, brush);
			SelectObject(hdc, pen);

			Ellipse(hdc, 0, 0, 60, 60);
			MoveToEx(hdc, 13, 31, NULL);
			LineTo(hdc, 24, 43);
			LineTo(hdc, 49, 19);
		}
		break;
		case BUTTON_CLEAR_ID:
		{
			HDC hdc = draw->hDC;
			if (draw->itemAction == ODA_SELECT) {
				brush = CreateSolidBrush(RGB(0, 0, 255));
			}
			else {
				brush = CreateSolidBrush(RGB(255, 0, 0));
			}
			pen = CreatePen(BS_SOLID, 2, RGB(0, 0, 0));

			SelectObject(hdc, brush);
			SelectObject(hdc, pen);

			Ellipse(hdc, 0, 0, 60, 60);
			MoveToEx(hdc, 20, 20, NULL);
			LineTo(hdc, 40, 40);
			MoveToEx(hdc, 40, 20, NULL);
			LineTo(hdc, 20, 40);
		}
		break;
		default:
			break;
		}
		break;
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		HBRUSH brush;
		HPEN pen;
		if (isPictureVisible)
		{
			brush = CreateSolidBrush(RGB(50, 50, 50));
			SelectObject(hdc, brush);

			Ellipse(hdc, 160, 360, 280, 480);
			Ellipse(hdc, 400, 360, 520, 480);

			brush = CreateSolidBrush(RGB(255, 133, 0));
			SelectObject(hdc, brush);
			pen = CreatePen(BS_SOLID, 2, RGB(0, 0, 0));
			SelectObject(hdc, pen);

			POINT p[16] = { 290, 420, 
				390, 420, 400, 350, 510, 350, 530, 420, 
				540, 420, 540, 300, 400, 290,
				380, 150, 
				300, 150, 
				280, 290, 140, 300, 140, 420,
				150, 420, 170, 350, 280, 350
				 };
			Polygon(hdc, p, 16);

			pen = CreatePen(BS_SOLID, 1, RGB(0, 0, 0));
			SelectObject(hdc, pen);
			brush = CreateSolidBrush(RGB(149, 184, 242));
			SelectObject(hdc, brush);

			POINT p2[3] = {350, 170, 350, 285, 385, 285};
			Polygon(hdc, p2, 3);

			POINT p3[3] = { 330, 170, 330, 285, 295, 285 };
			Polygon(hdc, p3, 3);

			pen = CreatePen(BS_SOLID, 2, RGB(0, 0, 0));
			SelectObject(hdc, pen);
			MoveToEx(hdc, 340, 150, NULL);
			LineTo(hdc, 340, 420);
		}
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
