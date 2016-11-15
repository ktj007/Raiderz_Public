#pragma once

#include "XOverlayAnimationInfo.h"
#include "XAnimationBlendInfo.h"
#include "XAnimationHitSoundInfo.h"
#include "XReActionAnimationInfo.h"

class XAnimationInfoMgr
{
protected:
	tstring						m_strFileName;

	bool						m_bLoad;					// 로딩을 한번이라도 시도를 해봤나?

public:
	XOverlayAnimationInfo		m_OverlayAnimationInfo;
	XAnimationBlendInfo			m_AnimationBlendInfo;
	XAnimationHitSoundInfo		m_AnimationHitSoundInfo;
	XReActionAnimationInfo		m_ReActionAnimationInfo;

public:
	XAnimationInfoMgr() : m_bLoad(false)			{}
	virtual ~XAnimationInfoMgr()					{}

	virtual	bool				LoadInfo(tstring& strMeshFileName);
};
