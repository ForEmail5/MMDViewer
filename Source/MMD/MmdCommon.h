#ifndef MMD_COMMON_H
#define MMD_COMMON_H

#include <string>
#include <vector>

namespace mmd
{
	//--------------------------------------------------------------------
	// 向量
	//--------------------------------------------------------------------
	typedef __int8	int8;
	typedef __int16 int16;
	typedef __int32 int32;

	typedef unsigned __int8	 uint8;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;

	//--------------------------------------------------------------------
	// 2D、3D、4D 向量
	//--------------------------------------------------------------------
	struct mmd_vector2d
	{
		float x, y;
	};

	struct mmd_vector3d
	{
		float x, y, z;
	};

	struct mmd_vector4d
	{
		float x, y, z, w;
	};

	static const float mmd_math_const_pi = 3.1415926f;

	//--------------------------------------------------------------------
	// 动作数据数据
	//--------------------------------------------------------------------
	struct vmd_header
	{
		std::string magic;	/* 30 bytes */
		std::wstring name;	/* 20 bytes */
	};

	struct vmd_bone
	{
		std::wstring bone_name;		/* 15 bytes */
		uint32 nframe;
		mmd_vector3d translation;
		mmd_vector4d rotation;

		int8 interpolator[64];
	};

	struct vmd_morph
	{
		std::wstring morph_name;		/* 15 bytes */
		uint32 nframe;
		float weight;
	};

	struct vmd_camera
	{
		uint32 nframe;
		float focal_length;
		mmd_vector3d position;
		mmd_vector3d rotation;
		int8 interpolator[24];
		float fov;
		uint8 orthographic;
	};

	struct vmd_light
	{
		uint32 nframe;
		mmd_vector3d color;
		mmd_vector3d position;
	};

	struct vmd_self_shadow
	{
		uint32 nframe;
		uint8 mode;
		float distance;
	};

	struct vmd_motion
	{
		vmd_header header;
		vmd_bone bone;
		vmd_morph morph;
	};

	struct mmd_motion
	{
		std::string magic;
		std::wstring name;

		std::vector<vmd_bone> bones;
		std::vector<vmd_morph> morphs;
		std::vector<vmd_camera> camera;
	};

	//--------------------------------------------------------------------
	// 模型数据
	//--------------------------------------------------------------------

	/* 顶点结构定义 */
	struct mmd_vertex
	{
		mmd_vector3d position;		/* 坐标 */
		mmd_vector3d normal;		/* 法线 */
		mmd_vector2d texcoord;		/* 纹理坐标 */

		uint8 bone_num;
		uint32 bone_indices[4];
		float weights[4];
	};

	/* 材质结构 */
	struct mmd_material
	{
		mmd_vector4d diffuse;		/* 漫反射颜色 */
		mmd_vector3d specular;		/* 高光颜色 */
		mmd_vector3d embient;		/* 高光颜色 */
		float shininess;			/* 反光度 */

		int index_num;				/* 索引数量 */
		int index_begin;			/* 索引起始位置 */

		float edge_size;
		mmd_vector4d edge_color;

		int toon;

		std::string sub_texture;
		std::string main_texture;
	};

	/* 骨骼 */
	struct mmd_bone
	{
		std::wstring name;
		std::wstring name_en;
		mmd_vector3d position;
		mmd_vector3d bone_offset;
		int child_id;
		int parent_id;
		int transform_level;

		bool has_ik;
		bool child_use_id;

		struct IKLink
		{
			int index;
			bool has_limit;
			mmd_vector3d lo;
			mmd_vector3d hi;
		};

		struct IKInfo
		{
			int target_index;
			int ccd_iterate_limit;
			float ccd_angle_limit;
			std::vector<IKLink> ik_links;
		} ik_info;
	};

	/* 变形器类别 */
	enum MorphCategory
	{
		MORPH_CAT_SYSTEM = 0x00,
		MORPH_CAT_EYEBROW = 0x01,
		MORPH_CAT_EYE = 0x02,
		MORPH_CAT_MOUTH = 0x03,
		MORPH_CAT_OTHER = 0x04
	};

	/* 变形器类型 */
	enum MorphType
	{
		MORPH_TYPE_GROUP = 0x00,
		MORPH_TYPE_VERTEX = 0x01,
		MORPH_TYPE_BONE = 0x02,
		MORPH_TYPE_UV = 0x03,
		MORPH_TYPE_EXT_UV_1 = 0x04,
		MORPH_TYPE_EXT_UV_2 = 0x05,
		MORPH_TYPE_EXT_UV_3 = 0x06,
		MORPH_TYPE_EXT_UV_4 = 0x07,
		MORPH_TYPE_MATERIAL = 0x08
	};

	union mmd_morph_data
	{
		struct group_morph_
		{
			int morph_index;
			float morph_rate;
		} group_morph;

		struct vertex_morph_
		{
			int vertex_index;
			mmd_vector3d offset;
		} vertex_morph;

		struct bone_morph_
		{
			int bone_index;
			mmd_vector3d translation;
			mmd_vector4d rotation;
		} bone_morph;

		struct uv_morph_
		{
			int vertex_index;
			mmd_vector4d offset;
		} nv_morph;
	};

	struct mmd_morph
	{
		std::wstring name;
		std::wstring name_en;

		MorphType morph_type;
		MorphCategory morph_category;
		std::vector<mmd_morph_data> morph_datas;
	};

	struct mmd_model
	{
		std::vector<uint32> indices;
		std::vector<mmd_vertex> vertices;
		std::vector<std::string> textures;
		std::vector<mmd_material> materials;
		std::vector<mmd_bone> bones;
		std::vector<mmd_morph> morphs;
	};
}

#endif