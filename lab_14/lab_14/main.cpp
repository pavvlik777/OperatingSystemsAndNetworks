#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "resource.h"

using namespace std;

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitControls(HWND hWnd, HINSTANCE hInst);
void Search(TCHAR* s);
void SearchRecord(HKEY hKeyRoot, LPTSTR lpSubKey, TCHAR* s, int level);

int width = 700;
int height = 650;

#define EDIT_ID 10001
#define BUTTON_SEARCH_ID 10002
#define LISTBOX_ID 10003

HWND hEdit, hButtonSearch, hListBox;

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

void InitControls(HWND hWnd, HINSTANCE hInst)
{
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "Text", WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		40, 40, 200, 20, hWnd, (HMENU)EDIT_ID, hInst, NULL);

	hButtonSearch = CreateWindow("button", "Search", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		260, 35, 80, 30, hWnd, (HMENU)BUTTON_SEARCH_ID, hInst, NULL);

	hListBox = CreateWindowEx(WS_EX_CLIENTEDGE, "listbox", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
		40, 80, 600, 500, hWnd, (HMENU)LISTBOX_ID, hInst, NULL);
}

void SearchRecord(HKEY hKeyRoot, LPTSTR lpSubKey, TCHAR* s, int level)
{
	if (level <= 0) return;
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS)
	{
		return;
	}

	if (_tcsstr(lpSubKey, s) != NULL)
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)lpSubKey);

	lpEnd = lpSubKey + lstrlen(lpSubKey);

	if (*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd = TEXT('\\');
		lpEnd++;
		*lpEnd = TEXT('\0');
	}

	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);

	if (lResult == ERROR_SUCCESS)
	{
		int index = 0;
		do {
			index++;

			StringCchCopy(lpEnd, MAX_PATH * 2, szName);
			SearchRecord(hKeyRoot, lpSubKey, s, level - 1);

			dwSize = MAX_PATH;
			lResult = RegEnumKeyEx(hKey, index, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
		} while (lResult == ERROR_SUCCESS);
	}
	RegCloseKey(hKey);
}

void Search(TCHAR* s) 
{
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);

	TCHAR szSearchKey[MAX_PATH * 2];
	StringCchCopy(szSearchKey, MAX_PATH * 2, ".Default");
	SearchRecord(HKEY_USERS, szSearchKey, s, 5);
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
		switch (wmId)
		{
		case BUTTON_SEARCH_ID:
		{
			int len;
			len = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
			TCHAR buffer[256];
			SendMessage(hEdit, WM_GETTEXT, len + 1, (LPARAM)buffer);
			Search(buffer);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
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
