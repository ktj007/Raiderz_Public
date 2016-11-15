#include "stdafx.h"
#include "CSInfoMgr.h"
#include "CSInfo.h"
#include "MLocale.h"


string CSInfoMgr::m_strMaiet = "maiet";

CSInfoMgr::CSInfoMgr()
{

}

CSInfoMgr::~CSInfoMgr()
{
	Clear();
}

void CSInfoMgr::Clear()
{
	for each (MAP_INFO::value_type val in m_mapInfo)
	{
		SAFE_DELETE(val.second);
	}

	m_mapInfo.clear();
}

bool CSInfoMgr::Load(const TCHAR* szFileName, const TCHAR* szTagName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(CSInfoMgr::m_strMaiet).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), MLocale::ConvTCHARToAnsi(szTagName).c_str()))
		{
			CSInfo* pNewInfo = New();
			if (true == Parse(pElement, &xml, pNewInfo))
			{
				Insert(pNewInfo);
			}
			else
			{
				SAFE_DELETE(pNewInfo);
			}
		}
	}

	return true;
}

void CSInfoMgr::Insert(CSInfo* pInfo)
{
	if (NULL == pInfo) return;
	if (true == IsExist(pInfo->m_nID)) return;

	m_mapInfo.insert(MAP_INFO::value_type(pInfo->m_nID, pInfo));
}

CSInfo* CSInfoMgr::Get(int nID)
{
	MAP_INFO::iterator itor = m_mapInfo.find(nID);
	if (m_mapInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool CSInfoMgr::IsExist(int nID)
{
	if (NULL ==  Get(nID)) return false;

	return true;
}
