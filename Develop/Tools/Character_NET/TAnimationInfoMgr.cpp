#include "stdafx.h"
#include "MXml.h"
#include "TAnimationInfoMgr.h"
#include "XAnimationInfoLoader.h"

void TAnimationInfoMgr::SaveXml()
{
	if(m_strFileName.empty())
		return;

	vector<string> vecAnimationList;
	g_pMainApp->GetAnimationList(vecAnimationList);

	SaveXml(vecAnimationList);
}

void TAnimationInfoMgr::SaveXml( vector<string>& vecAnimationList )
{
	if(m_strFileName.empty())
		return;

	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.LinkEndChild(pDec);

	MXmlElement *pMaiet = new MXmlElement(ANIMATION_INFO_XML_TAG_MAIET);

	for(vector<string>::iterator it = vecAnimationList.begin(); it != vecAnimationList.end(); ++it)
	{
		MXmlElement *pAnimationXml = new MXmlElement(ANIMATION_INFO_XML_TAG_ANIMATION);	
		_SetAttribute(pAnimationXml, ANIMATION_INFO_XML_ATTR_ANIMATION_NAME, it->c_str());

		bool bSave = false;
		// 블렌딩
		bSave |= ParseBlendInfo(pAnimationXml, &xml, (*it));
		// 오버레이
		bSave |= ParseOverlayInfo(pAnimationXml, &xml, (*it));
		// 히트 사운드
		bSave |= ParseHitSoundInfo(pAnimationXml, &xml, (*it));
		// 리액션
		bSave |= ParseReActionInfo(pAnimationXml, &xml, (*it));

		if(bSave)
			pMaiet->LinkEndChild(pAnimationXml);
		else
			SAFE_DELETE(pAnimationXml);
	}

	xml.LinkEndChild(pMaiet);

	xml.SaveFile(m_strFileName.c_str());
}

bool TAnimationInfoMgr::ParseBlendInfo( MXmlElement* pElement, MXml* pXml, string& strAniName )
{
	ANIMATION_BLEND_LIST blendList;
	m_toolAnimationBlendInfo.GetAnimationBlendList(strAniName, blendList);
	if(blendList.vecTargetList.empty())
		return false;

	bool bSave = false;
	for(vector<ANIMATION_BLEND_DATA>::iterator itInfo = blendList.vecTargetList.begin(); itInfo != blendList.vecTargetList.end(); ++itInfo)
	{
		MXmlElement *pBlendAnimationXml = new MXmlElement(ANIMATION_BLEND_INFO_XML_TAG_TRANSITION);

		_SetAttribute(pBlendAnimationXml, ANIMATION_BLEND_INFO_XML_ATTR_TARGET,		itInfo->strTarget.c_str());
		_SetAttribute(pBlendAnimationXml, ANIMATION_BLEND_INFO_XML_ATTR_DURATION,	itInfo->fDurationTime);

		pElement->LinkEndChild(pBlendAnimationXml);

		bSave = true;
	}

	return bSave;
}

bool TAnimationInfoMgr::ParseOverlayInfo( MXmlElement* pElement, MXml* pXml, string& strAniName )
{
	OVERLAY_ANIMATION_LIST * pOverlayList = m_toolOverlayAnimationInfo.Get(strAniName);
	if(pOverlayList == NULL)
		return false;

	bool bSave = false;
	for(vector<OVERLAY_ANIMATION_INFO>::iterator itInfo = pOverlayList->vecOverlayAnimationList.begin(); itInfo != pOverlayList->vecOverlayAnimationList.end(); ++itInfo)
	{
		MXmlElement *pOverlayAnimationXml = new MXmlElement(ANIMATION_OVERLAY_XML_TAG_OVAERLAYANIMATION);

		_SetAttribute(pOverlayAnimationXml, ANIMATION_OVERLAY_XML_ATTR_NAME,		itInfo->strOverlayAnimation);
		_SetAttribute(pOverlayAnimationXml, ANIMATION_OVERLAY_XML_ATTR_BONE_NAM,	itInfo->strBoneName);

		pElement->LinkEndChild(pOverlayAnimationXml);

		bSave = true;
	}

	return bSave;
}

bool TAnimationInfoMgr::ParseHitSoundInfo( MXmlElement* pElement, MXml* pXml, string& strAniName )
{
	string strSoundName = m_AnimationHitSoundInfo.Get(strAniName);
	if(strSoundName == "")
		return false;

	MXmlElement *pAnimationHitSoundXml = new MXmlElement(ANIMATION_HIT_SOUND_XML_TAG_HITSOUND);

	_SetAttribute(pAnimationHitSoundXml, ANIMATION_HIT_SOUND_XML_ATTR_SOUND_NAME,		strSoundName);

	pElement->LinkEndChild(pAnimationHitSoundXml);

	return true;
}

bool TAnimationInfoMgr::ParseReActionInfo( MXmlElement* pElement, MXml* pXml, string& strAniName )
{
	ReAction_DATA * pReActionData = m_toolReActionAnimationInfo.Get(strAniName);
	if(pReActionData == NULL)
		return false;

	MXmlElement *pReActionXml = new MXmlElement(ANIMATION_REACTION_XML_TAG_REACTIONANIMATION);
	
	_SetAttribute(pReActionXml, ANIMATION_REACTION_XML_ATTR_REACTION_NAME,		pReActionData->strReActionName);
	_SetAttribute(pReActionXml, ANIMATION_REACTION_XML_ATTR_WEIGHT,				pReActionData->fWeight);
	_SetAttribute(pReActionXml, ANIMATION_REACTION_XML_ATTR_SPEED,				pReActionData->fSpeed);
	_SetAttribute(pReActionXml, ANIMATION_REACTION_XML_WEAPON_BONE_NAME,		pReActionData->strWeaponBoneName);
	_SetAttribute(pReActionXml, ANIMATION_REACTION_XML_ATTR_DISABLE,			pReActionData->bDisable);

	pElement->LinkEndChild(pReActionXml);

	return true;
}

bool TAnimationInfoMgr::LoadInfo( string& strMeshFileName )
{
	m_strFileName = strMeshFileName + ANIMATION_INFO_EXT;
	XAnimationInfoLoader infoLoader;
	return infoLoader.LoadXml(m_strFileName, &m_toolAnimationBlendInfo, &m_toolOverlayAnimationInfo, &m_AnimationHitSoundInfo, &m_toolReActionAnimationInfo);
}
