#pragma once

#include "XAnimationInfoMgr.h"
#include "VAnimationEventInfo.h"
#include "VAnimationList.h"

class VErrorMsg;

struct BLEND_INFO_TOOL
{
	string						m_strSource;
	vector<string>				m_vecTarget;
};

class VAnimationInfoMgr : public XAnimationInfoMgr
{
public:
	VAnimationEventInfo			m_AnimationEventInfo;
	VAnimationList				m_AnimationList;

	vector<BLEND_INFO_TOOL>		m_vecAnimationBlendInfoForTool;

private:
	void						ReSetAnimationBlendInfo();
	void						ReSetAnimationBlendData(string strSource, string strTarget);
	BLEND_INFO_TOOL*			GetAnimationBlendInfo(string strSource);

public:
	VAnimationInfoMgr();
	virtual ~VAnimationInfoMgr();

	virtual	bool				LoadInfo(tstring& strMeshFileName, VErrorMsg* pErrorMsg);
	void						DeleteInfo();
};

