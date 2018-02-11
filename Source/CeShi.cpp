#include "CeShi.h"
#include "Debug.h"
#include "Model.h"
#include "Renderer.h"
#include "ModelBone.h"

#include "ImGUI\imgui.h"


void CeShi::ResetDraw()
{
	vec.clear();
	colors.clear();
}

void CeShi::PushLine(const Vector3D& v1, const Vector3D& v2, const Color& color)
{
	vec.push_back(v1);
	vec.push_back(v2);
	colors.push_back(color);
}

void CeShi::DrawLine(const Vector3D& v1, const Vector3D& v2, const Color& color)
{
	renderer->DrawLine(v1, v2, color);
}

void CeShi::MousePressed(const MouseEvent& event)
{
	if ( event.button_type == ButtonType::LEFT_BUTTON )
	{
		//this->ResetDraw();
		//model_bone->UpdateAllBoneIK();
		//model_bone->UpdateAllBoneMatrix();
		//model->UpdateModelBone();
	}
}

void CeShi::MouseReleased(const MouseEvent& event)
{

}

float* CeShi::RegisterSliderF(const std::string& name, float min, float max)
{
	float* value = nullptr;

	auto it =  sliderf.find(name);
	if ( it == sliderf.end() )
	{
		int index = sliderf.size();

		slider_f[index].value = 0.0f;
		slider_f[index].min = min;
		slider_f[index].max = max;
		value = &slider_f[index].value;

		sliderf.insert(std::make_pair(name, index));
	}
	else
	{
		value = &slider_f[it->second].value;
	}
	return value;
}

void CeShi::GUIDraw()
{
	if ( sliderf.size() <= 0 ) return;

	ImGui::Begin("Slider");
	{
		for ( auto it = sliderf.begin(); it != sliderf.end(); ++it )
		{
			int index = it->second;
			ImGui::SliderFloat(it->first.c_str(), &slider_f[index].value, slider_f[index].min, slider_f[index].max);
		}
	}
	ImGui::End();
}
