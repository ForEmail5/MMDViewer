#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <Windows.h>

#include "MathLib.h"
#include "Event\Event.h"

class RenderWindow
{
public:
	RenderWindow(int width, int height);

	~RenderWindow();

	Rect GetRenderRect();

	static HWND GetHwnd();

	static float GetAspect();

	int GetRenderWindowWidth() { return window_width; }

	int GetRenderWindowHeight() { return window_height; }

private:
	void Create();

	static LRESULT WINAPI WindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND hwnd;

	int render_width;
	int render_height;
	int window_width;
	int window_height;

	WinMsgHandle winmsg_handle;
};

#endif