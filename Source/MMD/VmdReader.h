#ifndef VMD_READER_H
#define VMD_READER_H

#include "MmdCommon.h"
#include "FileReader.h"

namespace mmd
{
	class VmdReader
	{
	public:
		static mmd_motion* ReadMotion(const std::string& filename);
	};
}

#endif