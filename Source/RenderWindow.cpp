#include "RenderWindow.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_impl_dx11.h"

static RenderWindow* render_window = nullptr;

RenderWindow::RenderWindow(int width, int height)
{
	render_window = this;

	window_width = width;
	window_height = height;
	render_width = window_width - 300;
	render_height = window_height - 23;

	this->winmsg_handle.SetWindowHeight(height);
	this->Create();
}

RenderWindow::~RenderWindow()
{
	/* 注销窗口类 */
	UnregisterClassA("MMDViewer", ( HINSTANCE ) GetModuleHandle(0));
}

Rect RenderWindow::GetRenderRect()
{
	return Rect(window_width - render_width, window_height - render_height, render_width, render_height);
}

HWND RenderWindow::GetHwnd()
{
	return render_window->hwnd;
}

float RenderWindow::GetAspect()
{ 
	return float(render_window->render_height) / render_window->render_height;
}

void RenderWindow::Create()
{
	/* 设计窗口类 */
	WNDCLASS wndclass;
	memset(&wndclass, 0, sizeof(WNDCLASSA));

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc = ( WNDPROC ) WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = ( HINSTANCE ) GetModuleHandle(0);
	wndclass.hIcon = 0;
	wndclass.hCursor = 0;
	wndclass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = L"MMDViewer";

	/* 注册窗口类 */
	RegisterClass(&wndclass);

	/* 不能改变窗口大小 */
	int style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

	/* 根据客户区大小计算窗口大小 */
	RECT rect = { 0, 0, window_width, window_height };
	AdjustWindowRect(&rect, style, 0);

	/* 居中显示计算窗口位置和大小 */
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

	/* 创建窗口 */
	hwnd = CreateWindow(L"MMDViewer", L"MMDViewer", style, x, y, w, h, NULL, 0, ( HINSTANCE ) GetModuleHandle(0), 0);

	/* 显示窗口 */
	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);
}

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI RenderWindow::WindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplDX11_WndProcHandler(wnd, msg, wParam, lParam);

	/* 分派事件 */
	render_window->winmsg_handle.HandleMessage(wnd, msg, wParam, lParam);

	switch ( msg )
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		RECT rect;
		if ( GetUpdateRect(wnd, &rect, FALSE) )
		{
			ValidateRect(wnd, &rect);
		}
		break;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}