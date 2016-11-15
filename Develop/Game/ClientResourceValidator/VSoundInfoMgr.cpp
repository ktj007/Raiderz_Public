#include "stdafx.h"
#include "VSoundInfoMgr.h"


bool VSoundResult::LoadInfo()
{
	MXml xml;		
	if (!xml.LoadFile(V_FILENAME_RESULT_SOUND)) 
		return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(STRINGS_XML_TAG_MAIET).FirstChildElement().Element();

	string strSound;
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		_Attribute(strSound,	pElement, STRINGS_XML_ATTR_STRING, &xml);

		m_vecSoundResult.push_back(strSound);
	}
	return true;
}

bool VSoundResult::CheckSoundDataValidator( string strSoundName )
{
	for(vector<string>::iterator it = m_vecSoundResult.begin(); it != m_vecSoundResult.end(); ++it)
	{
		if(strSoundName == *it)
			return true;

		string strMsg = *it;
		size_t pos = strMsg.find(_T("male"));
		strMsg.replace(pos, 4, _T("female"));

		if(strMsg == strSoundName)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

VSoundInfoMgr::VSoundInfoMgr()
{

}

VSoundInfoMgr::~VSoundInfoMgr()
{

}

void VSoundInfoMgr::GetSoundFileLIst()
{
	m_vecSoundFileList.clear();

	vector<string> vecAddString;
	vecAddString.push_back("ogg");
	vecAddString.push_back("wav");
	//------------------------------------------------------------------------
	// 파일 리스트 가져오기
	VFileName::GetFileList(V_PATH_SOUND_TOOL, m_vecSoundFileList, &vecAddString, NULL, FA_FILE);
}

bool VSoundInfoMgr::LoadingInfo()
{
	GetSoundFileLIst();

	m_ResultMsg.LoadInfo();

	return true;
}

RSampleInfo* VSoundInfoMgr::GetSampleInfo( string& strName )
{
	for(map< string, RSampleInfo>::iterator it = m_mapSoundInfo.begin(); it != m_mapSoundInfo.end(); ++it)
	{
		if(it->first == strName)
			return &(it->second);
	}

	return NULL;
}