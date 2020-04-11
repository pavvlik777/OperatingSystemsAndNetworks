#include <iostream>
#include <string.h>
#include <windows.h>
#include "resource.h"

using namespace std;

LPSTR lpzMenuName;
HWND hMainWnd;

HWND hSecondWindow;

void InitControls(HWND hWnd, HINSTANCE hInstance);
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
			"Îøèáêà", MB_OK);
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

	hSecondWindow = FindWindow(NULL, "Window_2");

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

#define RADIO_BUTTON_1_1_ID 1011
#define RADIO_BUTTON_1_2_ID 1012
#define RADIO_BUTTON_1_3_ID 1013

#define RADIO_BUTTON_2_1_ID 1021
#define RADIO_BUTTON_2_2_ID 1022
#define RADIO_BUTTON_2_3_ID 1023
#define RADIO_BUTTON_2_4_ID 1024

#define CHECKBOX_ID 1001

#define DRAW_MESSAGE "DRAWMESSAGE"
#define COLOR_MESSAGE "COLORMESSAGE"
#define SHAPE_MESSAGE "SHAPEMESSAGE"

UINT WM_DRAW;
UINT WM_COLOR;
UINT WM_SHAPE;

HWND hRadioBtn_1_1, hRadioBtn_1_2, hRadioBtn_1_3, hRadioBtn_2_1, hRadioBtn_2_2, hRadioBtn_2_3, hRadioBtn_2_4, hCheckbox;
void InitControls(HWND hWnd, HINSTANCE hInstance)
{
	hRadioBtn_1_1 = CreateWindow("button", "Red", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 10, 80, 30, hWnd, (HMENU)RADIO_BUTTON_1_1_ID, hInstance, NULL);
	hRadioBtn_1_2 = CreateWindow("button", "Green", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 35, 80, 30, hWnd, (HMENU)RADIO_BUTTON_1_2_ID, hInstance, NULL);
	hRadioBtn_1_3 = CreateWindow("button", "Blue", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 60, 80, 30, hWnd, (HMENU)RADIO_BUTTON_1_3_ID, hInstance, NULL);

	hRadioBtn_2_1 = CreateWindow("button", "Rhomb", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 110, 80, 30, hWnd, (HMENU)RADIO_BUTTON_2_1_ID, hInstance, NULL);
	hRadioBtn_2_2 = CreateWindow("button", "Square", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 135, 80, 30, hWnd, (HMENU)RADIO_BUTTON_2_2_ID, hInstance, NULL);
	hRadioBtn_2_3 = CreateWindow("button", "Circle", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 160, 80, 30, hWnd, (HMENU)RADIO_BUTTON_2_3_ID, hInstance, NULL);
	hRadioBtn_2_4 = CreateWindow("button", "Star", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		10, 185, 80, 30, hWnd, (HMENU)RADIO_BUTTON_2_4_ID, hInstance, NULL);

	CheckRadioButton(hWnd, RADIO_BUTTON_1_1_ID, RADIO_BUTTON_1_3_ID, RADIO_BUTTON_1_1_ID);
	CheckRadioButton(hWnd, RADIO_BUTTON_2_1_ID, RADIO_BUTTON_2_4_ID, RADIO_BUTTON_2_1_ID);

	hCheckbox = CreateWindow("button", "Draw",
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		100, 10, 80, 30, hWnd, (HMENU)CHECKBOX_ID, hInstance, NULL);

	WM_DRAW = RegisterWindowMessage(DRAW_MESSAGE);
	WM_COLOR = RegisterWindowMessage(COLOR_MESSAGE);
	WM_SHAPE = RegisterWindowMessage(SHAPE_MESSAGE);
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
		if (wmId >= RADIO_BUTTON_1_1_ID && wmId <= RADIO_BUTTON_1_3_ID) {
			hSecondWindow = FindWindow(NULL, "Window_2");
			if(hSecondWindow != NULL)
				SendMessage(hSecondWindow, WM_COLOR, 0, wmId - RADIO_BUTTON_1_1_ID);

			CheckRadioButton(hWnd, RADIO_BUTTON_1_1_ID, RADIO_BUTTON_1_3_ID, wmId);
		}
		else if (wmId >= RADIO_BUTTON_2_1_ID && wmId <= RADIO_BUTTON_2_4_ID) {
			hSecondWindow = FindWindow(NULL, "Window_2");
			if (hSecondWindow != NULL)
				SendMessage(hSecondWindow, WM_SHAPE, 0, wmId - RADIO_BUTTON_2_1_ID);

			CheckRadioButton(hWnd, RADIO_BUTTON_2_1_ID, RADIO_BUTTON_2_4_ID, wmId);
		}
		else if (wmId == CHECKBOX_ID)
		{
			hSecondWindow = FindWindow(NULL, "Window_2");
			if (hSecondWindow != NULL)
				SendMessage(hSecondWindow, WM_DRAW, 0, 0);
			LRESULT status = SendMessage(hCheckbox, BM_GETCHECK, 0, 0);
			if(status == BST_CHECKED)
			{
				hSecondWindow = FindWindow(NULL, "Window_2");
				if (hSecondWindow != NULL)
					SendMessage(hSecondWindow, WM_DRAW, 0, 1);
			}
			else if (status == BST_UNCHECKED)
			{
				hSecondWindow = FindWindow(NULL, "Window_2");
				if (hSecondWindow != NULL)
					SendMessage(hSecondWindow, WM_DRAW, 0, 0);
			}
		}
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
