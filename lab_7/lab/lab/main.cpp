#include <iostream>
#include <string.h>
#include <windows.h>
#include "resource.h"

using namespace std;

#define EDIT_ID 1
#define BUTTON_ADD_ID 2
#define BUTTON_CLEAR_ID 3
#define BUTTON_TORIGHT_ID 4
#define BUTTON_DELETE_ID 5
#define LISTBOX1_ID 6
#define LISTBOX2_ID 7

LPSTR lpzMenuName;
HWND hMainWnd;

bool DeleteSelectedItem(HWND hWnd, HWND hListBox);
void ToRight(HWND hWnd);
bool ContainsString(HWND hListBox, TCHAR* s);
void UpdateListBoxScroll(HWND hWnd, HWND hListBox, int itemId);
void AddStringInListBox(HWND hWnd, HWND hListBox, TCHAR* s);
ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitControls(HWND hWnd, HINSTANCE hInst);

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

HWND hEdit, hButtonAdd, hButtonClear, hButtonToRight, hButtonDelete, hListBox1, hListBox2;
void InitControls(HWND hWnd, HINSTANCE hInst)
{
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Edit", WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		40, 40, 280, 30, hWnd, (HMENU)EDIT_ID, hInst, NULL);

	hButtonAdd = CreateWindow("BUTTON", "Add", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		340, 40, 80, 30, hWnd, (HMENU)BUTTON_ADD_ID, hInst, NULL);

	hButtonClear = CreateWindow("BUTTON", "Clear", WS_TABSTOP | WS_CHILD | WS_VISIBLE,
		190, 80, 80, 30, hWnd, (HMENU)BUTTON_CLEAR_ID, hInst, NULL);

	hButtonToRight = CreateWindow("BUTTON", "ToRight", WS_TABSTOP | WS_CHILD | WS_VISIBLE,
		190, 145, 80, 30, hWnd, (HMENU)BUTTON_TORIGHT_ID, hInst, NULL);

	hButtonDelete = CreateWindow("BUTTON", "Delete", WS_TABSTOP | WS_CHILD | WS_VISIBLE,
		190, 210, 80, 30, hWnd, (HMENU)BUTTON_DELETE_ID, hInst, NULL);

	hListBox1 = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | LBS_MULTIPLESEL,
		40, 80, 140, 160, hWnd, (HMENU)LISTBOX1_ID, hInst, NULL);

	hListBox2 = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | LBS_MULTIPLESEL,
		280, 80, 140, 160, hWnd, (HMENU)LISTBOX2_ID, hInst, NULL);

	SendDlgItemMessage(hWnd, EDIT_ID, (UINT)LB_SETHORIZONTALEXTENT, (WPARAM)1024, (LPARAM)0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
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
				case BUTTON_ADD_ID:
					int length;
					length = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
					if (length == 0)
						MessageBox(hWnd, "Empty input", "Error", MB_OK);
					else {
						TCHAR* buffer = new TCHAR[length];
						SendMessage(hEdit, WM_GETTEXT, length + 1, (LPARAM)buffer);

						AddStringInListBox(hWnd, hListBox1, buffer);
						SetWindowText(GetDlgItem(hWnd, EDIT_ID), "");
					}
					break;
				case BUTTON_CLEAR_ID:
					SendMessage(hListBox1, LB_RESETCONTENT, 0, 0);
					SendMessage(hListBox2, LB_RESETCONTENT, 0, 0);
					UpdateListBoxScroll(hWnd, hListBox1, -1);
					UpdateListBoxScroll(hWnd, hListBox2, -1);
					break;
				case BUTTON_TORIGHT_ID:
					ToRight(hWnd);
					break;
				case BUTTON_DELETE_ID:
					DeleteSelectedItem(hWnd, hListBox1);
					DeleteSelectedItem(hWnd, hListBox2);
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

bool DeleteSelectedItem(HWND hWnd, HWND hListBox) 
{
	int selCount = SendMessage(hListBox, LB_GETSELCOUNT, 0, 0);
	int countBuffer[100];
	SendMessage(hListBox, LB_GETSELITEMS, 100, (LPARAM)countBuffer);

	for (int i = selCount - 1; i >= 0; --i) {
		int itemId = countBuffer[i];
		SendMessage(hListBox, LB_DELETESTRING, itemId, 0);
	}

	UpdateListBoxScroll(hWnd, hListBox, -1);
	return true;
}

void ToRight(HWND hWnd) 
{
	int selCount = SendMessage(hListBox1, LB_GETSELCOUNT, 0, 0);
	int countBuffer[100];
	SendMessage(hListBox1, LB_GETSELITEMS, 100, (LPARAM)countBuffer);

	if (selCount == 0) {
		MessageBox(hWnd, "Select item in first ListBox", "Error", MB_OK);
	}
	else {
		for (int i = selCount - 1; i >= 0; --i) {
			int itemId = countBuffer[i];
			TCHAR buffer[256];
			SendMessage(hListBox1, LB_GETTEXT, itemId, (LPARAM)buffer);
			AddStringInListBox(hWnd, hListBox2, buffer);
		}
	}
}

bool ContainsString(HWND hListBox, TCHAR* s) 
{
	int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		TCHAR buffer[256];
		SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)buffer);
		if (!strcmp(s, buffer))
			return true;
	}
	return false;
}

void UpdateListBoxScroll(HWND hWnd, HWND hListBox, int itemId) 
{
	int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
	if (count == 0) SendDlgItemMessage(hWnd, hListBox == hListBox1 ? LISTBOX1_ID : LISTBOX2_ID, (UINT)LB_SETHORIZONTALEXTENT, (WPARAM)0, (LPARAM)0);
	TCHAR buffer[256];

	for (int i = 0; i < count; i++) {
		if (i == itemId) continue;
		SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)buffer);
		int SizeOfHBListBox = strlen(buffer) * 8;
		SendDlgItemMessage(hWnd, hListBox == hListBox1 ? LISTBOX1_ID : LISTBOX2_ID, (UINT)LB_SETHORIZONTALEXTENT, (WPARAM)SizeOfHBListBox, (LPARAM)0);
	}
}

void AddStringInListBox(HWND hWnd, HWND hListBox, TCHAR* s)
{
	if (!ContainsString(hListBox, s)) {
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)s);
		UpdateListBoxScroll(hWnd, hListBox, -1);
	}
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
