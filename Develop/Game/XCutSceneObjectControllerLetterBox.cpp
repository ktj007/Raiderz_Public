#include "stdafx.h"
#include "XCutSceneObjectControllerLetterBox.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerLetterBox::ParseFromElement( MXmlElement* pControllerElement )
{
	LetterBoxTYPE eType;
	float fTime;

	MXmlElement* pLetterBoxKeyElement = pControllerElement->FirstChildElement("letterboxkey");
	while (pLetterBoxKeyElement)
	{
		string strType;
		_Attribute(strType, pLetterBoxKeyElement, "type");

		eType = LBT_START;
		if(strType == "end")	eType = LBT_END;

		_Attribute(fTime, pLetterBoxKeyElement, "time");

		AddKey(eType, fTime);
		pLetterBoxKeyElement = pLetterBoxKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerLetterBox::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pLetterBoxElement = new MXmlElement("letterbox");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pLetterBoxKey = new MXmlElement("letterboxkey");
		string strType = "start";
		if(m_vKeyInfos[i].eType == LBT_END)
			strType = "end";

		_SetAttribute(pLetterBoxKey, "type", strType);
		_SetAttribute(pLetterBoxKey, "time", m_vKeyInfos[i].fTime);

		pLetterBoxElement->LinkEndChild(pLetterBoxKey);
	}

	pObjectElement->LinkEndChild(pLetterBoxElement);
}

void XCutSceneObjectControllerLetterBox::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();

	if(m_pOwner->GetEntity())
		m_pOwner->GetEntity()->SetLetterBox(false, 0, 0);
}

void XCutSceneObjectControllerLetterBox::GetKeyInfo( int nIdx, LetterBoxTYPE& _eOutType, float& _rOutTime )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	_eOutType				= rKeyInfo.eType;
	_rOutTime				= rKeyInfo.fTime;
}

void XCutSceneObjectControllerLetterBox::SetKeyInfo( int nIdx, LetterBoxTYPE _eType, float _fTime )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	rKeyInfo.eType			= _eType;
	rKeyInfo.fTime			= _fTime;
}

void XCutSceneObjectControllerLetterBox::AddKey( LetterBoxTYPE eType, float _fTime )
{
	m_vKeyInfos.push_back( SKeyInfo(eType, _fTime) );
}

void XCutSceneObjectControllerLetterBox::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerLetterBox::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	// 레터박스 보여주기
	SetLetterBox(pCurrent->eType);
}

void XCutSceneObjectControllerLetterBox::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	SetLetterBox(pProcKey->eType);
}

void XCutSceneObjectControllerLetterBox::SetLetterBox( LetterBoxTYPE eType )
{
	if(m_pOwner->GetEntity())
	{		
		bool bShow = true;
		if(eType == LBT_END)
			bShow = false;

		m_pOwner->GetEntity()->SetLetterBox(bShow, float(LETTERBOX_SCRREN_RATIO_W), float(LETTERBOX_SCRREN_RATIO_H));
	}
}

void XCutSceneObjectControllerLetterBox::Update( float fElapsed )
{
	m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos);
}
