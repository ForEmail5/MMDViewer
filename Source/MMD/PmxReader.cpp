#include "PmxReader.h"
#include "../Debug.h"

namespace mmd
{
	mmd_model* PmxReader::ReadModel(FileReader& reader)
	{
		/* 读取文件头部 */
		pmx_header header;
		header.magic = reader.ReadStringA(4);
		header.vertion = reader.Read<float>();

		unsigned char file_flags_size = reader.Read<uint8>();

		/* 检测文件类型是否为 PMD 文件 */
		if ( strncmp(header.magic.c_str(), "PMX", 3) || header.vertion != 2.0f )
		{
			Debug::LogAndWrite("File is not a PMX 2.0 file_.");
			return nullptr;
		}

		mmd_model* model = new mmd_model;
		bool utf8_encoding = reader.Read<uint8>() > 0;

		size_t extra_uv_number = reader.Read<uint8>();

		size_t vertex_index_size	 = reader.Read<uint8>();
		size_t texture_index_size	 = reader.Read<uint8>();
		size_t material_index_size	 = reader.Read<uint8>();
		size_t bone_index_size		 = reader.Read<uint8>();
		size_t morph_index_size		 = reader.Read<uint8>();
		size_t rigid_body_index_size = reader.Read<uint8>();

		std::wstring name = reader.ReadStringW(utf8_encoding);
		std::wstring name_en = reader.ReadStringW(utf8_encoding);
		std::wstring description = reader.ReadStringW(utf8_encoding);
		std::wstring description_en = reader.ReadStringW(utf8_encoding);

		/* 读取顶点数据 */
		size_t vertex_num = reader.Read<int>();
		for ( int i = 0; i < vertex_num; i++ )
		{
			pmx_vertex_basic pv = reader.Read<pmx_vertex_basic>();

			mmd_vertex vertex = { 0 };
			vertex.position = pv.position;
			vertex.texcoord = pv.texcoord;
			vertex.normal   = pv.normal;

			vertex.bone_indices[0] = 65535;
			vertex.bone_indices[1] = 65535;
			vertex.bone_indices[2] = 65535;
			vertex.bone_indices[3] = 65535;

			for ( int ei = 0; ei < extra_uv_number; ei++ )
			{
				reader.Read<mmd_vector4d>();
			}

			SkinningType skinning_type = ( SkinningType ) reader.Read<int8>();
			switch ( skinning_type )
			{
			case mmd::SKINNING_BDEF1:
				vertex.bone_num = 1;
				vertex.bone_indices[0] = reader.ReadIndex(bone_index_size);
				vertex.weights[0] = 1;
				break;
			case mmd::SKINNING_BDEF2:
				vertex.bone_num = 2;
				vertex.bone_indices[0] = reader.ReadIndex(bone_index_size);
				vertex.bone_indices[1] = reader.ReadIndex(bone_index_size);
				vertex.weights[0] = reader.Read<float>();
				vertex.weights[1] = 1 - vertex.weights[0];
				break;
			case mmd::SKINNING_BDEF4:
				vertex.bone_num = 4;
				for ( int j = 0; j < 4; j++ ) vertex.bone_indices[j] = reader.ReadIndex(bone_index_size);
				for ( int j = 0; j < 4; j++ ) vertex.weights[j] = reader.Read<float>();
				break;
			case mmd::SKINNING_SDEF:
				vertex.bone_num = 2;
				vertex.bone_indices[0] = reader.ReadIndex(bone_index_size);
				vertex.bone_indices[1] = reader.ReadIndex(bone_index_size);
				vertex.weights[0] = reader.Read<float>();
				vertex.weights[1] = 1 - vertex.weights[0];
				reader.Read<mmd_vector3d>();
				reader.Read<mmd_vector3d>();
				reader.Read<mmd_vector3d>();
				break;
			default:
				// TODO: 
				break;
			}

			if ( vertex.bone_indices[0] > 122 )
			{
				int a = 0;
			}

			float edge_scale = reader.Read<float>();
			model->vertices.push_back(vertex);
		}

		/* 读取索引数据 */
		int index_num = ( int ) reader.Read<int>();
		for ( int i = 0; i < index_num; i++ )
		{
			model->indices.push_back(reader.ReadIndex(vertex_index_size));
		}

		/* 读取纹理数据 */
		int texture_num = reader.Read<int32>();
		for ( int i = 0; i < texture_num; i++ )
		{
			model->textures.push_back(UTF16ToNative(reader.ReadStringW(utf8_encoding)));
		}

		/* 读取材质数据 */
		int in = 0;
		int part_num = reader.Read<int32>();

		for ( int i = 0; i < part_num; i++ )
		{
			std::wstring name = reader.ReadStringW(utf8_encoding);
			std::wstring name_en = reader.ReadStringW(utf8_encoding);

			pmx_material_basic pm = reader.Read<pmx_material_basic>();

			mmd_material material;
			material.sub_texture = "";
			material.main_texture = "";
			material.diffuse = pm.diffuse;
			material.embient = pm.ambient;
			material.specular = pm.specular;
			material.shininess = pm.shininess;
			material.edge_size = pm.edge_size;
			material.edge_color = pm.edge_color;

			int texture_index = reader.ReadIndex(texture_index_size);
			if ( texture_index < model->textures.size() )
			{
				material.main_texture = model->textures[texture_index];
			}
			int sub_texture_index = reader.ReadIndex(texture_index_size);
			if ( sub_texture_index < model->textures.size() )
			{
				material.sub_texture = model->textures[sub_texture_index];
			}
			reader.Read<uint8>();

			bool use_global_toon = reader.Read<uint8>() > 0;
			if ( use_global_toon )
			{
				material.toon = reader.Read<uint8>();
			}
			else
			{
				material.toon = reader.ReadIndex(texture_index_size);
			}
			std::wstring material_info = reader.ReadStringW(utf8_encoding);

			material.index_begin = in;
			material.index_num = reader.Read<int32>();

			in += material.index_num;

			model->materials.push_back(material);
		}

		/* 读取骨骼数据 */
		int bone_num = reader.Read<int32>();
		for ( int i = 0; i < bone_num; i++ )
		{
			mmd_bone bone;

			bone.name = reader.ReadStringW(utf8_encoding);
			bone.name_en = reader.ReadStringW(utf8_encoding);
			bone.position = reader.Read<mmd_vector3d>();
			bone.child_id = 65535;
			bone.parent_id = 65535;

			int parent_index = reader.ReadIndex(bone_index_size);
			if ( parent_index < bone_num )
			{
				bone.parent_id = parent_index;
			}

			bone.transform_level = reader.Read<int32>();
			int flag = reader.Read<uint16>();

			bone.child_use_id		= (flag & pmx_bone_flags::PMX_BONE_CHILD_USE_ID		)	!= 0;
			bool rotatable			= (flag & pmx_bone_flags::PMX_BONE_ROTATABLE		)	!= 0;
			bool movable			= (flag & pmx_bone_flags::PMX_BONE_MOVABLE			)	!= 0;
			bool visable			= (flag & pmx_bone_flags::PMX_BONE_VISIBLE			)	!= 0;
			bool constrollable		= (flag & pmx_bone_flags::PMX_BONE_CONTROLLABLE		)	!= 0;
			bone.has_ik				= (flag & pmx_bone_flags::PMX_BONE_HAS_IK			)	!= 0;
			bool append_rotate		= (flag & pmx_bone_flags::PMX_BONE_ACQUIRE_ROTATE	)	!= 0;
			bool append_translate	= (flag & pmx_bone_flags::PMX_BONE_ACQUIRE_TRANSLATE)	!= 0;
			bool rot_aixs_fixed		= (flag & pmx_bone_flags::PMX_BONE_ROT_AXIS_FIXED	)	!= 0;
			bool use_local_axis		= (flag & pmx_bone_flags::PMX_BONE_USE_LOCAL_AXIS	)	!= 0;
			bool post_physics		= (flag & pmx_bone_flags::PMX_BONE_POST_PHYSICS		)	!= 0;
			bool receive_transform	= (flag & pmx_bone_flags::PMX_BONE_RECEIVE_TRANSFORM)	!= 0;

			if ( bone.child_use_id )
			{
				bone.child_id = reader.ReadIndex(bone_index_size);
			}
			else 
			{
				bone.bone_offset = reader.Read<mmd_vector3d>();
			}
			if ( append_rotate || append_translate )
			{
				int append_index = reader.ReadIndex(bone_index_size);
				float append_ratio = reader.Read<float>();
			}
			if ( rot_aixs_fixed )
			{
				mmd_vector3d rot_axis = reader.Read<mmd_vector3d>();
			}
			if ( use_local_axis )
			{
				mmd_vector3d local_x = reader.Read<mmd_vector3d>();
				mmd_vector3d local_y = reader.Read<mmd_vector3d>();
				mmd_vector3d local_z = local_x;
			}
			if ( receive_transform )
			{
				int export_key = reader.Read<int32>();
			}
			if ( bone.has_ik )
			{
				bone.ik_info.target_index = reader.ReadIndex(bone_index_size);
				bone.ik_info.ccd_iterate_limit = reader.Read<int32>();
				bone.ik_info.ccd_angle_limit = reader.Read<float>();
				int ik_link_num = reader.Read<int32>();
				for ( int j = 0; j < ik_link_num; j++ )
				{
					mmd_bone::IKLink link;
					link.index = reader.ReadIndex(bone_index_size);
					link.has_limit = reader.Read<int8>() != 0;
					if ( link.has_limit )
					{
						link.lo = reader.Read<mmd_vector3d>();
						link.hi = reader.Read<mmd_vector3d>();
					}
					bone.ik_info.ik_links.push_back(link);
				}
			}
			if ( i >= 1024 ) continue;
			model->bones.push_back(bone);
		}

		/* 读取表情数据 */
		int morph_num = reader.Read<int32>();
		int base_morph_index = -1;
		for ( int i = 0; i < morph_num; i++ )
		{
			mmd_morph morph;
			morph.name = reader.ReadStringW(utf8_encoding);
			morph.name_en = reader.ReadStringW(utf8_encoding);
			morph.morph_category = ( MorphCategory ) reader.Read<uint8>();
			morph.morph_type = ( MorphType ) reader.Read<uint8>();

			int morph_data_num = reader.Read<int32>();
			switch ( morph.morph_type )
			{
			case mmd::MORPH_TYPE_GROUP:
				for ( int i = 0; i < morph_data_num; i++ )
				{
					mmd_morph_data data;
					data.group_morph.morph_index = reader.ReadIndex(morph_index_size);
					data.group_morph.morph_rate = reader.Read<float>();
					morph.morph_datas.push_back(data);
				}
				break;
			case mmd::MORPH_TYPE_VERTEX:
				for ( int i = 0; i < morph_data_num; i++ )
				{
					mmd_morph_data data;
					data.vertex_morph.vertex_index = reader.ReadIndex(vertex_index_size);
					data.vertex_morph.offset = reader.Read<mmd_vector3d>();
					morph.morph_datas.push_back(data);
				}
				break;
			case mmd::MORPH_TYPE_BONE:
				for ( int i = 0; i < morph_data_num; i++ )
				{
					mmd_morph_data data;
					data.bone_morph.bone_index	= reader.ReadIndex(bone_index_size);
					data.bone_morph.translation = reader.Read<mmd_vector3d>();
					data.bone_morph.rotation	= reader.Read<mmd_vector4d>();
					morph.morph_datas.push_back(data);
				}
				break;
			case mmd::MORPH_TYPE_UV:
			case mmd::MORPH_TYPE_EXT_UV_1:
			case mmd::MORPH_TYPE_EXT_UV_2:
			case mmd::MORPH_TYPE_EXT_UV_3:
			case mmd::MORPH_TYPE_EXT_UV_4:
				for ( int i = 0; i < morph_data_num; i++ )
				{
					mmd_morph_data data;
					data.nv_morph.vertex_index = reader.ReadIndex(vertex_index_size);
					data.nv_morph.offset = reader.Read<mmd_vector4d>();
					morph.morph_datas.push_back(data);
				}
				break;
			case mmd::MORPH_TYPE_MATERIAL:
				for ( int i = 0; i < morph_data_num; i++ )
				{
					int mm_index = reader.ReadIndex(material_index_size);
					pmx_material_morph pmm = reader.Read<pmx_material_morph>();
				}
				break;
			default:
				break;
			}
			model->morphs.push_back(morph);
		}

		int entry_item_num = reader.Read<int32>();
		for ( int i = 0; i < entry_item_num; i++ )
		{
			std::wstring entry_item_name = reader.ReadStringW(utf8_encoding);
			std::wstring entry_item_name_en = reader.ReadStringW(utf8_encoding);
			bool is_special = reader.Read<uint8>() == 1;
			int element_num = reader.Read<int32>();
			for ( int j = 0; j < element_num; j++ )
			{
				bool is_morph = reader.Read<uint8>() == 1;
				if ( is_morph )
				{
					int morph_index = reader.ReadIndex(morph_index_size);
				}
				else
				{
					int bone_index = reader.ReadIndex(bone_index_size);
				}
			}
		}

		/* 读取刚体数据 */
		//int rigid_body_num = reader.Read<int32>();
		//for ( int i = 0; i < rigid_body_num; i++ )
		//{
		//	std::wstring name = reader.ReadStringW(utf8_encoding);
		//	std::wstring name_en = reader.ReadStringW(utf8_encoding);
		//	int associated_bone_index = reader.ReadIndex(bone_index_size);
		//	pmx_rigid_body rigid_body = reader.Read<pmx_rigid_body>();
		//}

		///* 读取约束数据 */
		//int constraint_num = reader.Read<int32>();
		//for ( int i = 0; i < constraint_num; i++ )
		//{
		//	std::wstring name = reader.ReadStringW(utf8_encoding);
		//	std::wstring name_en = reader.ReadStringW(utf8_encoding);
		//	int  dof_type = reader.Read<uint8>();
		//	if ( dof_type == 0 )
		//	{
		//		reader.ReadIndex(rigid_body_index_size);
		//		reader.ReadIndex(rigid_body_index_size);
		//		pmx_constraint pc = reader.Read<pmx_constraint>();
		//	}
		//}

		return model;
	}
}