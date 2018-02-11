#include "VmdReader.h"
#include "../Debug.h"

namespace mmd
{
	mmd_motion* VmdReader::ReadMotion(const std::string& filename)
	{
		FileReader reader(filename);

		if ( reader.HasData() == false ) return nullptr;

		/* 读取头部 */
		mmd_motion* motion = new mmd_motion;
		motion->magic = reader.ReadStringA(30);
		motion->name = ShiftJISToUTF16String(reader.ReadStringA(20));

		if ( strcmp(motion->magic.c_str(), "Vocaloid Motion Data 0002") )
		{
			Debug::Log("VmdReader::ReadMotion: File is not a VMD file.");
			return nullptr;
		}

		struct Block { int8 buffer[64]; } block;

		/* 读取动作数据 */
		int bone_motion_num = reader.Read<uint32>();
		int skip = 15 + sizeof( uint32 ) + sizeof( mmd_vector3d ) + sizeof( mmd_vector4d ) + sizeof(Block);

		for ( int i = 0; i < bone_motion_num; i++ )
		{
			vmd_bone bone;
			bone.bone_name	 = ShiftJISToUTF16String(reader.ReadStringA(15));
			bone.nframe		 = reader.Read<uint32>();
			bone.translation = reader.Read<mmd_vector3d>();
			bone.rotation	 = reader.Read<mmd_vector4d>();
			
			block = reader.Read<Block>();
			memcpy(bone.interpolator, block.buffer, sizeof(Block));

			motion->bones.push_back(bone);
		}

		/* 读取表情数据 */
		int morph_motion_num = reader.Read<uint32>();
		for ( int i = 0; i < morph_motion_num; i++ )
		{
			vmd_morph morph;
			morph.morph_name = ShiftJISToUTF16String(reader.ReadStringA(15));
			morph.nframe = reader.Read<uint32>();
			morph.weight = reader.Read<float>();

			motion->morphs.push_back(morph);
		}

		if ( reader.GetRemainedLength() > 0 )
		{
			struct CameraBlock { char buffer[24]; } camera_block;

			int camera_motion_num = reader.Read<uint32>();
			for ( int i = 0; i < camera_motion_num; i++ )
			{
				vmd_camera camera;
				camera.nframe = reader.Read<uint32>();
				camera.focal_length = reader.Read<float>();
				camera.position = reader.Read<mmd_vector3d>();
				camera.rotation = reader.Read<mmd_vector3d>();

				camera_block = reader.Read<CameraBlock>();
				memcpy(camera.interpolator, camera_block.buffer, 24);

				camera.fov = reader.Read<float>();
				camera.orthographic = reader.Read<uint8>();
				motion->camera.push_back(camera);
			}
		}
		return motion;
	}
}