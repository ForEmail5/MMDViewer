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

		/* ����Ƿ��ȡģ�����ݳɹ� */
		if ( reader.HasData() == false )
		{
			succeed = false;
			Debug::LogAndWrite("Can't open model file: %s", filename.c_str());
			return nullptr;
		}

		std::string model_type = reader.ReadStringA(3);

		/* �����ļ�ָ�� */
		reader.Reset();

		/* ��ȡ pmx ģ�� */
		if ( strncmp(model_type.c_str(), "PMX", 3) == 0 )
		{
			return PmxReader::ReadModel(reader);
		}

		/* ��ȡ pmd ģ�� */
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

