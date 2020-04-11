#include <iostream>
#include <windows.h>
#include "resource.h"

using namespace std;

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int width = 700;
int height = 500;

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
			"Ошибка", MB_OK);
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

	HMENU hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, 1, "Start");
	AppendMenu(hMenu, MF_STRING, 2, "Stop");
	SetMenu(hMainWnd, hMenu);

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

TCHAR text[] = "Animation";

#define TIMER_ID 1
#define TIMER_DELAY 20

int x = 0;
bool isInit = false;
bool animStarted = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1: //Start
			isInit = true;
			if (!animStarted) {
				SetTimer(hWnd, TIMER_ID, TIMER_DELAY, NULL);
				animStarted = true;
			}
			break;
		case 2://Stop
			if (animStarted) {
				KillTimer(hWnd, TIMER_ID);
				animStarted = false;
			}
			break;
		}
		break;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_PAINT:
		if (isInit)
		{
			hdc = BeginPaint(hWnd, &ps);
			for (int i = 0; i < lstrlen(text); ++i) {
				TextOut(hdc, x + i * 20, height / 2, &text[i], 1);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_TIMER:
		x = (x + 3) % width;
		InvalidateRect(hWnd, NULL, TRUE);
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
	wc.hCursor = LoadCursor(wc.hInstance, MAKEINTRESOURCE(IDC_POINTER));
	wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	wc.lpszMenuName = lpzMenuName;
	wc.lpszClassName = "MainWndClass"; 
	wc.hIconSm = wc.hIcon;

	return RegisterClassEx(&wc);
}
