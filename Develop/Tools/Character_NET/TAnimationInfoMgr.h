#pragma once

#include "XAnimationInfoMgr.h"
#include "TOverlayAnimationInfo.h"
#include "TAnimationBlendInfo.h"
#include "TReActionAnimationInfo.h"

class TAnimationInfoMgr : public XAnimationInfoMgr
{
public:
	TOverlayAnimationInfo		m_toolOverlayAnimationInfo;
	TAnimationBlendInfo			m_toolAnimationBlendInfo;
	TReActionAnimationInfo		m_toolReActionAnimationInfo;

private:
	bool ParseBlendInfo(MXmlElement* pElement, MXml* pXml, string&  strAniName);
	bool ParseOverlayInfo(MXmlElement* pElement, MXml* pXml, string&  strAniName);
	bool ParseHitSoundInfo(MXmlElement* pElement, MXml* pXml, string&  strAniName);
	bool ParseReActionInfo(MXmlElement* pElement, MXml* pXml, string&  strAniName);

public:
	TAnimationInfoMgr() : XAnimationInfoMgr()		{}
	virtual ~TAnimationInfoMgr()					{}

	virtual	bool				LoadInfo(string& strMeshFileName);

	void						SaveXml();
	void						SaveXml(vector<string>& vecAnimationList);
};
