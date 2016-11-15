#pragma once

#include "XTalentEffectInfo.h"

class CSTalentInfoMgr;

class XTalentEffectMgr
{
protected:
	CSTalentInfoMgr*					m_pTalentInfoMgr;

	map<int, XTalentEffectBase *>		m_mapEffectBase;

private:
	void								CopyTalentEffectInfoByReference();
	void								CopyTalentEffectInfo(XTalentInfo* pTalentInfo);

	void								Destroy();

public:
	XTalentEffectMgr(CSTalentInfoMgr* pTalentInfoMgr);
	virtual ~XTalentEffectMgr();

	bool								Load(const wchar_t* szFileName);
	bool								Reload(const wchar_t* szFileName);

	XTalentEffectBase *					GetTalentEffectBase(TALENT_EFFECT_BASE_TYPE nType);

};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 전역함수
float CalParabola(float fFullLength, float fCurrLength, float fMaxHeight);
vec3 MoveParabola(vec3 vStartPos, vec3 vTargetPos, vec3 vCurPos, vec3 vVelocity, float fTickTime, float fMaxHeight);
