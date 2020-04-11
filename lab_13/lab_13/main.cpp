#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <windowsx.h>
#include "resource.h"

using namespace std;

LPSTR lpzMenuName;
HWND hMainWnd;

ATOM RegMainWindowClass(HINSTANCE hInst);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitControls(HWND hWnd, HINSTANCE hInst);

void ClearListBox(HWND listBox);
void ShowProcesses();
void ShowModules(PROCESSENTRY32 proc);
void SetPriority(DWORD priorityClass);

int width = 700;
int height = 700;

int procCount = 0;
PROCESSENTRY32 procs[500];

int moduleCount = 0;
MODULEENTRY32 modules[500];

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

	ShowProcesses();

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
#define HPROCESSES_ID 10001
#define HMODULES_ID 10002
HWND hProcesses, hModules;
HMENU hPopupMenu;

#define ID_IDLE 10
#define ID_NORMAL 11
#define ID_HIGH 12
#define ID_REAL_TIME 13

void SetPriority(DWORD priorityClass) 
{
	int itemId = SendMessage(hProcesses, LB_GETCURSEL, 0, NULL);
	if (itemId != -1) {
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procs[itemId].th32ProcessID);
		SetPriorityClass(hProcess, priorityClass);
		CloseHandle(hProcess);
	}
}

void ClearListBox(HWND listBox) 
{
	while (SendMessage(listBox, LB_GETCOUNT, 0, NULL))
		SendMessage(listBox, LB_DELETESTRING, 0, NULL);
}

void ShowProcesses()
{
	procCount = 0;
	procs[0].dwSize = sizeof(PROCESSENTRY32);

	HANDLE toolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bool result = Process32First(toolHelp, &procs[0]);
	while (result)
	{
		procCount++;
		procs[procCount].dwSize = sizeof(PROCESSENTRY32);
		result = Process32Next(toolHelp, &procs[procCount]);
	}
	CloseHandle(toolHelp);

	ClearListBox(hProcesses);
	for (int i = 0; i < procCount; i++)
		SendMessage(hProcesses, LB_ADDSTRING, 0, (LPARAM)procs[i].szExeFile);
}

void ShowModules(PROCESSENTRY32 proc)
{
	moduleCount = 0;
	modules[0].dwSize = sizeof(MODULEENTRY32);

	HANDLE toolHelp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, proc.th32ProcessID);
	bool result = Module32First(toolHelp, &modules[0]);
	while (result)
	{
		moduleCount++;
		modules[moduleCount].dwSize = sizeof(MODULEENTRY32);
		result = Module32Next(toolHelp, &modules[moduleCount]);
	}
	CloseHandle(toolHelp);

	ClearListBox(hModules);
	for (int i = 0; i < moduleCount; ++i)
		SendMessage(hModules, LB_ADDSTRING, 0, (LPARAM)modules[i].szModule);
}

void InitControls(HWND hWnd, HINSTANCE hInst)
{
	hProcesses = CreateWindow("LISTBOX", "Processes", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
		20, 20, 250, 500, hWnd, (HMENU)HPROCESSES_ID, hInst, NULL);
	hModules = CreateWindow("LISTBOX", "Modules", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
		300, 20, 250, 500, hWnd, (HMENU)HMODULES_ID, hInst, NULL);
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
		wmEvent = HIWORD(wParam); switch (wmId)
		{
		case HPROCESSES_ID:
			if (wmEvent == LBN_SELCHANGE)
			{
				int itemId = SendMessage(hProcesses, LB_GETCURSEL, 0, NULL);
				if (itemId != -1)
					ShowModules(procs[itemId]);
			}
			break;
		case ID_IDLE:
			SetPriority(IDLE_PRIORITY_CLASS);
			break;
		case ID_NORMAL:
			SetPriority(NORMAL_PRIORITY_CLASS);
			break;
		case ID_HIGH:
			SetPriority(HIGH_PRIORITY_CLASS);
			break;
		case ID_REAL_TIME:
			SetPriority(REALTIME_PRIORITY_CLASS);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CONTEXTMENU:
		if ((HWND)wParam == hProcesses) {
			int itemId = SendMessage(hProcesses, LB_GETCURSEL, 0, NULL);
			if (itemId != -1) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procs[itemId].th32ProcessID);
				int pC = GetPriorityClass(hProcess);
				CloseHandle(hProcess);

				hPopupMenu = CreatePopupMenu();
				AppendMenu(hPopupMenu, pC == IDLE_PRIORITY_CLASS ? MF_CHECKED : MF_STRING, ID_IDLE, "Idle");
				AppendMenu(hPopupMenu, pC == NORMAL_PRIORITY_CLASS ? MF_CHECKED : MF_STRING, ID_NORMAL, "Normal");
				AppendMenu(hPopupMenu, pC == HIGH_PRIORITY_CLASS ? MF_CHECKED : MF_STRING, ID_HIGH, "High");
				AppendMenu(hPopupMenu, pC == REALTIME_PRIORITY_CLASS ? MF_CHECKED : MF_STRING, ID_REAL_TIME, "Real time");
				TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hWnd, NULL);
			}
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
