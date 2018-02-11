#include "MmdReader.h"
#include "PmxReader.h"
#include "PmdReader.h"
#include "VmdReader.h"

#include "../Debug.h"

namespace mmd
{
	static bool succeed = false;

	mmd_model* MMDReader::ReadModel(const std::string& filename)
	{
		succeed = true;
		FileReader reader(filename);

		/* 检测是否获取模型数据成功 */
		if ( reader.HasData() == false )
		{
			succeed = false;
			Debug::LogAndWrite("Can't open model file: %s", filename.c_str());
			return nullptr;
		}

		std::string model_type = reader.ReadStringA(3);

		/* 重置文件指针 */
		reader.Reset();

		/* 读取 pmx 模型 */
		if ( strncmp(model_type.c_str(), "PMX", 3) == 0 )
		{
			return PmxReader::ReadModel(reader);
		}

		/* 读取 pmd 模型 */
		if ( strncmp(model_type.c_str(), "Pmd", 3) == 0 )
		{
			return PmdReader::ReadModel(reader);
		}
	}

	mmd_motion* MMDReader::ReadMotion(const std::string& filename)
	{
		return VmdReader::ReadMotion(filename);
	}

	bool MMDReader::GetReadStatus()
	{
		return succeed;
	}
}

