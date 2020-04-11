#include <iostream>
#include <windows.h>
#include "resource.h"


using namespace std;

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitControls(HWND hWnd, HINSTANCE hInst);

int width = 700;
int height = 500;

#define MODE 1 //change mode 0 - event; other - critical section
HANDLE thread1, thread2, thread3;
#if MODE == 0
	HANDLE hEvent1, hEvent2, hEvent3;
	DWORD WINAPI ChangeColor_1();
	DWORD WINAPI ChangeColor_2();
	DWORD WINAPI ChangeColor_3();
#else
	CRITICAL_SECTION cs;
	DWORD WINAPI ChangeColor(int* newColor);
#endif

int color = 0;

int color_1 = 0;
int color_2 = 1;
int color_3 = 2;
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
			"O¯Ë·Í‡", MB_OK);
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

	#if MODE == 0
		hEvent1 = CreateEvent(NULL, FALSE, TRUE, NULL);
		hEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
		hEvent3 = CreateEvent(NULL, FALSE, FALSE, NULL);
		thread1 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)ChangeColor_1, &color_1, 0, NULL);
		thread2 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)ChangeColor_2, &color_2, 0, NULL);
		thread3 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)ChangeColor_3, &color_3, 0, NULL);
	#else
		InitializeCriticalSection(&cs);
		thread1 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)ChangeColor, &color_1, 0, NULL);
		thread2 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)ChangeColor, &color_2, 0, NULL);
		thread3 = CreateThread(NULL, 0u, (LPTHREAD_START_ROUTINE)ChangeColor, &color_3, 0, NULL);
	#endif

	InitControls(hMainWnd, hInstance);


	MSG msg = { 0 };
	int iGetOk = 0;
	while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (iGetOk == -1) return 3;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	#if MODE != 0
		DeleteCriticalSection(&cs);
	#endif
	return msg.wParam;
}

void InitControls(HWND hWnd, HINSTANCE hInst)
{

}

#if MODE == 0
DWORD WINAPI ChangeColor_1()
{
	while (true)
	{
		WaitForSingleObject(hEvent1, INFINITE);
		color = 0;
		Sleep(1000);
		InvalidateRect(hMainWnd, NULL, TRUE);
		SetEvent(hEvent2);
	}
	return 0;
}

DWORD WINAPI ChangeColor_2()
{
	while (true)
	{
		WaitForSingleObject(hEvent2, INFINITE);
		color = 1;
		Sleep(1000);
		InvalidateRect(hMainWnd, NULL, TRUE);
		SetEvent(hEvent3);
	}
	return 0;
}

DWORD WINAPI ChangeColor_3()
{
	while (true)
	{
		WaitForSingleObject(hEvent3, INFINITE);
		color = 2;
		Sleep(1000);
		InvalidateRect(hMainWnd, NULL, TRUE);
		SetEvent(hEvent1);
	}
	return 0;
}
#else
DWORD WINAPI ChangeColor(int* newColor)
{
	while (true)
	{
		if (color != *newColor)
		{
			if (TryEnterCriticalSection(&cs))
			{
				color = *newColor;
				Sleep(1000);
				InvalidateRect(hMainWnd, NULL, TRUE);
				LeaveCriticalSection(&cs);
			}
		}
	}
	return 0;
}
#endif

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

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
		switch (color)
		{
		case 0:
			hBrush = CreateSolidBrush(RGB(0, 0, 255));
			break;
		case 1:
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			break;
		case 2:
			hBrush = CreateSolidBrush(RGB(0, 255, 0));
			break;
		default:
			break;
		}
		SelectObject(hdc, hBrush);
		Rectangle(hdc, -1, -1, width + 1, height + 1);
		DeleteObject(hBrush);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_TIMER:
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
	//wc.hCursor = LoadCursor(wc.hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wc.hCursor = LoadCursor(wc.hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	wc.lpszMenuName = lpzMenuName;
	wc.lpszClassName = "MainWndClass";
	wc.hIconSm = wc.hIcon;

	return RegisterClassEx(&wc);
}
