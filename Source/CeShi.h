#pragma once

#include "MathLib.h"
#include "Event\Event.h"

class Model;
class Renderer;
class ModelBone;

class CeShi : public MouseEventListener
{
public:
	static CeShi* GetInstance()
	{
		static CeShi cs;
		return &cs;
	}

	void ResetDraw();

	void PushLine(const Vector3D& v1, const Vector3D& v2, const Color& color);

	void DrawLine(const Vector3D& v1, const Vector3D& v2, const Color& color);

	virtual void MousePressed(const MouseEvent& event);

	virtual void MouseReleased(const MouseEvent& event);

	/* 创建一个滑动条控件 */
	float* RegisterSliderF(const std::string& name, float min = 0.0f, float max = 1.0f);

	void GUIDraw();

	void SetRenderer(Renderer* renderer) { this->renderer = renderer; }

public:
	Vector3D pos;

	std::vector<Vector3D> vec;
	std::vector<Color> colors;

	struct SliderF
	{
		float value;
		float min;
		float max;
	};

	SliderF slider_f[256];
	std::map<std::string, int> sliderf;

	Renderer* renderer;
	Model* model;
	ModelBone* model_bone;
};
