#pragma once

#include "VFileName.h"

class VCameraInfoMgr
{
private:
	vector<FILENAME_DATA>				m_vecCameraFileList;

private:
	void								GetCameraFileLIst();

public:
	VCameraInfoMgr();
	virtual ~VCameraInfoMgr();

	void								LoadingInfo();

	FILENAME_DATA*						GetCameraFileInfo(string& strName);
};

