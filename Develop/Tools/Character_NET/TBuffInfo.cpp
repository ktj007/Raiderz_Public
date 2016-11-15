#include "stdafx.h"
#include "TBuffInfo.h"
#include "MLocale.h"

///////////////////////////////////////////////////////////////////////////////
TBuffInfoMgr::TBuffInfoMgr()
:XBuffInfoMgr()
{
	m_bBuffEffectFileLoadError = false;
}

TBuffInfoMgr::~TBuffInfoMgr()
{
}

void TBuffInfoMgr::Init()
{
	if(Load(FILENAME_BUFFINFO_TOOL, FILENAME_BUFFEFFECTINFO_TOOL, FILENAME_ENCHANT_BUFFEFFECTINFO_TOOL) == false)
		m_bBuffEffectFileLoadError = true;
}

XBuffInfo * TBuffInfoMgr::GetBuffInfo( int nBuffID )
{
	return Get(nBuffID);
}

XBuffEffectData * TBuffInfoMgr::GetBuffEffect( int nBuffID )
{
	XBuffInfo* pBuffInfo = Get(nBuffID);
	if(pBuffInfo)
	{
		return &(pBuffInfo->m_BuffEffect);
	}

	return NULL;
}

void TBuffInfoMgr::GetBuffInfoList( vector<XBuffInfo*>& vecBuffInfo )
{
	// 버프 정보 가져오기
	for(map<int , CSBuffInfo* > ::iterator it = begin(); it != end(); it++)
	{
		XBuffInfo* pBuffInfo = static_cast<XBuffInfo*>(it->second);
		vecBuffInfo.push_back(pBuffInfo);
	}
}

XBuffEffectInfo * TBuffInfoMgr::GetBuffEffectInfo( int nBuffID, BUFF_EFFECT_TYPE nType, int nIndex )
{
	XBuffInfo* pBuffInfo = Get(nBuffID);

	int nCount = 0;
	for(vector<XBuffEffectInfo *>::iterator itEffect = pBuffInfo->m_BuffEffect.begin(); itEffect != pBuffInfo->m_BuffEffect.end(); itEffect++)
	{
		if((*itEffect)->m_nBuffEffectType == nType)
		{
			if(nCount == nIndex)
			{
				return *itEffect;
			}

			nCount++;
		}
	}

	return NULL;
}

bool TBuffInfoMgr::ReLoad()
{
	Clear();

	Init();

	return !m_bBuffEffectFileLoadError;
}

bool TBuffInfoMgr::CheckTalentInfoModify()
{
	return ReLoad();
}

void TBuffInfoMgr::GetEnchantBuffIDList( vector<int>& vecEnchantBuffID )
{
	for(iterator itor = begin(); itor != end(); ++itor)
	{
		XBuffInfo* pBuffInfo = (XBuffInfo*)(itor->second);
		if(pBuffInfo->m_EnchantBuffEffect.empty() == false || pBuffInfo->m_EnchantBuffEffect.m_DefaultEffect.empty() == false)
			vecEnchantBuffID.push_back(itor->first);
	}
}

XEnchantBuffEffectData* TBuffInfoMgr::GetEnchantBuffEffectData( int nBuffID )
{
	XBuffInfo* pBuffInfo = Get(nBuffID);
	if(pBuffInfo)
	{
		return &(pBuffInfo->m_EnchantBuffEffect);
	}

	return NULL;
}