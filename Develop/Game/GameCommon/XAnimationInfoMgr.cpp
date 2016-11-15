#include "stdafx.h"
#include "MXml.h"
#include "XAnimationInfoMgr.h"
#include "XAnimationInfoLoader.h"

bool XAnimationInfoMgr::LoadInfo( tstring& strMeshFileName )
{	
	// 로딩이 되어 있나?
	if(m_bLoad)
		return true;

	m_bLoad = true;
	m_strFileName = strMeshFileName + ANIMATION_INFO_EXT;
	XAnimationInfoLoader infoLoader;
	return infoLoader.LoadXml(m_strFileName, &m_AnimationBlendInfo, &m_OverlayAnimationInfo, &m_AnimationHitSoundInfo, &m_ReActionAnimationInfo);
}
