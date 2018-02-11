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
	/* �������� */
	RenderWindow render_window(1000, 600);

	/* ���� D3D ͼ�������� */
	D3DGraphicsContext graphics_context(&render_window);

	/* ���� MMD ����� */
	MMDViewer viewer(&graphics_context);

	GUI gui(&viewer, &render_window, &graphics_context);

	/* ��ȡ�����ڲ���ʱ����ʱ��Ƶ�� */
	//LARGE_INTEGER perf_frequency_int64;
	//QueryPerformanceFrequency(&perf_frequency_int64);
	//double perf_frequency = ( double ) perf_frequency_int64.QuadPart;

	//LARGE_INTEGER last_frame, now_frame;
	//QueryPerformanceCounter(&last_frame);

	//double frame_interval = 0;


	/* ��Ϣѭ�� */
	MSG msg = { 0 };
	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* ���������¼� */
		EventDispatcher::GetInstance()->FlushEvent();

		/* ֡���ʱ�� */
		//QueryPerformanceCounter(&now_frame);
		//frame_interval = ((now_frame.QuadPart - last_frame.QuadPart) * 1000.0f) / perf_frequency;
		//last_frame = now_frame;

		/* ����ǰ��պ󻺳� */
		graphics_context.Clear();

		/* ��ʼ MMDViewer ���� */
		viewer.Update(1000.0f / ImGui::GetIO().Framerate);
		viewer.Render();

		/* ��ʼ������� */
		gui.BeginDraw();
		gui.Draw();

		graphics_context.Flush();
	}
	return 0;
}