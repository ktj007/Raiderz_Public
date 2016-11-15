#pragma once

#include "GDef.h"

class GTalentInfo;
class GBuffInfo;
struct CSBuffEnchantInfo;
class GEntityActor;

class GUTHelper_Buff
{
public:
	// 버프 탤런트 기본정보 입력
	void SetBuffTalentInfo(GTalentInfo* pTalentInfo, int nID=INVALID_ID, CSBuffEnchantInfo* pBuffEnchantInfo=NULL);
	// 버프 기본정보 입력
	void SetBuffInfo(GBuffInfo* pInfo, int nID=INVALID_ID);
	// 버프정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GBuffInfo* NewBuffInfo(int nID=INVALID_BUFF_ID);
	// 버프 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewBuffTalentInfo(int nID=INVALID_ID, CSBuffEnchantInfo* pBuffEnchantInfo=NULL);
	// 버프를 생성할 수 있는 탤런트를 만들고 TalentMgr에 추가한다.
	GTalentInfo* NewBuffTalent(GBuffInfo*& pBuff, float fDuration=-1.0f, int nTalentID=INVALID_TALENT_ID, int nBuffID=INVALID_BUFF_ID );
	// 버프를 생성할 수 있는 탤런트를 만들고 TalentMgr에 추가한다.
	GTalentInfo* NewBuffTalent(GBuffInfo*& pBuff, CSBuffEnchantInfo* pBuffEnchantInfo);
	// 버프정보를 받고(없으면 새로 생성), 해당 버프를 pUser에게 걸리게 한다.
	void GainBuffSelf( GEntityActor* pUser, GBuffInfo* pBuff=NULL, float fDuration=BUFF_DURATION_INFINITY);
	// 버프정보를 받고(없으면 새로 생성), 해당 버프를 pUser에게 걸리게 한다.
	void GainBuffSelf( GEntityActor* pUser, GBuffInfo* pBuff, CSBuffEnchantInfo* pBuffEnchantInfo);
	// 특정 버프를 제거
	void LostBuff( GEntityActor* pUser, int nBuffID);
	// 특정 버프 스택 전부를 제거
	void LostBuffStack( GEntityActor* pUser, int nBuffIDSlot);
};
