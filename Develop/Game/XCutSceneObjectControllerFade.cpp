#include "stdafx.h"
#include "XCutSceneObjectControllerFade.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

XCutSceneObjectControllerFade::XCutSceneObjectControllerFade( XCutSceneObject* pOwner )
: XCutSceneObjectController(pOwner, E_FADE)
{

}

XCutSceneObjectControllerFade::~XCutSceneObjectControllerFade()
{

}

bool XCutSceneObjectControllerFade::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	float fAlpha;

	MXmlElement* pFadeKeyElement = pControllerElement->FirstChildElement("Fadekey");
	while (pFadeKeyElement)
	{
		_Attribute(fTime,	pFadeKeyElement, "time");
		_Attribute(fAlpha,	pFadeKeyElement, "Alpha");

		AddKey(fTime, fAlpha);
		pFadeKeyElement = pFadeKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerFade::GetKeyInfo( int nIdx, float& _rOutTime, float& _rOutAlpha)
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	_rOutTime		= rKeyInfo.fTime;
	_rOutAlpha		= rKeyInfo.fAlpha;
}

void XCutSceneObjectControllerFade::SetKeyInfo( int nIdx, float _fTime, float _fAlpha)
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime		= _fTime;
	rKeyInfo.fAlpha		= _fAlpha;
}

void XCutSceneObjectControllerFade::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerFade::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pFadeElement = new MXmlElement("fade");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pFadeKey = new MXmlElement("Fadekey");
		_SetAttribute(pFadeKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pFadeKey, "Alpha", m_vKeyInfos[i].fAlpha);

		pFadeElement->LinkEndChild(pFadeKey);
	}

	pObjectElement->LinkEndChild(pFadeElement);
}

void XCutSceneObjectControllerFade::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	if(m_pOwner->GetEntity())
	{
		float fFadingAlpha = MMath::LinearInterpolation(pCurrent->fAlpha, pNext->fAlpha, fIntervalRatio);
		m_pOwner->GetEntity()->FadeUpdate(fFadingAlpha);
	}
}

void XCutSceneObjectControllerFade::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if(m_pOwner->GetEntity())	m_pOwner->GetEntity()->FadeEnd();
}

void XCutSceneObjectControllerFade::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
}
