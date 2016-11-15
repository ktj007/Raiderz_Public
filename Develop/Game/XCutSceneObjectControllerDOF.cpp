#include "stdafx.h"
#include "XCutSceneObjectControllerDOF.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerDOF::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime, fFocusDist, fFocusRange, fCocSize;
	MXmlElement* pDofKeyElement = pControllerElement->FirstChildElement("dofkey");
	while (pDofKeyElement)
	{
		_Attribute(fTime, pDofKeyElement, "time");
		_Attribute(fFocusDist, pDofKeyElement, "focus_dist");
		_Attribute(fFocusRange, pDofKeyElement, "focus_range");
		_Attribute(fCocSize, pDofKeyElement, "coc_size");

		AddKey(fTime, fFocusDist, fFocusRange, fCocSize);
		pDofKeyElement = pDofKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerDOF::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pDofElement = new MXmlElement("dof");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pDofKey = new MXmlElement("dofkey");
		_SetAttribute(pDofKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pDofKey, "focus_dist", m_vKeyInfos[i].fFocusDist);
		_SetAttribute(pDofKey, "focus_range", m_vKeyInfos[i].fFocusRange);
		_SetAttribute(pDofKey, "coc_size", m_vKeyInfos[i].fCocSize);
		pDofElement->LinkEndChild(pDofKey);
	}

	pObjectElement->LinkEndChild(pDofElement);
}

void XCutSceneObjectControllerDOF::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	if( rs3::REngine::GetSceneManager().IsEnableDOF() == false)
	{
		rs3::REngine::GetSceneManager().SetDOFEnable(true);
	}

	rs3::REngine::GetSceneManager().SetDOFFocalDist( MMath::LinearInterpolation(pCurrent->fFocusDist, pNext->fFocusDist, fIntervalRatio) );
	rs3::REngine::GetSceneManager().SetDOFFocalRange( MMath::LinearInterpolation(pCurrent->fFocusRange, pNext->fFocusRange, fIntervalRatio) );
	rs3::REngine::GetSceneManager().SetDOFCOCSize( MMath::LinearInterpolation(pCurrent->fCocSize, pNext->fCocSize, fIntervalRatio) );
}

void XCutSceneObjectControllerDOF::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if (rs3::REngine::GetSceneManagerPtr()->IsEnableDOF() == false)
	{
		rs3::REngine::GetSceneManagerPtr()->SetDOFEnable(true);
	}

	rs3::REngine::GetSceneManagerPtr()->SetDOFFocalDist( pProcKey->fFocusDist );
	rs3::REngine::GetSceneManagerPtr()->SetDOFFocalRange( pProcKey->fFocusRange );
	rs3::REngine::GetSceneManagerPtr()->SetDOFCOCSize( pProcKey->fCocSize );
}

void XCutSceneObjectControllerDOF::Update( float fElapsed )
{
	if (!m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos))
	{
		rs3::REngine::GetSceneManagerPtr()->SetDOFEnable(false);
	}
}

void XCutSceneObjectControllerDOF::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
	if (rs3::REngine::GetSceneManagerPtr()->IsEnableDOF() == true)
	{
		rs3::REngine::GetSceneManagerPtr()->SetDOFEnable(false);
	}
}

void XCutSceneObjectControllerDOF::GetKeyInfo( int nIdx, float& _rOutTime, float& _rFocusDist, float& _rFocusRange, float& _rCocSize )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	_rOutTime = rKeyInfo.fTime;
	_rFocusDist = rKeyInfo.fFocusDist;
	_rFocusRange = rKeyInfo.fFocusRange;
	_rCocSize = rKeyInfo.fCocSize;
}

void XCutSceneObjectControllerDOF::SetKeyInfo( int nIdx, float _fTime, float _fFocusDist, float _fFocusRange, float _fCocSize )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime = _fTime;
	rKeyInfo.fFocusDist = _fFocusDist;
	rKeyInfo.fFocusRange = _fFocusRange;
	rKeyInfo.fCocSize = _fCocSize;
}

void XCutSceneObjectControllerDOF::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}
