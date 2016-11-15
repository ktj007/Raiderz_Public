#include "stdafx.h"
#include "VCameraInfoMgr.h"

VCameraInfoMgr::VCameraInfoMgr()
{

}

VCameraInfoMgr::~VCameraInfoMgr()
{

}

void VCameraInfoMgr::GetCameraFileLIst()
{
	m_vecCameraFileList.clear();

	vector<string> vecAddString;
	vecAddString.push_back("elu");
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	VFileName::GetFileList(V_PATH_CAMERA_TOOL, m_vecCameraFileList, &vecAddString, NULL, FA_FILE);
}

void VCameraInfoMgr::LoadingInfo()
{
	GetCameraFileLIst();
}

FILENAME_DATA* VCameraInfoMgr::GetCameraFileInfo( string& strName )
{
	for(vector<FILENAME_DATA>::iterator it = m_vecCameraFileList.begin(); it != m_vecCameraFileList.end(); ++it)
	{
		if(it->strPureFileName == strName)
			return &(*it);
	}

	return NULL;
}