#pragma once

namespace rs3 {

/// 시스템 정보
class RS_API RSystemInfo
{
	bool GetFileList(const char* path, const char* filter, vector<string>& outvecFiles);

public:
	RSystemInfo();
	~RSystemInfo() {}

	void			CheckAndDeleteCache();
};









} // namespace rs3