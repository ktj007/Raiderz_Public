#include "stdafx.h"
#include "XCutSceneObjectControllerEffect.h"

#include "XCutSceneObject.h"

#include "MLocale.h"

bool XCutSceneObjectControllerEffect::ParseFromElement( MXmlElement* pControllerElement )
{
	float fTime;
	float fDurationTime;
	string strEffectName;
	string strEffectInfo;
	vec3 vPos;
	vec3 vDir;
	vec3 vUp;

	MXmlElement* pEffectKeyElement = pControllerElement->FirstChildElement("effectkey");
	while (pEffectKeyElement)
	{
		_Attribute(strEffectName, pEffectKeyElement, "effectname");
		_Attribute(strEffectInfo, pEffectKeyElement, "effectinfo", "");
		_Attribute(fTime, pEffectKeyElement, "time", -1.0f);
		_Attribute(fDurationTime, pEffectKeyElement, "durationtime");

		MXmlElement* pEffectMatElement = pEffectKeyElement->FirstChildElement("mat");
		char szBuffer[512] = "";
		_Contents(szBuffer, pEffectMatElement);

		sscanf_s(szBuffer,"%f %f %f %f %f %f %f %f %f", 
				&vPos.x, &vPos.y, &vPos.z, 
				&vDir.x, &vDir.y, &vDir.z,
				&vUp.x,  &vUp.y,  &vUp.z);

#ifdef _UNICODE
		AddKey(fTime, fDurationTime, MLocale::ConvUTF8ToUTF16(strEffectName.c_str()), MLocale::ConvUTF8ToUTF16(strEffectInfo.c_str()), vPos, vDir, vUp);
#else
		AddKey(fTime, fDurationTime, MLocale::ConvUTF8ToAnsi(strEffectName.c_str()), MLocale::ConvUTF8ToAnsi(strEffectInfo.c_str()), vPos, vDir, vUp);
#endif

		pEffectKeyElement = pEffectKeyElement->NextSiblingElement();
	}

	SortKey();
	return true;
}

void XCutSceneObjectControllerEffect::SaveToXML( MXmlElement* pObjectElement )
{
	MXmlElement* pEffectElement = new MXmlElement("effect");

	size_t nTotalKey = m_vKeyInfos.size();
	for (size_t i = 0; i < nTotalKey; ++i)
	{
		MXmlElement* pEffectKey = new MXmlElement("effectkey");

#ifdef _UNICODE
		_SetAttribute(pEffectKey, "effectname", MLocale::ConvUTF16ToUTF8(m_vKeyInfos[i].strEffectName.c_str()));
		_SetAttribute(pEffectKey, "effectinfo", MLocale::ConvUTF16ToUTF8(m_vKeyInfos[i].strEffectInfo.c_str()));
#else
		_SetAttribute(pEffectKey, "effectname", MLocale::ConvAnsiToUTF8(m_vKeyInfos[i].strEffectName.c_str()));
		_SetAttribute(pEffectKey, "effectinfo", MLocale::ConvAnsiToUTF8(m_vKeyInfos[i].strEffectInfo.c_str()));
#endif

		_SetAttribute(pEffectKey, "time", m_vKeyInfos[i].fTime);
		_SetAttribute(pEffectKey, "durationtime", m_vKeyInfos[i].fDurationTime);

		MXmlElement *pmat = new MXmlElement("mat");
		char szBuffer[512] = "";
		sprintf_s(szBuffer,"%f %f %f %f %f %f %f %f %f",
				m_vKeyInfos[i].vPos.x, m_vKeyInfos[i].vPos.y, m_vKeyInfos[i].vPos.z, 
				m_vKeyInfos[i].vDir.x, m_vKeyInfos[i].vDir.y, m_vKeyInfos[i].vDir.z,
				m_vKeyInfos[i].vUp.x,  m_vKeyInfos[i].vUp.y,  m_vKeyInfos[i].vUp.z);
		_SetContents(pmat, szBuffer);

		pEffectKey->LinkEndChild(pmat);
		pEffectElement->LinkEndChild(pEffectKey);
	}

	pObjectElement->LinkEndChild(pEffectElement);
}

void XCutSceneObjectControllerEffect::ResetUpdate()
{
	m_KeyFrameUpdator.Reset();
	for(std::vector< SKeyInfo >::iterator it = m_vKeyInfos.begin(); it != m_vKeyInfos.end(); ++it)
	{
		it->m_bProced = false;
	}

	DestroyAllEffect();
}

void XCutSceneObjectControllerEffect::GetKeyInfo( int nIdx, float& _rOutTime, float& _rOutDurationTime, vec3& _rpos, vec3& _rdir, vec3& _rup, std::tstring& _rEffectName,  std::tstring& _rEffectInfo)
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	_rOutTime				= rKeyInfo.fTime;
	_rOutDurationTime		= rKeyInfo.fDurationTime;
	_rEffectName			= rKeyInfo.strEffectName;
	_rEffectInfo			= rKeyInfo.strEffectInfo;

	_rpos					= rKeyInfo.vPos;
	_rdir					= rKeyInfo.vDir;
	_rup					= rKeyInfo.vUp;
}

void XCutSceneObjectControllerEffect::SetKeyInfo( int nIdx, float _fTime, float _fDurationTime, vec3 _pos, vec3 _dir, vec3 _up, const std::tstring& _rEffectName, const std::tstring& _rEffectInfo )
{
	SKeyInfo& rKeyInfo		= m_vKeyInfos[nIdx];
	rKeyInfo.fTime			= _fTime;
	rKeyInfo.fDurationTime	= _fDurationTime;
	rKeyInfo.strEffectName	= _rEffectName;
	rKeyInfo.strEffectInfo	= _rEffectInfo;

	rKeyInfo.vPos			= _pos;
	rKeyInfo.vDir			= _dir;
	rKeyInfo.vUp			= _up;
}

void XCutSceneObjectControllerEffect::AddKey( float _fTime, float _DurationTime, const tstring& _strName, const tstring& _strInfo, vec3 _pos, vec3 _dir, vec3 _up )
{
	m_vKeyInfos.push_back( SKeyInfo(_fTime, _DurationTime, _strName, _strInfo, _pos, _dir, _up) );
}

void XCutSceneObjectControllerEffect::DeleteKey( int nIdx )
{
	if (nIdx < 0)
		return;

	m_vKeyInfos.erase( m_vKeyInfos.begin() + nIdx );
}

void XCutSceneObjectControllerEffect::OnUpdateBetweenKey( SKeyInfo* pCurrent, SKeyInfo* pNext, float fIntervalRatio, SKeyInfo* pPre, SKeyInfo* pNextNext )
{
	// 이펙트 생성
	if (pCurrent->m_bProced == false)
	{
		CreateEffect(pCurrent);
		pCurrent->m_bProced = true;
	}
}

void XCutSceneObjectControllerEffect::OnUpdateOverKey( SKeyInfo* pProcKey )
{
	// 이펙트 생성
	if (pProcKey->m_bProced == false)
	{
		CreateEffect(pProcKey);
		pProcKey->m_bProced = true;
	}
}

void XCutSceneObjectControllerEffect::Update( float fElapsed )
{
	UpdateEffect(fElapsed);

	m_KeyFrameUpdator.UpdateKey(this, fElapsed, m_vKeyInfos);
}

void XCutSceneObjectControllerEffect::CreateEffect( SKeyInfo* keyInfo )
{

	if(m_pOwner->GetEntity())
	{
		std::vector<stControllerEffectData> resultData;
		m_pOwner->GetEntity()->CreateEffect(keyInfo->strEffectName, keyInfo->strEffectInfo, keyInfo->vPos, keyInfo->vDir, keyInfo->vUp, &resultData); 

		// -1.0f 값은 계속 유지하거나 알아서 없어지는 이펙트이다.
		// 그러므로 저장 할 필요가 없다.
		if(keyInfo->fDurationTime > -1.0f)
		{
			for(std::vector<stControllerEffectData>::iterator itResult = resultData.begin(); itResult != resultData.end(); ++itResult)
			{
				stEffectData effectData;
				effectData.type				= itResult->type;
				effectData.fDurationTime	= keyInfo->fDurationTime;
				effectData.uidEffect		= itResult->uidEffect;
				m_vecEffectData.push_back(effectData);
			}
		}
	}
}

void XCutSceneObjectControllerEffect::DestroyEffect( MUID uid )
{
	if(m_pOwner->GetEntity())
		m_pOwner->GetEntity()->DestroyEffect(uid);
}

void XCutSceneObjectControllerEffect::SortKey()
{
	m_KeyFrameUpdator.SortKey(m_vKeyInfos, &m_fStartTime, &m_fEndTime);

	int nSetIndex = 0;
	for(std::vector< SKeyInfo >::iterator it = m_vKeyInfos.begin(); it != m_vKeyInfos.end(); ++it)
	{
		++nSetIndex;
		it->nIndex = nSetIndex; 
	}
}

void XCutSceneObjectControllerEffect::UpdateEffect( float fDelta )
{
	std::vector<stEffectData>::iterator itEffect = m_vecEffectData.begin();
	while(itEffect != m_vecEffectData.end())
	{
		itEffect->fDurationTime -= fDelta;
		if(itEffect->fDurationTime <= 0.0f)
		{
			DestroyEffect(itEffect->uidEffect);

			itEffect = m_vecEffectData.erase(itEffect);
			continue;
		}
		 
		++itEffect;
	}
}

void XCutSceneObjectControllerEffect::DestroyAllEffect()
{
	for(std::vector<stEffectData>::iterator itEffect = m_vecEffectData.begin(); itEffect != m_vecEffectData.end(); ++itEffect)
	{
		DestroyEffect(itEffect->uidEffect);
	}

	m_vecEffectData.clear();
}

XCutSceneObjectControllerEffect::~XCutSceneObjectControllerEffect()
{
	DestroyAllEffect();
}