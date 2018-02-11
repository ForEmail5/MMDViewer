#include "MorphController.h"
#include "Model.h"

//---------------------------------------------------------------------------
// MorphControlHandle
//---------------------------------------------------------------------------
MorphHandle::MorphHandle(const std::wstring& name, MorphController* mc)
{
	this->name = name;
	this->weight = 0.0f;
	this->morph_controller = mc;
}

void MorphHandle::AppendData(int index, const Vector3D& begin, const Vector3D& offset)
{
	Element ele;
	ele.index = index;
	ele.begin = begin;
	ele.offset = offset;

	elements.push_back(ele);
}

void MorphHandle::SetOutputVertices(std::vector<Vector3D>* positions)
{
	this->positions = positions;
}

void MorphHandle::SetValue(float weight)
{
	if ( Math::Equal(this->weight, weight) ) return;
	this->weight = weight;

	/* 计算顶点的新位置 */
	for ( auto& ele : elements )
	{
		(*positions)[ele.index] = ele.begin + ele.offset * this->weight;
	}
}

//---------------------------------------------------------------------------
// MorphController
//---------------------------------------------------------------------------
MorphController::MorphController(Model* model)
{
	this->model = model;
}

MorphController::~MorphController()
{
	for ( auto& it : morph_handles )
	{
		delete it.second;
	}
	morph_handles.clear();
}

void MorphController::LoadMorph(const std::vector<mmd::mmd_morph>& morphs)
{
	std::vector<Vector3D>* positions = model->GetPositions();

	/* 将 mmd 变形动画信息转换到 MorphControlHandle 中 */
	for ( int i = 0; i < morphs.size(); i++ )
	{
		mmd::mmd_morph morph = morphs[i];
		MorphHandle* mch = new MorphHandle(morph.name, this);
		mch->SetOutputVertices(positions);

		/* 每个表情由大量相关联顶点偏移而成，将每一个顶点的偏移信息保存到 MorphControlHandle 中 */
		for ( int j = 0; j < morph.morph_datas.size(); j++ )
		{
			mmd::mmd_morph_data data = morph.morph_datas[j];

			/* 偏移量 */
			Vector3D offset;
			offset.x = data.vertex_morph.offset.x;
			offset.y = data.vertex_morph.offset.y;
			offset.z = data.vertex_morph.offset.z;

			mch->AppendData(data.vertex_morph.vertex_index, positions->at(data.vertex_morph.vertex_index), offset);
		}
		morph_handles.insert(std::make_pair(morph.name, mch));
	}
}

void MorphController::Update()
{
	model->UpdateVectices();
}

int MorphController::GetMorphHandleCount()
{
	return morph_handles.size();
}

MorphHandle* MorphController::GetMorphHandleByName(const std::wstring& name)
{
	auto it = morph_handles.find(name);
	if ( it == morph_handles.end() )
	{
		return nullptr;
	}
	return it->second;
}

MorphHandle* MorphController::GetMorphHandleByIndex(int index)
{
	if ( index >= morph_handles.size() ) return nullptr;

	auto it = morph_handles.begin();
	for ( int i = 0; i < morph_handles.size(); i++, ++it )
	{
		if ( i == index )
		{
			return it->second; 
		}
	}
	return nullptr;
}