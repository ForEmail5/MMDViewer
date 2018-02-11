#ifndef MODEL_BONE_H
#define MODEL_BONE_H

#include <map>
#include <vector>

#include "CeShi.h"
#include "MathLib.h"
#include "MMD\MmdCommon.h"


enum BoneType
{
	BT_Normal,
	BT_IK,
	BT_Unknown
};

//------------------------------------------------------------------------
// Bone
//------------------------------------------------------------------------
class Bone
{
public:
	Bone();

	void InitWithMMDBone(const mmd::mmd_bone& bone);

	void CreateBasicMatrix();

	BoneType GetBoneType();

	std::wstring GetName();

	void SetDoubleBone(Bone* bone);

	void SetParent(Bone* parent);

	Bone* GetParent();

	bool HasParent();

	void SetIKController(bool enable);

	bool IKControlled();

	Vector3D GetRootPosition();

	Vector3D GetEndPosition();

	Vector3D GetRootPosToWorldSpace();

	Vector3D GetEndPosToWorldSpace();

	/* 在初始状态下对骨骼进行旋转、平移等变换 */
	void UpdateLocalMatrix(const Matrix4& transform);

	/* 在上次状态下对骨骼进行旋转、平移等变换 */
	void TransformLocalMatrix(const Matrix4& transform);

	/* 每个骨骼最终的变换矩阵 */
	Matrix4 GetFinalMatrix();

	/* 从世界坐标系到局部坐标系变换矩阵 */
	Matrix4 ToLocalSpace();

	/* 从局部坐标系到世界坐标系变换矩阵 */
	Matrix4 ToWorldSpace();

private:
	BoneType type;
	std::wstring name;

	bool ik_controlled;

	Bone* double_bone;
	Bone* parent_bone;

	Vector3D end_pos;
	Vector3D root_pos;

	Matrix4 basic_matrix;
	Matrix4 local_matrix;
	Matrix4 offset_matrix;
};


//------------------------------------------------------------------------
// IK
//------------------------------------------------------------------------
class IK
{
public:
	struct IKNode
	{
		Bone* bone;
		bool limit;

		Vector3D end;
		Vector3D begin;
		Vector3D dof;
	};

public:
	IK(Bone* bone, int ccd_iterate, float limit_angle);

	~IK();

	void SetParent(IK* parent);

	void AppendIKNode(Bone* bone, const Vector3D& dof = Vector3D::ONE);

	void Reset();

	void Update();

	void CalculateIKBonePosition();

	void CyclicCoordinateDescent(const Vector3D& target);

	bool IsUpdate() { return is_update; }

	Bone* GetBone() { return ik; }

private:
	Bone* ik;
	IK* parent;

	bool is_update;

	float ccd_angle_limit;
	int ccd_iterater_limit;		/* 执行 CCD 算法时的最大迭代次数 */

	std::vector<IKNode*> ik_nodes;
};


//------------------------------------------------------------------------
// ModelBone
//------------------------------------------------------------------------
class ModelBone
{
public:
	ModelBone(const std::vector<mmd::mmd_bone>& bones);

	~ModelBone();

	void Reset();

	int HasBone(const std::wstring& name);

	void SetBoneMatrix(int index, const Matrix4& mat);

	void UpdateAllBoneMatrix();

	void UpdateAllBoneIK();

	int GetBoneCount() { return bone_nodes.size(); }

	Bone* GetBoneByIndex(int index) { return bone_nodes[index]; }

private:
	void CreateModelBones(const std::vector<mmd::mmd_bone>& bones);

	void CreateBoneIK(const std::vector<mmd::mmd_bone>& bones);

public:
	std::vector<Bone*> bone_nodes;
	std::map<std::wstring, IK*> iks;
	std::map<std::wstring, int> bone_indexed_tabel;

	std::vector<Matrix4> final_matries;
};

#endif