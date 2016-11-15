#include "stdafx.h"
#include "XCutSceneObjectControllerKeyEvent.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

XCutSceneObjectControllerKeyEvent::XCutSceneObjectControllerKeyEvent(XCutSceneObject* pOwner)
: XCutSceneObjectController(pOwner, E_KEYEVENT), m_fPosX(10.0f), m_fPosY(10.0f) 
{
	tstring strKyeEventName = MLocale::ConvAnsiToTCHAR("ESC:SKIP");
	tstring strFont = MLocale::ConvAnsiToTCHAR("±¼¸²Ã¼");


	SKeyInfo keyInfo( 0.0f, 1000.0f, KEY_ESC, EVENT_SKIP, strKyeEventName, strFont, 20, false, false );
	
	AddKey( keyInfo );
}

bool XCutSceneObjectControllerKeyEvent::ParseFromElement( MXmlElement* pControllerElement )
{
	return true;
}

void XCutSceneObjectControllerKeyEvent::SaveToXML( MXmlElement* pObjectElement )
{

}

void XCutSceneObjectControllerKeyEvent::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();

	if(m_pOwner->GetEntity())
		m_pOwner->GetEntity()->HideKeyEvent();

	m_CurrentKeyEvent.Init();
}

void XCutSceneObjectControllerKeyEvent::GetKeyInfo( int nIdx, float& _rOutTime, float& _rOutDurationTime )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	_rOutTime				= rKeyInfo.fTime;
	_rOutDurationTime		= rKeyInfo.fDurationTime;
}

void XCutSceneObjectControllerKeyEvent::SetKeyInfo( int nIdx, float _fTime, float _fDurationTime )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	rKeyInfo.fTime			= _fTime;
	rKeyInfo.fDurationTime	= _fDurationTime;
}

void XCutSceneObjectControllerKeyEvent::AddKey( SKeyInfo& keyInfo )
{
	m_vKeyInfos.push_back( keyInfo );
}

void XCutSceneObjectControllerKeyEvent::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerKeyEvent::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	ShowKeyEvent(pCurrent);
}

void XCutSceneObjectControllerKeyEvent::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	ShowKeyEvent(pProcKey);
}

void XCutSceneObjectControllerKeyEvent::Update( float fElapsed )
{
	if(m_CurrentKeyEvent.bActive)
	{
		m_CurrentKeyEvent.fDurationTime -= fElapsed;
		if(m_CurrentKeyEvent.fDurationTime <= 0.0f)
		{
			HideKeyEvent();
		}
	}

	m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos);
}

void XCutSceneObjectControllerKeyEvent::ShowKeyEvent( SKeyInfo* keyInfo )
{
	if(keyInfo->nIndex == m_CurrentKeyEvent.nIndex)
		return;

	m_CurrentKeyEvent.Set(*keyInfo);
	m_CurrentKeyEvent.bActive = true;

	if(m_pOwner->GetEntity())
	{
		m_pOwner->GetEntity()->ShowKeyEvent(m_CurrentKeyEvent.strKyeEventName.c_str(), m_fPosX, m_fPosY, m_CurrentKeyEvent.strFont.c_str(), m_CurrentKeyEvent.nSize, m_CurrentKeyEvent.bBold, m_CurrentKeyEvent.bItalic);
	}
}

void XCutSceneObjectControllerKeyEvent::HideKeyEvent()
{
	m_CurrentKeyEvent.bActive = false;

	if(m_pOwner->GetEntity())
	{
		m_pOwner->GetEntity()->HideKeyEvent();
	}
}

void XCutSceneObjectControllerKeyEvent::SortKey()
{
	m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime);

	int nSetIndex = 0;
	for(std::vector< SKeyInfo >::iterator it = m_vKeyInfos.begin(); it != m_vKeyInfos.end(); ++it)
	{
		++nSetIndex;
		it->nIndex = nSetIndex; 
	}
}

bool XCutSceneObjectControllerKeyEvent::OnEvent(mint3::MEvent& e)
{
	switch ( e.nMessage)
	{
		case MWM_KEYDOWN:
		{
			switch ( e.nKey)
			{
			case VK_ESCAPE:
				{
					if(m_pOwner->GetEntity())
					{
						m_pOwner->GetEntity()->CutSceneSkip();
					}
				}
				break;
			}
			break;
		}
	}

	return false;
}
