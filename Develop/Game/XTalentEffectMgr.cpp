#include "stdafx.h"
#include "XTalentEffectMgr.h"
#include "XTalentInfoDef.h"
#include "XTalentInfoMgr.h"
#include "XTalentInfo.h"
#include "CSTalentInfoParser.h"
#include "XTalentEffectInfoParser.h"

//----------------------------------------------------------------------------//
// [2007/5/25 isnara]
XTalentEffectMgr::XTalentEffectMgr(CSTalentInfoMgr* pTalentInfoMgr) : m_pTalentInfoMgr(pTalentInfoMgr)
{
}

XTalentEffectMgr::~XTalentEffectMgr()
{
	Destroy();
}

bool XTalentEffectMgr::Load(const wchar_t* szFileName)
{
	XTalentEffectInfoParser talentEffectInfoParser;
	talentEffectInfoParser.Load(szFileName, m_pTalentInfoMgr, m_mapEffectBase);

	CopyTalentEffectInfoByReference();

	return true;
}

XTalentEffectBase * XTalentEffectMgr::GetTalentEffectBase( TALENT_EFFECT_BASE_TYPE nType )
{
	map<int, XTalentEffectBase *>::iterator it = m_mapEffectBase.find((int)nType);
	if(it != m_mapEffectBase.end())
	{
		return it->second;
	}

	return NULL;
}

void XTalentEffectMgr::CopyTalentEffectInfoByReference()
{
	for(map<int , CSTalentInfo* >::iterator itTalentInfo = m_pTalentInfoMgr->begin(); itTalentInfo != m_pTalentInfoMgr->end(); itTalentInfo++)
	{
		XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>(itTalentInfo->second);
		
		CopyTalentEffectInfo(pTalentInfo);

		// 모드 카피
		for(int i = 0; i < MAX_TALENT_MODE; ++i)
		{
			XTalentInfo* pModeTalentInfo = static_cast<XTalentInfo*>(pTalentInfo->GetMode(i));
			if(pModeTalentInfo != NULL)
				CopyTalentEffectInfo(pModeTalentInfo);
		}
	}
}

void XTalentEffectMgr::Destroy()
{
	map<int, XTalentEffectBase *>::iterator itBase = m_mapEffectBase.begin();
	while(itBase != m_mapEffectBase.end())
	{
		XTalentEffectBase * pBase = itBase->second;
		SAFE_DELETE(pBase);
		itBase++;
	}

	m_mapEffectBase.clear();
}

bool XTalentEffectMgr::Reload( const wchar_t* szFileName )
{
	Destroy();

	return Load(szFileName);
}

void XTalentEffectMgr::CopyTalentEffectInfo( XTalentInfo* pTalentInfo )
{
	XTalentEffectDataMgr* pTalentEffectDataMgr = pTalentInfo->GetEffectDataMgr();

	if(pTalentEffectDataMgr)
	{
		int nRefID = pTalentEffectDataMgr->m_nRefID;
		int nRefMode = pTalentEffectDataMgr->m_nRefMode;
		if(nRefID > 0)
		{
			XTalentInfo* pRefTalentInfo = static_cast<XTalentInfo*>(m_pTalentInfoMgr->Find(nRefID, nRefMode, true));

			if(pRefTalentInfo)
			{
				XTalentEffectDataMgr* pRefTalentEffectDataMgr = pRefTalentInfo->GetEffectDataMgr();

				for(vector<XTalentEffectInfo *>::iterator itRefTalentEffecInfo = pRefTalentEffectDataMgr->begin(); itRefTalentEffecInfo != pRefTalentEffectDataMgr->end(); itRefTalentEffecInfo++)
				{
					XTalentEffectInfo* pRefTalentEffectInfo = *itRefTalentEffecInfo;
					XTalentEffectInfo* pCopyTalentEffectInfo = new XTalentEffectInfo;

					pCopyTalentEffectInfo->copyData(*pRefTalentEffectInfo);

					pTalentEffectDataMgr->push_back(pCopyTalentEffectInfo);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 전역 함수

vec3 MoveParabola( vec3 vStartPos, vec3 vTargetPos, vec3 vCurPos, vec3 vVelocity, float fTickTime, float fMaxHeight )
{
	// 방향 회전
	vec3 vDir			= (vTargetPos - vCurPos).Normalize();

	// 위치
	float fSpeed		= vVelocity.Length();
	vec3 vCalPos		= vCurPos + (vDir * fSpeed * fTickTime);

	// 포물선 계산
	float fFullLength	= (vTargetPos - vStartPos).Length();
	float fCulLength	= (vCalPos - vStartPos).Length();

	float fHeight		= CalParabola(fFullLength, fCulLength, fMaxHeight);

	vCalPos.z			+= fHeight;

	return vCalPos;
}

float CalParabola( float fFullLength, float fCurrLength, float fMaxHeight )
{
	float fAngle			= (180.0f * fCurrLength) / fFullLength;
	float fPI				= 3.1415926535f;
	float fDegreeToRadian	= (fPI * fAngle / 180.0f);

	return (sin(fDegreeToRadian) * fMaxHeight);
}
