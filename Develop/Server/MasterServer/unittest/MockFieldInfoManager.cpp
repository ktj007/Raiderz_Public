#include "stdafx.h"
#include "MockFieldInfoManager.h"


void MockFieldInfoManager::AddFieldInfo( int nFieldID, const wchar_t* szFieldName, const wchar_t* szFieldFile, bool bDynamic, int nDefaultChannelCount )
{
	ZFieldInfo* pFieldInfo = new ZFieldInfo();
	pFieldInfo->m_nFieldID = nFieldID;
	pFieldInfo->m_strFieldName = szFieldName;
	pFieldInfo->m_strFieldFile = szFieldFile;
	pFieldInfo->m_bDynamic = bDynamic;
	pFieldInfo->m_nDefaultChannelCount = nDefaultChannelCount;

	m_FieldInfoMap.insert(ZFieldInfoMap::value_type(pFieldInfo->m_nFieldID, pFieldInfo));
}

void MockFieldInfoManager::AddFieldGroupInfo(int nFieldGroupID, int nFirstDynamicFieldID)
{
	ZFieldInfo* pFieldInfo = Find(nFirstDynamicFieldID);
	if (NULL == pFieldInfo) return;

	FIELD_GROUP_INFO* pFieldGroupInfo = new FIELD_GROUP_INFO();
	pFieldGroupInfo->nID = nFieldGroupID;
	pFieldGroupInfo->pStartFieldInfo = pFieldInfo;
	pFieldGroupInfo->vecFieldInfo.push_back(pFieldInfo);

	InsertFieldGroup(pFieldGroupInfo);
}