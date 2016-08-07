// dx11bvh.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hWnd;
static App app;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

// WindowMessage
static BOOL ProcessWindowMessage(){
	MSG msg;
	for (;;){
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			if (msg.message == WM_QUIT){
				return FALSE;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		BOOL active = !IsIconic(hWnd) && GetForegroundWindow() == hWnd;

		if (!active){
			WaitMessage();
			continue;
		}

		return TRUE;
	}
}

#ifdef _DEBUG
int main(int, char**)
#else
int APIENTRY _tWinMain(_In_ HINSTANCE,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
#endif
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	GoMyDir();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DX11BVH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance))
	{
		return FALSE;
	}

	deviceMan11.Create(hWnd);

	app.Init(nullptr);

	// Main message loop:
	for (;;) {
		if (!ProcessWindowMessage()) {
			break;
		}
		app.Update();
		app.Draw();
		Sleep(1);
	}

	app.Destroy();
	texMan.Destroy();
	shaderMan.Destroy();
	bufferMan.Destroy();
	deviceMan11.Destroy();
	return 0;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DX11BVH);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance)
{
   hInst = hInstance; // Store instance handle in our global variable

   RECT r;
   SetRect(&r, 0, 0, SCR_W, SCR_H);
   AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, TRUE);
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_SHOWNORMAL);
   UpdateWindow(hWnd);

   DragAcceptFiles(hWnd, TRUE);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_RELOAD_SHADERS:
			shaderMan.Reload();
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			char fileName[MAX_PATH];
			DragQueryFileA(hDrop, 0, fileName, MAX_PATH);
			DragFinish(hDrop);
			app.Init(fileName);
			break;
		}
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		devCamera.LButtonDown(LOWORD(lParam) / (float)SCR_W, HIWORD(lParam) / (float)SCR_H);
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		devCamera.RButtonUp(LOWORD(lParam) / (float)SCR_W, HIWORD(lParam) / (float)SCR_H);
		break;
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		devCamera.RButtonDown(LOWORD(lParam) / (float)SCR_W, HIWORD(lParam) / (float)SCR_H);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		devCamera.LButtonUp(LOWORD(lParam) / (float)SCR_W, HIWORD(lParam) / (float)SCR_H);
		break;
	case WM_MOUSEMOVE:
		devCamera.MouseMove(MAKEPOINTS(lParam).x / (float)SCR_W, MAKEPOINTS(lParam).y / (float)SCR_H);
		break;
	case WM_MOUSEWHEEL:
		devCamera.MouseWheel((short)HIWORD(wParam) / (float)WHEEL_DELTA);
		break;
	case WM_SIZE:
		systemMisc.SetScreenSize(IVec2(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
