#include "PmdReader.h"
#include "../Debug.h"

#include <set>
#include <cassert>
#include <algorithm>
#include <exception>

namespace mmd
{
	mmd_model* PmdReader::ReadModel(FileReader& reader)
	{
		/* 读取文件头部 */
		pmd_header header = reader.Read<pmd_header>();

		/* 检测文件类型是否为 PMD 文件 */
		if ( strcmp(header.magic, "Pmd") || header.vertion != 1.0f )
		{
			Debug::LogAndWrite("Not Pmd file.");
			return nullptr;
		}
		mmd_model* model = new mmd_model;

		/* 读取顶点数据 */
		size_t vertex_num = reader.Read<uint32>();
		for ( int i = 0; i < vertex_num; i++ )
		{
			pmd_vertex pv = reader.Read<pmd_vertex>();

			mmd_vertex vertex = { 0 };
			vertex.bone_num = 2;
			vertex.normal   = pv.normal;
			vertex.position = pv.position;
			vertex.texcoord = pv.texcoord;

			vertex.weights[0] = pv.skinning_weight * 0.01;
			vertex.weights[1] = 1 - vertex.weights[0];
			vertex.bone_indices[0] = pv.skinning_bone_id[0];
			vertex.bone_indices[1] = pv.skinning_bone_id[1];

			model->vertices.push_back(vertex);
		}

		/* 读取顶点索引数据 */
		size_t triangle_num = reader.Read<uint32>();
		for ( int i = 0; i < triangle_num; i++ )
		{
			model->indices.push_back(reader.Read<uint16>());
		}

		/* 读取材质数据 */
		size_t part_num = reader.Read<uint32>();
		for ( int i = 0; i < part_num; i++ )
		{
			pmd_material pm = reader.Read<pmd_material>();

			mmd_material material;
			material.diffuse   = pm.diffuse;
			material.shininess = pm.shininess;
			material.specular  = pm.specular;
			material.embient   = pm.embient;
			material.index_num = pm.vertex_num;
			material.sub_texture = "";
			material.main_texture = "";

			std::string texture_name = pm.texture_name;
			model->textures.push_back(texture_name);
			if ( texture_name.size() > 0 )
			{
				size_t pos = texture_name.find('*');
				if ( pos != std::string::npos )
				{
					std::string tex_name = texture_name.substr(0, pos);
					std::string spx_name = texture_name.substr(pos + 1);

					if ( tex_name.size() > 0 ) material.main_texture = tex_name;
					if ( spx_name.size() > 0 ) material.sub_texture = spx_name;
				}
				else
				{
					material.main_texture = texture_name;
				}
			}

			material.toon = -1;

			mmd_vector4d black_color;
			black_color.w = 1.0f;
			black_color.x = black_color.y = black_color.z = 0.0f;
			material.edge_color = black_color;
			material.edge_size = 1.0f;

			model->materials.push_back(material);
		}

		/* 读取骨骼数据 */
		int bone_num = reader.Read<uint16>();
		std::vector<pmd_bone> raw_bones;
		for ( int i = 0; i < bone_num; i++ )
		{
			raw_bones.push_back(reader.Read<pmd_bone>());
		}

		std::set<int> ik_bone_ids;

		/* 读取 IK 数据 */
		int ik_num = reader.Read<uint16>();
		std::vector<pmd_raw_ik> raw_iks(ik_num);
		for ( int i = 0; i < ik_num; i++ )
		{
			raw_iks[i].preamble = reader.Read<pmd_ik_preamble>();
			ik_bone_ids.insert(raw_iks[i].preamble.ik_bone_index);
			for ( int j = 0; j < raw_iks[i].preamble.ik_chain_length; j++ )
			{
				raw_iks[i].chain.push_back(reader.Read<uint16>());
			}
		}

		std::sort(raw_iks.begin(), raw_iks.end(), [] (const pmd_raw_ik& v1, const pmd_raw_ik& v2)->bool{
			size_t a = 0, b = 0;
			if ( v1.chain.size() > 0 ) a = v1.chain[0];
			if ( v2.chain.size() > 0 ) b = v2.chain[0];
			return a < b;
		});

		int center_bone_index = 0;
		for ( int i = 0; i < bone_num; i++ )
		{
			mmd_bone bone;
			memset(&bone, 0, sizeof(mmd_bone));

			pmd_bone& pb = raw_bones[i];

			bone.name = ShiftJISToUTF16String(pb.name);
			if ( bone.name == L"\x30BB\x30F3\x30BF\x30FC" )
			{
				center_bone_index = i;
			}
			bone.position = pb.position;
			bone.child_id = pb.child_id;
			bone.parent_id = (pb.parent_id != i) ? pb.parent_id : 65535;
			if ( bone.parent_id == -1 ) bone.parent_id = 65535;

			pmd_bone_types type = ( pmd_bone_types ) pb.type;

			/* 这不是普通的骨骼，而是 IK 骨骼 */
			if ( type == pmd_bone_types::PMD_BONE_IK || ik_bone_ids.count(i) > 0 )
			{
				bone.has_ik = true;
			}
			model->bones.push_back(bone);
		}

		/* 获取 IK 数据 */
		mmd_vector3d lo_limit = { 0, 0, 0 };
		mmd_vector3d hi_limit = { 0, 0, 0 };
		lo_limit.x = -mmd_math_const_pi;
		hi_limit.x = -0.5f / 180.0f * mmd_math_const_pi;

		for ( int i = 0; i < bone_num; i++ )
		{
			/* 跳过不是 IK 的骨骼 */
			if ( ik_bone_ids.count(i) <= 0 ) continue;

			int associated_ik_count = 0;
			for ( int j = 0; j < ik_num; j++ )
			{
				/* 获取该 IK 对应的 IK 骨骼 */
				if ( i != raw_iks[j].preamble.ik_bone_index ) continue;

				pmd_raw_ik& raw_ik = raw_iks[j];
				mmd_bone* bone = &(model->bones[i]);

				//if ( associated_ik_count == 0 )
				//{
				//	bone = &(model.bones[i]);
				//}
				//else
				//{
				//	mmd_bone* original_bone = &(model.bones[i]);
				//}

				bone->ik_info.target_index = raw_ik.preamble.ik_target_bone_index;
				bone->ik_info.ccd_iterate_limit = raw_ik.preamble.ccd_iterate_limit;
				bone->ik_info.ccd_angle_limit = raw_ik.preamble.ccd_angle_limit;

				/* 获取 IK 影响的骨骼链 */
				for ( int k = 0; k < raw_ik.chain.size(); k++ )
				{
					mmd_bone::IKLink link;
					link.index = raw_ik.chain[k];
					std::wstring ikname = model->bones[link.index].name;
					if ( ikname == L"\x5DE6\x3072\x3056" ||
						 ikname == L"\x53F3\x3072\x3056" )
					{
						link.has_limit = true;
						link.lo = lo_limit;
						link.hi = hi_limit;
					}
					else
					{
						link.has_limit = false;
					}
					bone->ik_info.ik_links.push_back(link);
				}
				associated_ik_count++;
			}
		}

		// TODO: 验证

		/* 读取变形数据 */
		int base_morph_index = -1;
		int face_num = reader.Read<uint16>();
		for ( int i = 0; i < face_num; i++ )
		{
			pmd_face_preamble fp = reader.Read<pmd_face_preamble>();

			mmd_morph morph;
			morph.name = ShiftJISToUTF16String(fp.name);
			morph.morph_category = ( MorphCategory ) fp.face_type;
			if ( morph.morph_category == MorphCategory::MORPH_CAT_SYSTEM )
			{
				base_morph_index = i;
			}
			/* 模型是通过顶点变形 */
			morph.morph_type = MorphType::MORPH_TYPE_VERTEX;

			for ( int j = 0; j < fp.vertex_num; j++ )
			{
				mmd_morph_data data;
				data.vertex_morph.vertex_index = reader.Read<uint32>();
				data.vertex_morph.offset = reader.Read<mmd_vector3d>();
				morph.morph_datas.push_back(data);
			}
			model->morphs.push_back(morph);
		}

		/* 这个有什么用呢？ */
		if ( base_morph_index != -1 )
		{
			mmd_morph& base_morph = model->morphs[base_morph_index];
			for ( int i = 0; i < face_num; i++ )
			{
				if ( i == base_morph_index ) continue;
				mmd_morph& morph = model->morphs[i];
				for ( int j = 0; j < morph.morph_datas.size(); j++ )
				{
					mmd_morph_data& data = morph.morph_datas[j];
					int morph_data_vertex_index = data.vertex_morph.vertex_index;
					data.vertex_morph.vertex_index = base_morph.morph_datas[morph_data_vertex_index].vertex_morph.vertex_index;
				}
			}
		}

		/* UNDONE */
		//size_t face_disp_list_num = reader.Read<uint8>();
		//for ( size_t i = 0; i < face_disp_list_num; ++i ) 
		//{
		//	reader.Read<uint16>();
		//}

		//size_t bone_name_list_num = reader.Read<uint8>();
		//for ( size_t i = 0; i < bone_name_list_num; ++i ) 
		//{
		//	reader.ReadStringA(50);
		//}

		//size_t bone_disp_num = reader.Read<uint32>();
		//for ( size_t i = 0; i < bone_disp_num; ++i ) 
		//{
		//	reader.Read<uint16>();
		//	reader.Read<uint8>();
		//}

		return model;
	}
}

