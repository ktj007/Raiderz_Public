#include "stdafx.h"
#include "XAutoSoundChecker.h"
#include "XEffectInvoker.h"
#include "XTalentInfoMgr.h"

XAutoSoundChecker::XAutoSoundChecker()
{

}

XAutoSoundChecker::~XAutoSoundChecker()
{

}

void XAutoSoundChecker::HitSoundChecker( XObject* pObject, XTalentInfo* pTalentInfo, XObject* pDestObject /*= NULL*/ )
{
	vector<XTalentEffectInfo *> vecEffectInfoByType;
	GetTalentEffectInfo(pTalentInfo, vecEffectInfoByType);

	//////////////////////////////////////////////////////////////////////////
	// 사운드 정보 가져오기
	vector<CHECK_EFFECT_DATA> vecCheckEffectData;
	for(vector<XTalentEffectInfo*>::iterator it = vecEffectInfoByType.begin(); it != vecEffectInfoByType.end(); it++)
	{
		if((*it)->m_nEffectType == ATTACK_EFFECT ||
			(*it)->m_nEffectType == ATTACK_SECONDARY_EFFECT)
		{
			XEffectInvoker soundCheckInvoker;
			soundCheckInvoker.GetEffect_Data(CET_SOUND, pObject, pDestObject, (*it)->m_strEffectName, (*it)->m_strBoneName, (*it)->m_strParameter, &vecCheckEffectData);
		}
	}

	// 사운드 정보 체크
	bool bUseSound = false;
	if(vecCheckEffectData.size() == 1)
	{
		if(vecCheckEffectData[0].strEffectName == L"ss")
		{
			bUseSound = true;
		}
	}
	else if(vecCheckEffectData.size() > 1)
	{
		bUseSound = true;
	}

	if(bUseSound == false)
	{
		// 아무것도 없다면... 디폴트 실시....
		XEffectInvoker soundInvoker;
		soundInvoker.Invoke(pObject, wstring(L"ss"), wstring(), wstring(), NULL, pDestObject);
	}
}

void XAutoSoundChecker::GetTalentEffectInfo( XTalentInfo* pTalentInfo, vector<XTalentEffectInfo *>& vecEffectInfoByType )
{
	XTalentEffectDataMgr* pTalentEffectDataMgr = pTalentInfo->GetEffectDataMgr();

	// nType으로 이펙트Info를 가져온다.
	if(pTalentEffectDataMgr && pTalentEffectDataMgr->IsEmpty() == false)
	{
		pTalentEffectDataMgr->Get(ATTACK_EFFECT, vecEffectInfoByType);
	}
}