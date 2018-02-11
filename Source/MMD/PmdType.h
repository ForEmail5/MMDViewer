#ifndef PMD_TYPE_H
#define PMD_TYPE_H

#include "MmdCommon.h"

#pragma pack(push)	/* �������״̬ */
#pragma pack(1)		/* ���� 1 �ֽڶ��� */

namespace mmd
{
	struct pmd_header
	{
		char  magic[3];
		float vertion;
		char  name[20];
		char  description[256];
	};

	/* �������ݽṹ���� */
	struct pmd_vertex
	{
		mmd_vector3d position;		/* �������� */
		mmd_vector3d normal;		/* ���㷨�� */
		mmd_vector2d texcoord;		/* ������������ */

		int16 skinning_bone_id[2];
		uint8 skinning_weight;
		uint8 non_edge_flag;
	};

	/* ���ʽṹ���� */
	struct pmd_material
	{
		mmd_vector4d diffuse;		/* ��������ɫ */
		float shininess;			/* �߹�ǿ�� */
		mmd_vector3d specular;		/* �߹ⷴ����ɫ */
		mmd_vector3d embient;		/* ����������ɫ */
		int8 toon_id;
		uint8 edge_flag;
		uint32 vertex_num;			/* �������� */
		char texture_name[20];		/* �������� */
	};

	/* �����Ľṹ���� */
	struct pmd_bone
	{
		char  name[20];
		int16 parent_id;
		int16 child_id;
		uint8 type;
		int16 ik_number;
		mmd_vector3d position;
	};

	enum pmd_bone_types
	{
		PMD_BONE_ROTATE					= 0,
		PMD_BONE_ROTATE_AND_TRANSLATE	= 1,
		PMD_BONE_IK						= 2,
		PMD_BONE_UNKNOWN				= 3,
		PMD_BONE_IK_LINK				= 4,
		PMD_BONE_ROTATE_EFFECT			= 5,
		PMD_BONE_IK_TO					= 6,
		PMD_BONE_INVISIBLE				= 7,
		PMD_BONE_TWIST					= 8,
		PMD_BONE_ROTATE_RATIO			= 9
	};

	struct pmd_ik_preamble
	{
		int16 ik_bone_index;
		int16 ik_target_bone_index;
		uint8 ik_chain_length;
		uint16 ccd_iterate_limit;
		float ccd_angle_limit;
	};

	struct pmd_raw_ik
	{
		pmd_ik_preamble preamble;
		std::vector<size_t> chain;
	};

	struct pmd_face_preamble
	{
		char name[20];
		uint32 vertex_num;
		uint8 face_type;
	};
}

#pragma pack(pop)	/* �ָ�����״̬ */	
#endif