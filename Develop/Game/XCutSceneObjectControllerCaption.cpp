#include "stdafx.h"
#include "XCutSceneObjectControllerCaption.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerCaption::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	float fDurationTime;
	string strCaptionScript;

	MXmlElement* pCaptionKeyElement = pControllerElement->FirstChildElement("captionkey");
	while (pCaptionKeyElement)
	{
		_Attribute(fTime, pCaptionKeyElement, "time");
		_Attribute(fDurationTime, pCaptionKeyElement, "durationtime");
		_Attribute(strCaptionScript, pCaptionKeyElement, "script");

#ifdef _UNICODE		
		AddKey(fTime, fDurationTime, MLocale::ConvUTF8ToUTF16(strCaptionScript.c_str()));
#else
		AddKey(fTime, fDurationTime, MLocale::ConvUTF8ToAnsi(strCaptionScript.c_str()));
#endif


		pCaptionKeyElement = pCaptionKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerCaption::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pCaptionElement = new MXmlElement("caption");

#ifdef _UNICODE
	_SetAttribute(pCaptionElement, "font", MLocale::ConvUTF16ToUTF8(m_strFontName.c_str()));
#else
	_SetAttribute(pCaptionElement, "font", MLocale::ConvAnsiToUTF8(m_strFontName.c_str()));
#endif

	_SetAttribute(pCaptionElement, "size", m_nSize);
	_SetAttribute(pCaptionElement, "bold", m_bBold);
	_SetAttribute(pCaptionElement, "italic", m_bItalic);


	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pCaptionKey = new MXmlElement("captionkey");
		_SetAttribute(pCaptionKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pCaptionKey, "durationtime", m_vKeyInfos[i].fDurationTime);

#ifdef _UNICODE
		_SetAttribute(pCaptionKey, "script", MLocale::ConvUTF16ToUTF8(m_vKeyInfos[i].strScript.c_str()));
#else
		_SetAttribute(pCaptionKey, "script", MLocale::ConvAnsiToUTF8(m_vKeyInfos[i].strScript.c_str()));
#endif

		pCaptionElement->LinkEndChild(pCaptionKey);
	}

	pObjectElement->LinkEndChild(pCaptionElement);
}

void XCutSceneObjectControllerCaption::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();

	if(m_pOwner->GetEntity())
		m_pOwner->GetEntity()->HideCaption();

	m_CurrentCaption.Init();
}

void XCutSceneObjectControllerCaption::GetKeyInfo( int nIdx, float& _rOutTime, float& _rOutDurationTime, std::tstring& _rScript )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	_rOutTime				= rKeyInfo.fTime;
	_rOutDurationTime		= rKeyInfo.fDurationTime;
	_rScript				= rKeyInfo.strScript;
}

void XCutSceneObjectControllerCaption::SetKeyInfo( int nIdx, float _fTime, float _fDurationTime, const std::tstring& _rScript )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	rKeyInfo.fTime			= _fTime;
	rKeyInfo.fDurationTime	= _fDurationTime;
	rKeyInfo.strScript		= _rScript;
}

void XCutSceneObjectControllerCaption::AddKey( float _fTime, float _DurationTime, tstring _strScript )
{
	m_vKeyInfos.push_back( SKeyInfo(_fTime, _DurationTime, _strScript) );
}

void XCutSceneObjectControllerCaption::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerCaption::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	// 자막 보여준다.
	ShowCaption(pCurrent, m_strFontName, m_nSize, m_bBold, m_bItalic);
}

void XCutSceneObjectControllerCaption::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	// 자막 보여준다.
	ShowCaption(pProcKey, m_strFontName, m_nSize, m_bBold, m_bItalic);
}

void XCutSceneObjectControllerCaption::Update( float fElapsed )
{
	if(m_CurrentCaption.bActive)
	{
		m_CurrentCaption.fDurationTime -= fElapsed;
		if(m_CurrentCaption.fDurationTime <= 0.0f)
		{
			// 자막 삭제
			HideCaption();
		}
	}

	m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos);
}

void XCutSceneObjectControllerCaption::ShowCaption( SKeyInfo* keyInfo, tstring strFontName, int nSize, bool bBold, bool bItalic )
{
	if(keyInfo->nIndex == m_CurrentCaption.nIndex)
		return;

	m_CurrentCaption.nIndex			= keyInfo->nIndex;
	m_CurrentCaption.fDurationTime	= keyInfo->fDurationTime;
	m_CurrentCaption.strScript		= keyInfo->strScript;
	m_CurrentCaption.bActive		= true;

	if(m_pOwner->GetEntity())
	{
		m_pOwner->GetEntity()->ShowCaption(m_CurrentCaption.strScript.c_str(), strFontName.c_str(), m_nSize, bBold, bItalic);
	}
}

void XCutSceneObjectControllerCaption::HideCaption()
{
	m_CurrentCaption.bActive = false;

	if(m_pOwner->GetEntity())
		m_pOwner->GetEntity()->HideCaption();
}

void XCutSceneObjectControllerCaption::SetFontName( tstring strName )
{ 
	if(strName.empty())
		return;

	m_strFontName = strName; 
}

void XCutSceneObjectControllerCaption::SortKey()
{
	m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime);

	int nSetIndex = 0;
	for(std::vector< SKeyInfo >::iterator it = m_vKeyInfos.begin(); it != m_vKeyInfos.end(); ++it)
	{
		++nSetIndex;
		it->nIndex = nSetIndex; 
	}
}
