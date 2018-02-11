#ifndef PMX_TYPE_H
#define PMX_TYPE_H

#include "MmdCommon.h"

#pragma pack(push)	/* 保存对齐状态 */
#pragma pack(1)		/* 设置 1 字节对齐 */

namespace mmd
{
	/* 文件头部 */
	struct pmx_header
	{
		std::string magic;	// 4 bytes
		float vertion;		// 4 bytes
	};

	/* 顶点结构 */
	struct pmx_vertex_basic
	{
		mmd_vector3d position;
		mmd_vector3d normal;
		mmd_vector2d texcoord;
	};

	enum pmx_material_draw_flags
	{
		PMX_MATERIAL_DRAW_BOUBLE_FACE = 0x01,
		PMX_MATERIAL_DRAW_GROUND_SHADOW = 0x02,
		PMX_MATERIAL_CAST_SELF_SHADOW = 0x04,
		PMX_MATERIAL_DRAW_SELF_SHADOW = 0x08,
		PMX_MERTAIL_DRAW_EDGE = 0x10
	};

	/* 材质结构 */
	struct pmx_material_basic
	{
		mmd_vector4d diffuse;
		mmd_vector3d specular;
		float shininess;
		mmd_vector3d ambient;
		uint8 draw_flag;
		mmd_vector4d edge_color;
		float edge_size;
	};

	enum pmx_bone_flags
	{
		PMX_BONE_CHILD_USE_ID = 0x0001,
		PMX_BONE_ROTATABLE = 0x0002,
		PMX_BONE_MOVABLE = 0x0004,
		PMX_BONE_VISIBLE = 0x0008,
		PMX_BONE_CONTROLLABLE = 0x0010,
		PMX_BONE_HAS_IK = 0x0020,
		PMX_BONE_ACQUIRE_ROTATE = 0x0100,
		PMX_BONE_ACQUIRE_TRANSLATE = 0x0200,
		PMX_BONE_ROT_AXIS_FIXED = 0x0400,
		PMX_BONE_USE_LOCAL_AXIS = 0x0800,
		PMX_BONE_POST_PHYSICS = 0x1000,
		PMX_BONE_RECEIVE_TRANSFORM = 0x2000
	};

	struct pmx_material_morph
	{
		uint8 offset_type;
		mmd_vector4d diffuse;
		mmd_vector3d specular;
		float shininess;
		mmd_vector3d ambient;
		mmd_vector4d edge_color;
		float edge_size;
		mmd_vector4d texture_shift;
		mmd_vector4d sub_texture_shift;
		mmd_vector4d toon_shift;
	};

	/* 刚体的结构定义 */
	struct pmx_rigid_body
	{
		uint8 collision_group;
		uint16 collision_mask;
		uint8 shape;
		mmd_vector3d dimemsions;
		mmd_vector3d position;
		mmd_vector3d rotation;
		float mass;
		float translate_damp;
		float rotate_damp;
		float restitation;
		float friction;
		uint8 type;
	};

	/* 约束的结构定义 */
	struct pmx_constraint
	{
		mmd_vector3d position;
		mmd_vector3d rotation;
		mmd_vector3d position_limit[2];
		mmd_vector3d rotation_limit[2];
		mmd_vector3d stiffness[2];
	};

	/* 蒙皮类型 */
	enum SkinningType
	{
		SKINNING_BDEF1 = 0,
		SKINNING_BDEF2 = 1,
		SKINNING_BDEF4 = 2,
		SKINNING_SDEF = 3,
	};
}

#pragma pack(pop)	/* 恢复对齐状态 */	
#endif