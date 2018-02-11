#pragma once
#include <Windows.h>

#include "GUI.h"
#include "MMDViewer.h"
#include "RenderWindow.h"
#include "D3DGraphicsContext.h"
#include "Event\EventDispatcher.h"

#include "ImGUI\imgui.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/* 创建窗口 */
	RenderWindow render_window(1000, 600);

	/* 创建 D3D 图形上下文 */
	D3DGraphicsContext graphics_context(&render_window);

	/* 创建 MMD 浏览器 */
	MMDViewer viewer(&graphics_context);

	GUI gui(&viewer, &render_window, &graphics_context);

	/* 获取机器内部计时器的时钟频率 */
	//LARGE_INTEGER perf_frequency_int64;
	//QueryPerformanceFrequency(&perf_frequency_int64);
	//double perf_frequency = ( double ) perf_frequency_int64.QuadPart;

	//LARGE_INTEGER last_frame, now_frame;
	//QueryPerformanceCounter(&last_frame);

	//double frame_interval = 0;


	/* 消息循环 */
	MSG msg = { 0 };
	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* 分派所有事件 */
		EventDispatcher::GetInstance()->FlushEvent();

		/* 帧间隔时间 */
		//QueryPerformanceCounter(&now_frame);
		//frame_interval = ((now_frame.QuadPart - last_frame.QuadPart) * 1000.0f) / perf_frequency;
		//last_frame = now_frame;

		/* 绘制前清空后缓冲 */
		graphics_context.Clear();

		/* 开始 MMDViewer 绘制 */
		viewer.Update(1000.0f / ImGui::GetIO().Framerate);
		viewer.Render();

		/* 开始界面绘制 */
		gui.BeginDraw();
		gui.Draw();

		graphics_context.Flush();
	}
	return 0;
}