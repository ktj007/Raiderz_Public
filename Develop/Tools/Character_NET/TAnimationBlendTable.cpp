#include "stdafx.h"
#include "TAnimationBlendTable.h"
#include "RMeshAnimationMgr.h"
#include "TAnimationBlendInfo.h"

TAnimationBlendTable::TAnimationBlendTable()
: m_pActor(NULL)
, m_pAnimationBlendInfo(NULL)
{

}

TAnimationBlendTable::~TAnimationBlendTable()
{

}

ANIMATION_BLEND_LIST * TAnimationBlendTable::GetAnimationBlendList( string& strSource )
{
	for(vector<ANIMATION_BLEND_LIST>::iterator it = m_vecAnimationBlendList.begin(); it != m_vecAnimationBlendList.end(); ++it)
	{
		if(it->strSource == strSource)
			return &(*it);
	}

	return NULL;
}

void TAnimationBlendTable::SetActor( TCharacter* pActor )
{
	m_vecAnimationBlendList.clear();
	m_pActor = pActor;

	if(m_pActor == NULL ||
		m_pActor->m_pMesh == NULL)
		return;

	m_pAnimationBlendInfo = &(m_pActor->GetAnimationInfoMgr()->m_toolAnimationBlendInfo);
	TAnimationBlendInfo::AnimationBlendData* pAnimationBlendData = m_pAnimationBlendInfo->GetAnimationBlendData();

	for( TAnimationBlendInfo::AnimationBlendData::const_iterator i = pAnimationBlendData->begin(); i != pAnimationBlendData->end(); ++i )
	{
		RAnimation* pSourceAnimation = m_pActor->m_pMesh->m_AniMgr.GetAnimation(i->first.first.c_str());
		RAnimation* pTargetAnimation = m_pActor->m_pMesh->m_AniMgr.GetAnimation(i->first.second.c_str());
		if(pSourceAnimation == NULL ||
			pTargetAnimation == NULL)
			continue;

		SetBlendAnimationData(pSourceAnimation->GetAliasName(), pTargetAnimation->GetAliasName(), i->second.fDurationTime);
	}
}

void TAnimationBlendTable::SetBlendAnimationData( string strSourceAni, string strTargetAni, float fTime )
{
	ANIMATION_BLEND_LIST * pList = GetAnimationBlendList(strSourceAni);
	if(pList == NULL)
	{
		ANIMATION_BLEND_LIST newList;
		newList.strSource = strSourceAni;
		m_vecAnimationBlendList.push_back(newList);	

		pList = GetAnimationBlendList(strSourceAni);
	}

	ANIMATION_BLEND_DATA blenData;
	blenData.strTarget		= strTargetAni;
	blenData.fDurationTime	= fTime;

	pList->vecTargetList.push_back(blenData);
}

void TAnimationBlendTable::InsertBlendAnimationData( string strSourceAni, string strTargetAni, float fTime )
{
	SetBlendAnimationData(strSourceAni, strTargetAni, fTime);

	if(m_pActor == NULL ||
		m_pActor->m_pMesh == NULL)
		return;

	m_pAnimationBlendInfo->SetAnimationTransData(strSourceAni, strTargetAni, fTime);
}

void TAnimationBlendTable::DeleteBlendAnimationData( string strSourceAni, string strTargetAni )
{
	DelBlendAnimationData(strSourceAni, strTargetAni);

	if(m_pActor == NULL ||
		m_pActor->m_pMesh == NULL)
		return;

	m_pAnimationBlendInfo->DeleteAnimationTransType(strSourceAni, strTargetAni);
}

void TAnimationBlendTable::DelBlendAnimationData( string strSourceAni, string strTargetAni )
{
	ANIMATION_BLEND_LIST * pList = GetAnimationBlendList(strSourceAni);
	if(pList == NULL)
		return;

	for(vector<ANIMATION_BLEND_DATA>::iterator it = pList->vecTargetList.begin(); it != pList->vecTargetList.end(); ++it)
	{
		if(it->strTarget == strTargetAni)
		{
			pList->vecTargetList.erase(it);
			return;
		}
	}
}

ANIMATION_BLEND_DATA * TAnimationBlendTable::GetAnimationBlendData( string& strSource, string& strTarget )
{
	ANIMATION_BLEND_LIST * pList = GetAnimationBlendList(strSource);
	if(pList == NULL)
		return NULL;

	for(vector<ANIMATION_BLEND_DATA>::iterator it = pList->vecTargetList.begin(); it != pList->vecTargetList.end(); ++it)
	{
		if(it->strTarget == strTarget)
			return &(*it);
	}

	return NULL;
}

void TAnimationBlendTable::ModifyBlendAnimationData( string strSourceAni, string strTargetAni, string strModifyTargetAni, float fModifyTime )
{
	ReSetBlendAnimationData(strSourceAni, strTargetAni, strModifyTargetAni, fModifyTime);

	if(m_pActor == NULL ||
		m_pActor->m_pMesh == NULL)
		return;

	m_pAnimationBlendInfo->DeleteAnimationTransType(strSourceAni, strTargetAni);
	m_pAnimationBlendInfo->SetAnimationTransData(strSourceAni, strModifyTargetAni, fModifyTime);
}

void TAnimationBlendTable::ReSetBlendAnimationData( string strSourceAni, string strTargetAni, string strModifyTargetAni, float fModifyTime )
{
	ANIMATION_BLEND_DATA * pData = GetAnimationBlendData(strSourceAni, strTargetAni);
	if(pData == NULL)
	{
		SetBlendAnimationData(strSourceAni, strModifyTargetAni, fModifyTime);
		return;
	}

	pData->strTarget		= strModifyTargetAni;
	pData->fDurationTime	= fModifyTime;
}

void TAnimationBlendTable::SaveBlendInfo()
{
	m_pActor->GetAnimationInfoMgr()->SaveXml();
}