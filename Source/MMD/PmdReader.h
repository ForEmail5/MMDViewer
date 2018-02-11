#ifndef PMD_READER_H
#define PMD_READER_H

#include "PmdType.h"
#include "FileReader.h"

namespace mmd
{
	class PmdReader
	{
	public:
		static mmd_model* ReadModel(FileReader& reader);
	};
}

#endif