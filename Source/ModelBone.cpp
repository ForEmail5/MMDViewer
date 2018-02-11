#include "ModelBone.h"
#include "Debug.h"

#define BONE_NULL 65535

//------------------------------------------------------------------------
// Bone
//------------------------------------------------------------------------
Bone::Bone()
{
	end_pos = Vector3D::ZERO;
	root_pos = Vector3D::ZERO;

	basic_matrix = Matrix4::IDENTITY;
	local_matrix = Matrix4::IDENTITY;
	offset_matrix = Matrix4::IDENTITY;

	double_bone = nullptr;
	parent_bone = nullptr;
	ik_controlled = false;
}

void Bone::InitWithMMDBone(const mmd::mmd_bone& bone)
{
	name = bone.name;
	type = bone.has_ik ? BT_IK : BT_Normal;

	root_pos.Set(bone.position.x, bone.position.y, bone.position.z);
	offset_matrix = Matrix4::MakeTransform(-root_pos, Vector3D::ONE);
}

void Bone::CreateBasicMatrix()
{
	Vector3D offset = root_pos;
	if ( HasParent() )
	{
		offset = offset - GetParent()->GetRootPosition();
	}
	basic_matrix = Matrix4::MakeTransform(offset, Vector3D::ONE);
	local_matrix = Matrix4::MakeTransform(offset, Vector3D::ONE);
}

BoneType Bone::GetBoneType()
{
	return type;
}

std::wstring Bone::GetName()
{
	return name;
}

void Bone::SetDoubleBone(Bone* bone)
{
	double_bone = bone;
	double_bone->SetIKController(true);
}

void Bone::SetParent(Bone* parent)
{
	parent_bone = parent;
	parent->end_pos = root_pos;
}

Bone* Bone::GetParent() 
{
	return parent_bone; 
}

bool Bone::HasParent() 
{ 
	return parent_bone != nullptr; 
}

void Bone::SetIKController(bool enable)
{
	ik_controlled = enable;
}

bool Bone::IKControlled()
{
	return ik_controlled;
}

Vector3D Bone::GetRootPosition() 
{ 
	return root_pos; 
}

Vector3D Bone::GetEndPosition()
{ 
	return end_pos; 
}

Vector3D Bone::GetRootPosToWorldSpace() 
{
	return GetFinalMatrix() * root_pos; 
}

Vector3D Bone::GetEndPosToWorldSpace() 
{ 
	return GetFinalMatrix() * end_pos; 
}

/* 在初始状态下对骨骼进行旋转、平移等变换 */
void Bone::UpdateLocalMatrix(const Matrix4& transform) 
{ 
	if ( IKControlled() )
	{
		local_matrix = basic_matrix;
	}
	else
	{
		local_matrix = basic_matrix * transform;
	}
}

/* 在上次状态下对骨骼进行旋转、平移等变换 */
void Bone::TransformLocalMatrix(const Matrix4& transform)
{ 
	local_matrix = local_matrix * transform; 

	/* 共享同样的矩阵 */
	if ( double_bone )
	{
		double_bone->local_matrix = local_matrix;
	}
}

/* 每个骨骼最终的变换矩阵 */
Matrix4 Bone::GetFinalMatrix()
{ 
	return ToWorldSpace() * ToLocalSpace(); 
}

/* 从世界坐标系到局部坐标系变换矩阵 */
Matrix4 Bone::ToLocalSpace() 
{ 
	return offset_matrix; 
}

/* 从局部坐标系到世界坐标系变换矩阵 */
Matrix4 Bone::ToWorldSpace()
{
	Bone* parent = parent_bone;
	Matrix4 ws_mat = local_matrix;

	while ( parent )
	{
		ws_mat = parent->local_matrix * ws_mat;
		parent = parent->parent_bone;
	}
	return ws_mat;
}


//------------------------------------------------------------------------
// IK
//------------------------------------------------------------------------
IK::IK(Bone* bone, int ccd_iterate, float limit_angle)
{
	this->ik = bone;
	this->parent = nullptr;
	this->is_update = false;
	this->ccd_angle_limit = limit_angle;
	this->ccd_iterater_limit = ccd_iterate;
}

IK::~IK()
{
	/* 释放所有 IK 节点 */
	for ( int i = 0; i < ik_nodes.size(); i++ ) delete ik_nodes[i];
}

void IK::SetParent(IK* parent)
{
	this->parent = parent;
}

void IK::AppendIKNode(Bone* bone, const Vector3D& dof)
{
	IKNode* node = new IKNode;
	ik_nodes.push_back(node);

	node->dof = dof;
	node->bone = bone;
	node->limit = (dof.x != 0 || dof.y != 0 || dof.z != 0);
}

void IK::Reset()
{
	this->is_update = false;
}

void IK::Update()
{
	if ( is_update == false )
	{
		is_update = true;

		/* 先更新父 IK */
		if ( parent && parent->IsUpdate() == false )
		{
			parent->Update();
		}
		this->CyclicCoordinateDescent(ik->GetRootPosToWorldSpace());
	}
}

void IK::CalculateIKBonePosition()
{
	for ( int i = 0; i < ik_nodes.size(); i++ )
	{
		ik_nodes[i]->end = ik_nodes[i]->bone->GetEndPosToWorldSpace();
		ik_nodes[i]->begin = ik_nodes[i]->bone->GetRootPosToWorldSpace();
	}
}

/* 循环坐标下降，迭代所有结点的位置 */
void IK::CyclicCoordinateDescent(const Vector3D& target)
{
	if ( ik_nodes.size() <= 0 ) return;

	CeShi* cs = CeShi::GetInstance();
	cs->ResetDraw();

	for ( int j = 0; j < 128; j++ )
	{
		this->CalculateIKBonePosition();
		Vector3D final_pos = ik_nodes[0]->end;

		if ( Vector3D::Length(target, final_pos) < 0.02 ) return;

		/* 进行一次迭代 */
		for ( int i = 0; i < ik_nodes.size(); i++ )
		{
			IKNode* node = ik_nodes[i];
			Matrix4 inverse = node->bone->GetFinalMatrix().Inverse();
			Vector3D bp = inverse * node->bone->GetRootPosToWorldSpace();
			Vector3D fp = inverse * final_pos;
			Vector3D tp = inverse * target;
			Vector3D src_vector = fp - bp;
			Vector3D dst_vector = tp - bp;

			/* 计算骨骼需要旋转的四元数 */
			float angle = Math::Clamp(Vector3D::Angle(src_vector, dst_vector), -ccd_angle_limit, ccd_angle_limit);
			Quaternion rot = Quaternion::AngleAxis(angle, Vector3D::Cross(src_vector, dst_vector));

			/* 对于有自由度限制的骨骼，只允许向活动轴转动 */
			if ( node->limit )
			{
				/* 自由度约束 */
				Vector3D euler = rot.ToEular() * node->dof;

				/* 计算限制角度，比如人的小腿不能向前弯，只能往后弯 */
				if ( euler.x > 0 && i < ik_nodes.size() - 1 )
				{
					/* 计算该骨骼只能旋转的最大角度 */
					Vector3D v1 = node->end - node->begin;
					Vector3D v2 = ik_nodes[i + 1]->end - ik_nodes[i + 1]->begin;
					float max_angle = Vector3D::Angle(v1, v2);

					/* 这里只对 X 轴进行限制，只是针对脚部的 IK 的约束 */
					if ( max_angle > 0.14 )
					{
						euler.x = Math::Clamp(euler.x, -PI, max_angle - 0.1);
					}
					else
					{
						euler.x = 0;
					}
				}
				rot = Quaternion::EulerXYZ(euler.x, euler.y, euler.z);
			}
			/* 更新该骨骼变换矩阵 */
			node->bone->TransformLocalMatrix(rot.ToRotationMatrx());

			/* 计算更新后的末端效应器的位置 */
			final_pos = ik_nodes[0]->bone->GetEndPosToWorldSpace();
		}
	}
}

//------------------------------------------------------------------------
// ModelBone
//------------------------------------------------------------------------
ModelBone::ModelBone(const std::vector<mmd::mmd_bone>& bones)
{
	this->CreateModelBones(bones);
	this->CreateBoneIK(bones);

	this->UpdateAllBoneIK();
	this->UpdateAllBoneMatrix();

	CeShi::GetInstance()->model_bone = this;
}

ModelBone::~ModelBone()
{
	/* 释放模型的骨骼资源 */
	for ( int i = 0; i < bone_nodes.size(); i++ )
	{
		delete bone_nodes[i];
	}

	/* 释放模型的 IK 结点 */
	for ( auto it = iks.begin(); it != iks.end(); ++it )
	{
		delete it->second;
	}
}

void ModelBone::Reset()
{
	for ( int i = 0; i < final_matries.size(); i++ )
	{
		final_matries[i] = Matrix4::IDENTITY;
	}
}

void ModelBone::CreateModelBones(const std::vector<mmd::mmd_bone>& bones)
{
	/* 将骨骼的变换矩阵储存到数组中，并建立一个索引表，通过骨骼名称来获取骨骼矩阵 */
	for ( int i = 0; i < bones.size(); i++ )
	{
		Bone* bone = new Bone;
		bone->InitWithMMDBone(bones[i]);

		bone_nodes.push_back(bone);
		final_matries.push_back(Matrix4::IDENTITY);
		bone_indexed_tabel.insert(std::make_pair(bones[i].name, bone_nodes.size() - 1));
	}

	for ( int i = 0; i < bones.size(); i++ )
	{
		Bone* bone = bone_nodes[i];
		int parent = bones[i].parent_id;

		/* 查找该骨骼的父骨骼 */
		if ( parent != BONE_NULL )
		{
			bone->SetParent(bone_nodes[parent]);
		}
		/* 有些模型把脚部的骨骼拷贝一份，在 IK 更新骨骼时，要同时更新另一份骨骼。为什么呢？ */
		std::wstring copy_bone = bones[i].name + L"D";
		auto it =  bone_indexed_tabel.find(copy_bone);
		if ( it != bone_indexed_tabel.end() )
		{
			bone_nodes[i]->SetDoubleBone(bone_nodes[it->second]);
		}

		bone_nodes[i]->CreateBasicMatrix();
	}
}

void ModelBone::CreateBoneIK(const std::vector<mmd::mmd_bone>& bones)
{
	for ( int i = 0; i < bones.size(); i++ )
	{
		if ( bones[i].has_ik == false ) continue;
		mmd::mmd_bone::IKInfo ik_info = bones[i].ik_info;

		/* 创建 IK，将关联的骨骼绑定到 IK 中 */
		IK* ik = new IK(bone_nodes[i], ik_info.ccd_iterate_limit, ik_info.ccd_angle_limit);

		for ( int j = 0; j < ik_info.ik_links.size(); j++ )
		{
			int bone_idx = ik_info.ik_links[j].index;

			bone_nodes[bone_idx]->SetIKController(true);
			if ( ik_info.ik_links.size() == 1 )
			{
				ik->AppendIKNode(bone_nodes[bone_idx], Vector3D(1, 1, 0));
			}
			else if ( j < ik_info.ik_links.size() - 1 )
			{
				ik->AppendIKNode(bone_nodes[bone_idx], Vector3D(1, 0, 0));
			}
			else
			{
				ik->AppendIKNode(bone_nodes[bone_idx], Vector3D(1, 0, 1));
			}
		}
		iks.insert(std::make_pair(bones[i].name, ik));
	}

	/* 设置父子 IK */
	for ( int i = 0; i < bones.size(); i++ )
	{
		if ( bones[i].has_ik == false || bones[i].parent_id == 0 ) continue;

		int parent = bones[i].parent_id;
		if ( parent != BONE_NULL && bones[parent].has_ik )
		{
			auto it = iks.find(bones[i].name);
			auto pit = iks.find(bones[parent].name);
			it->second->SetParent(pit->second);
		}
	}
}

int ModelBone::HasBone(const std::wstring& name)
{
	auto it =  bone_indexed_tabel.find(name);
	if ( it != bone_indexed_tabel.end() )
	{
		return it->second;
	}
	return -1;
}

void ModelBone::SetBoneMatrix(int index, const Matrix4& mat)
{
	bone_nodes[index]->UpdateLocalMatrix(mat);
}

void ModelBone::UpdateAllBoneIK()
{
	for ( auto it = iks.begin(); it != iks.end(); ++it )
	{
		it->second->Update();
	}

	for ( auto it = iks.begin(); it != iks.end(); ++it )
	{
		it->second->Reset();
	}
}

void ModelBone::UpdateAllBoneMatrix()
{
	for ( int i = 0; i < final_matries.size(); i++ )
	{
		final_matries[i] = bone_nodes[i]->GetFinalMatrix();
	}
}