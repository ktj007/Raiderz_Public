#pragma once

#include "MSingleton.h"

namespace minet {

class MiNetLogger : public MSingleton<MiNetLogger>
{
public:
	MiNetLogger() {}
	~MiNetLogger() {}
	void Log(const char* szText, ...);
	void DebugLog(const char* szText, ...);
};

#define minet_log			MiNetLogger::GetInstance().Log
#define minet_logd			MiNetLogger::GetInstance().DebugLog

} // namespace minet {