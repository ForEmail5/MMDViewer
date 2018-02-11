#ifndef MORPH_CONTROLLER_H
#define MORPH_CONTROLLER_H

#include <map>

#include "MathLib.h"
#include "MMD\MmdCommon.h"

class Model;
class MorphController;

//---------------------------------------------------------------------------
// MorphHandle
//---------------------------------------------------------------------------
class MorphHandle
{
public:
	MorphHandle(const std::wstring& name, MorphController* mc);

	void SetValue(float weight);

	void SetOutputVertices(std::vector<Vector3D>* positions);

	void AppendData(int index, const Vector3D& begin, const Vector3D& offset);

	std::wstring GetName() { return name; }

private:
	std::wstring name;					/* 变形动画名称 */
	MorphController* morph_controller;

	float weight;
	std::vector<Vector3D>* positions;

	struct Element
	{
		int index;
		Vector3D begin;
		Vector3D offset;
	};
	std::vector<Element> elements;
};


//---------------------------------------------------------------------------
// MorphController
//---------------------------------------------------------------------------
class MorphController
{
public:
	MorphController(Model* model);

	~MorphController();

	void LoadMorph(const std::vector<mmd::mmd_morph>& morphs);

	void Update();

	int GetMorphHandleCount();

	MorphHandle* GetMorphHandleByName(const std::wstring& name);

	MorphHandle* GetMorphHandleByIndex(int index);

private:
	Model* model;

	std::map<std::wstring, MorphHandle*> morph_handles;
};

#endif