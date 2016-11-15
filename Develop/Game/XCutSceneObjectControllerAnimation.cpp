#include "stdafx.h"
#include "XCutSceneObjectControllerAnimation.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerAnimation::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	std::tstring strAniName;

	MXmlElement* pAniKeyElement = pControllerElement->FirstChildElement("anikey");
	while (pAniKeyElement)
	{
		_Attribute(fTime, pAniKeyElement, "time");
		_Attribute(strAniName, pAniKeyElement, "name");

		AddKey(fTime, strAniName);
		pAniKeyElement = pAniKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerAnimation::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pAnimationElement = new MXmlElement("ani");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pAniKey = new MXmlElement("anikey");
		_SetAttribute(pAniKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pAniKey, "name", MLocale::ConvTCHARToAnsi(m_vKeyInfos[i].strAniName.c_str()).c_str());
		pAnimationElement->LinkEndChild(pAniKey);
	}

	pObjectElement->LinkEndChild(pAnimationElement);
}

void XCutSceneObjectControllerAnimation::GetKeyInfo( int nIdx, float& _rOutTime, std::tstring& _rAniName )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	_rOutTime = rKeyInfo.fTime;
	_rAniName = rKeyInfo.strAniName;
}

void XCutSceneObjectControllerAnimation::SetKeyInfo( int nIdx, float _fTime, const std::tstring& _rAniName )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime = _fTime;
	rKeyInfo.strAniName = _rAniName;
}

void XCutSceneObjectControllerAnimation::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerAnimation::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	if (pCurrent->bUpdated)
		return;

	pCurrent->bUpdated = true;
	m_pOwner->GetEntity()->SetAnimation(pCurrent->strAniName);
}

void XCutSceneObjectControllerAnimation::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if (pProcKey->bUpdated)
		return;

	pProcKey->bUpdated = true;
	m_pOwner->GetEntity()->SetAnimation(pProcKey->strAniName);
}

void XCutSceneObjectControllerAnimation::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
	for (std::vector< SKeyInfo >::iterator itr = m_vKeyInfos.begin(); itr != m_vKeyInfos.end(); ++itr)
	{
		itr->bUpdated = false;
	}
}
