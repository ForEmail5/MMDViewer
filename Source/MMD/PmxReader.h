#ifndef PMX_READER_H
#define PMX_READER_H

#include "PmxType.h"
#include "FileReader.h"

namespace mmd
{
	class PmxReader
	{
	public:
		static mmd_model* ReadModel(FileReader& reader);
	};
}

#endif