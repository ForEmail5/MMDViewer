#ifndef MMD_READER_H
#define MMD_READER_H

#include "MmdCommon.h"

namespace mmd
{
	class MMDReader
	{
	public:
		static mmd_model* ReadModel(const std::string& filename);

		static mmd_motion* ReadMotion(const std::string& filename);

		static bool GetReadStatus();
	};
}

#endif