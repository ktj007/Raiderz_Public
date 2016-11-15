#include "stdafx.h"
#include "VEffectInfoMgr.h"

VEffectInfoMgr::VEffectInfoMgr()
: XEffectInfoMgr()
{

}

VEffectInfoMgr::~VEffectInfoMgr()
{

}

void VEffectInfoMgr::ParseEffectInfo( MXmlElement* pElement, MXml* pXml )
{
	ParseEffect(pElement, pXml);
}

void VEffectInfoMgr::LoadingFileInfo()
{
	m_vecEffectFileList.clear();

	vector<string> vecAddString;
	vecAddString.push_back("effect.scene.xml");
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	VFileName::GetFileList(V_PATH_EEFECT_TOOL, m_vecEffectFileList, &vecAddString, NULL, FA_FILE);
}

FILENAME_DATA* VEffectInfoMgr::GetEffectFileInfo( string& strName )
{
	// 파일은 소문자로 입력되어 있으므로 비교도 소문자로 변경후 한다.
	string strEffectFile = strName;
	_strlwr( (char*)strEffectFile.c_str());	

	for(vector<FILENAME_DATA>::iterator it = m_vecEffectFileList.begin(); it != m_vecEffectFileList.end(); ++it)
	{
		if(it->strPureFileName == strEffectFile)
			return &(*it);
	}

	return NULL;
}