#ifndef GUI_H
#define GUI_H

#include "Renderer.h"
#include "RenderWindow.h"
#include "D3DGraphicsContext.h"

#include "MMD\FileReader.h"

#include "ImGUI\imgui.h"
#include "ImGUI\imgui_impl_dx11.h"

class MMDViewer;

class GUI
{
public:
	GUI(MMDViewer* viewer, RenderWindow* window, D3DGraphicsContext* gc);

	~GUI();

	void InitGUIFont();

	//------------------------------------- GUI -------------------------------
	void DrawDemoWindow();

	void DrawRightWindow();

	void BeginDraw();

	void Draw();

	//------------------------------------- function -------------------------------

private:
	MMDViewer* mmd_viewer;
	RenderWindow* window;
	D3DGraphicsContext* gc;

	std::string str_null;
	std::string str_play;
	std::string str_stop;
	std::string str_model;
	std::string str_bone_motion;
	std::string str_camera_motion;
};

#endif