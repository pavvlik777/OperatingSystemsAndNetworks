#include <iostream>
#include <string.h>
#include <windows.h>
#include "resource.h"

using namespace std;

#define BUTTON_START 1
#define BUTTON_END 2

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitControls(HWND hWnd, HINSTANCE hInst);

int width = 700;
int height = 500;

/////////////////////////

DWORD WINAPI MoveText_1();
DWORD WINAPI MoveText_2();
DWORD WINAPI MoveText_3();

/////////////////////////

HANDLE thread1, thread2, thread3;
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

	InitControls(hMainWnd, hInstance);

	thread1 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveText_1, NULL, 0, NULL);
	SuspendThread(thread1);

	thread2 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveText_2, NULL, 0, NULL);
	SuspendThread(thread2);

	thread3 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)MoveText_3, NULL, 0, NULL);
	SuspendThread(thread3);

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

HWND hStart, hStop;
void InitControls(HWND hWnd, HINSTANCE hInst)
{
	hStart = CreateWindow("BUTTON", "Start", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		340, 40, 80, 30, hWnd, (HMENU)BUTTON_START, hInst, NULL);

	hStop = CreateWindow("BUTTON", "Stop", WS_TABSTOP | WS_CHILD | WS_VISIBLE,
		190, 80, 80, 30, hWnd, (HMENU)BUTTON_END, hInst, NULL);
}

TCHAR text_1[] = "Text 1";
TCHAR text_2[] = "Text 2";
TCHAR text_3[] = "Text 3";

int x_1 = 0;
int y_1 = 0;
int x_2 = 200;
int y_2 = 100;
int x_3 = 100;
int y_3 = 200;

#define TIMER_ID 1
#define TIMER_DELAY 20

bool animStarted = false;

DWORD WINAPI MoveText_1()
{
	while (true)
	{
		x_1 = (x_1 + 3) % width;
		y_1 = (y_1 + 4) % height;
		Sleep(TIMER_DELAY);
	}
	return 0;
}

DWORD WINAPI MoveText_2()
{
	while (true)
	{
		x_2 = (x_2 - 6) % width;
		if (x_2 < 0)
			x_2 = width;
		y_2 = (y_2 + 5) % height;
		Sleep(TIMER_DELAY);
	}
	return 0;
}

DWORD WINAPI MoveText_3()
{
	while (true)
	{
		x_3 = (x_3 + 7) % width;
		y_3 = (y_3 + 0) % height;
		Sleep(TIMER_DELAY);
	}
	return 0;
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
		switch (wmEvent)
		{
		case BN_CLICKED:
			switch (wmId)
			{
			case BUTTON_START:
				if (!animStarted)
				{
					ResumeThread(thread1);
					ResumeThread(thread2);
					ResumeThread(thread3);
					SetTimer(hWnd, TIMER_ID, TIMER_DELAY, NULL);
					animStarted = true;
				}
				break;
			case BUTTON_END:
				if (animStarted)
				{
					SuspendThread(thread1);
					SuspendThread(thread2);
					SuspendThread(thread3);
					KillTimer(hWnd, TIMER_ID);
					animStarted = false;
				}
				break;
			default:
				break;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		break;
	case WM_TIMER:
		if (animStarted)
		{
			InvalidateRect(hWnd, NULL, TRUE);
			SetTimer(hWnd, TIMER_ID, TIMER_DELAY, NULL);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		for (int i = 0; i < lstrlen(text_1); ++i) {
			TextOut(hdc, x_1 + i * 20, y_1, &text_1[i], 1);
		}
		for (int i = 0; i < lstrlen(text_2); ++i) {
			TextOut(hdc, x_2 + i * 20, y_2, &text_2[i], 1);
		}
		for (int i = 0; i < lstrlen(text_3); ++i) {
			TextOut(hdc, x_3 + i * 20, y_3, &text_3[i], 1);
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
