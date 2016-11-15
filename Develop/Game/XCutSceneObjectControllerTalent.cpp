#include "stdafx.h"
#include "XCutSceneObjectControllerTalent.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerTalent::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	int nTalentID;

	MXmlElement* pAniKeyElement = pControllerElement->FirstChildElement("talentkey");
	while (pAniKeyElement)
	{
		_Attribute(fTime, pAniKeyElement, "time");
		_Attribute(nTalentID, pAniKeyElement, "ID");

		AddKey(fTime, nTalentID);
		pAniKeyElement = pAniKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerTalent::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pTalentElement = new MXmlElement("talent");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pTalentKey = new MXmlElement("talentkey");
		_SetAttribute(pTalentKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pTalentKey, "ID", m_vKeyInfos[i].nTalentID);
		pTalentElement->LinkEndChild(pTalentKey);
	}

	pObjectElement->LinkEndChild(pTalentElement);
}

void XCutSceneObjectControllerTalent::GetKeyInfo( int nIdx, float& _rOutTime, int& nTalentID )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	_rOutTime = rKeyInfo.fTime;
	nTalentID = rKeyInfo.nTalentID;
}

void XCutSceneObjectControllerTalent::SetKeyInfo( int nIdx, float _fTime, int nTalentID )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime = _fTime;
	rKeyInfo.nTalentID = nTalentID;
}

void XCutSceneObjectControllerTalent::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerTalent::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	if (pCurrent->bUpdated)
		return;

	pCurrent->bUpdated = true;
	m_pOwner->GetEntity()->OnUseTalent(pCurrent->nTalentID);
}

void XCutSceneObjectControllerTalent::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if (pProcKey->bUpdated)
		return;

	pProcKey->bUpdated = true;
	m_pOwner->GetEntity()->OnUseTalent(pProcKey->nTalentID);
}

void XCutSceneObjectControllerTalent::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
	for (std::vector< SKeyInfo >::iterator itr = m_vKeyInfos.begin(); itr != m_vKeyInfos.end(); ++itr)
	{
		itr->bUpdated = false;
	}
}
