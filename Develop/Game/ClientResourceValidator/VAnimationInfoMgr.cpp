#include "stdafx.h"
#include "VAnimationInfoMgr.h"
#include "XAnimationInfoLoader.h"

VAnimationInfoMgr::VAnimationInfoMgr()
:XAnimationInfoMgr()
{

}

VAnimationInfoMgr::~VAnimationInfoMgr()
{

}

bool VAnimationInfoMgr::LoadInfo( tstring& strMeshFileName, VErrorMsg* pErrorMsg )
{
	if(XAnimationInfoMgr::LoadInfo(strMeshFileName) == false)
		CheckExistXmlFile(strMeshFileName + ANIMATION_INFO_EXT, pErrorMsg);
	else
		ReSetAnimationBlendInfo();

	if(m_AnimationList.LoadInfo(strMeshFileName) == false)
		CheckExistXmlFile(strMeshFileName + ANIMATION_LIST_EXT, pErrorMsg);

	if(m_AnimationEventInfo.LoadInfo(strMeshFileName) == false)
		CheckExistXmlFile(strMeshFileName + ANIMATION_EVENT_INFO_EXT, pErrorMsg);

	return true;
}

void VAnimationInfoMgr::DeleteInfo()
{
	m_AnimationEventInfo.DeleteInfo();
}

void VAnimationInfoMgr::ReSetAnimationBlendInfo()
{
	for(XAnimationBlendInfo::AnimationBlendData::iterator it = m_AnimationBlendInfo.GetAnimationBlendData()->begin(); it != m_AnimationBlendInfo.GetAnimationBlendData()->end(); ++it)
	{
		XAnimationBlendInfo::AnimationBlendPair blendPair = it->first;

		ReSetAnimationBlendData(blendPair.first, blendPair.second);
	}
}

void VAnimationInfoMgr::ReSetAnimationBlendData( string strSource, string strTarget )
{
	BLEND_INFO_TOOL* pBlendInfo = GetAnimationBlendInfo(strSource);
	if(pBlendInfo)
	{
		pBlendInfo->m_vecTarget.push_back(strTarget);
	}
	else
	{
		BLEND_INFO_TOOL blendInfo;
		blendInfo.m_strSource = strSource;
		blendInfo.m_vecTarget.push_back(strTarget);
		m_vecAnimationBlendInfoForTool.push_back(blendInfo);
	}
}

BLEND_INFO_TOOL* VAnimationInfoMgr::GetAnimationBlendInfo( string strSource )
{
	for(vector<BLEND_INFO_TOOL>::iterator it = m_vecAnimationBlendInfoForTool.begin(); it != m_vecAnimationBlendInfoForTool.end(); ++it)
	{
		if(it->m_strSource == strSource)
			return &(*it);
	}

	return NULL;
}