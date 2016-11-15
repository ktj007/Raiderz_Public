#include "stdafx.h"
#include "XCutSceneObjectControllerEnv.h"

#include "XCutSceneObject.h"
#include "XCutSceneContent.h"

#include "MMath.h"
#include "MLocale.h"

#include "RCameraSceneNode.h"

bool XCutSceneObjectControllerEnv::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	int nEnvTime, nEnvWeather;
	std::tstring strSetName;

	MXmlElement* pEnvKeyElement = pControllerElement->FirstChildElement("envkey");
	while (pEnvKeyElement)
	{
		_Attribute(fTime, pEnvKeyElement, "time");
		_Attribute(nEnvTime, pEnvKeyElement, "envtime");
		_Attribute(nEnvWeather, pEnvKeyElement, "envweather");
		_Attribute(strSetName, pEnvKeyElement, "envset");

		AddKey(fTime, nEnvTime, nEnvWeather, strSetName);
		pEnvKeyElement = pEnvKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerEnv::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pEnvElement = new MXmlElement("env");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pEnvKey = new MXmlElement("envkey");
		_SetAttribute(pEnvKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pEnvKey, "envtime", m_vKeyInfos[i].nEnvTime);
		_SetAttribute(pEnvKey, "envweather", m_vKeyInfos[i].nEnvWeather);
		_SetAttribute(pEnvKey, "envset", MLocale::ConvTCHARToAnsi(m_vKeyInfos[i].strSetName.c_str()).c_str());
		pEnvElement->LinkEndChild(pEnvKey);
	}

	pObjectElement->LinkEndChild(pEnvElement);
}

void XCutSceneObjectControllerEnv::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
	for (std::vector< SKeyInfo >::iterator itr = m_vKeyInfos.begin(); itr != m_vKeyInfos.end(); ++itr)
	{
		itr->bUpdated = false;
	}
}

void XCutSceneObjectControllerEnv::GetKeyInfo( int nIdx, float& _rOutTime, int& _rEnvTime, int& _rEnvWeather, std::tstring& _rSetName )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	_rOutTime = rKeyInfo.fTime;
	_rEnvTime = rKeyInfo.nEnvTime;
	_rEnvWeather = rKeyInfo.nEnvWeather;
	_rSetName = rKeyInfo.strSetName;
}

void XCutSceneObjectControllerEnv::SetKeyInfo( int nIdx, float _fTime, int _nEnvTime, int _nEnvWeather, const std::tstring& _rSetName )
{
	SKeyInfo& rKeyInfo = m_vKeyInfos[nIdx];
	rKeyInfo.fTime = _fTime;
	rKeyInfo.nEnvTime = _nEnvTime;
	rKeyInfo.nEnvWeather = _nEnvWeather;
	rKeyInfo.strSetName = _rSetName;
}

void XCutSceneObjectControllerEnv::AddKey( float _fTime, int _nEnvTime, int _nEnvWeather, const std::tstring& _rSetName )
{
	m_vKeyInfos.push_back( SKeyInfo(_fTime, _nEnvTime, _nEnvWeather, _rSetName) );
}

void XCutSceneObjectControllerEnv::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerEnv::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	if (pCurrent->bUpdated)
		return;

	pCurrent->bUpdated = true;
	m_pOwner->GetEntity()->SetEnvironment(pCurrent->nEnvTime, pCurrent->nEnvWeather, pCurrent->strSetName.c_str(), (pCurrent->fTime > 0.0001f) );
}

void XCutSceneObjectControllerEnv::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	if (pProcKey->bUpdated)
		return;

	pProcKey->bUpdated = true;
	m_pOwner->GetEntity()->SetEnvironment(pProcKey->nEnvTime, pProcKey->nEnvWeather, pProcKey->strSetName.c_str(), (pProcKey->fTime > 0.0001f) );
}
