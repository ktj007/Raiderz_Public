#pragma once

#include "GTalentInfo.h"

class GEntityActor;
class GEntityPlayer;
class GTalentInfo;

class GUTHelper_Talent
{
public:
	// 플레이어가 탤런트를 배움
	void Train(GEntityPlayer* pPlayer, GTalentInfo* pTalentInfo);
	// 플레이어가 배운 모든 탤런트를 없앰
	void UntrainAll( GEntityPlayer* pPlayer );
	// 탤런트에 데미지를 입력한다.
	void SetTalentDamage( GTalentInfo* pTalentInfo, int nDamage );
	// 탤런트에 피격 가능한 간단한 충돌정보를 입력한다.
	void AddSimpleTalentInfoHitInfo( GTalentInfo* pTalentInfo, float fCheckTime=0.0f );
	// 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewTalentInfo(int nID=INVALID_ID, int nTalentLine=0, int8 nRank=0, int nCooltimeLine=INVALID_ID);
	// 배우야만 사용 가능한 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewTrainingNeedTalentInfo(int nID=INVALID_ID, int nTalentLine=0, int8 nRank=0);
	// 접근 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewMeleeTalentInfo(int nID=INVALID_ID);
	// 패시브 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewPassiveTalentInfo(int nID=INVALID_ID);
	// 패시브 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewExtraPassiveTalentInfo( TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType, int nID=INVALID_ID, int nTalentLine=INVALID_ID);
	// 방어 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewGuardTalentInfo(int nID=INVALID_ID);
	// 활 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewArcheryTalentInfo(int nID=INVALID_ID);
	// 마법 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewMagicTalentInfo(int nID=INVALID_ID);
	// 아이템사용 탤런트정보를 생성하고,TalentMgr에 추가한다.  nID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* NewItemTalentInfo(int nID=INVALID_ID);
	// 접근 탤런트정보를 생성하고, 데미지를 입력한 후 TalentMgr에 추가한다.  
	// nTalentID가 INVALID일 경우, 새로운 ID를 할당해준다.
	GTalentInfo* MakeDamageTalent( int nDamage, int nTalentID=INVALID_ID );
	// 밀리 탤런트 기본정보 입력
	void SetMeleeTalentInfo( GTalentInfo* pTalentInfo, int nID=INVALID_ID );
	// 익스트라액티브 탤런트 기본정보 입력
	void SetExtraActiveTalentInfo( GTalentInfo* pTalentInfo, int nID, TALENT_EXTRA_ACTIVE_TYPE nExtraActiveType );
	// 익스트라패시브 탤런트 기본정보 입력
	void SetExtraPassiveTalentInfo( GTalentInfo* pTalentInfo, int nID, TALENT_EXTRA_PASSIVE_TYPE nExtraPassiveType );
	// 패시브 탤런트 기본정보 입력
	void SetPassiveTalentInfo(GTalentInfo* pTalentInfo, int nID=INVALID_ID);
	// 활 탤런트 기본정보 입력
	void SetArcheryTalentInfo(GTalentInfo* pTalentInfo, int nID=INVALID_ID);
	// 마법 탤런트 기본정보 입력
	void SetMagicTalentInfo(GTalentInfo* pTalentInfo, int nID=INVALID_ID);
	// 미사일이 포함된 마법 탤런트 기본정보 입력
	void SetMagicMissileTalentInfo(GTalentInfo* pTalentInfo, int nID=INVALID_ID);
	// 타켓팅 마법 탤런트 기본정보 입력
	void SetMagicTargetTalentInfo(GTalentInfo* pTalentInfo, int nID=INVALID_ID);
	// 지역마법 탤런트 기본정보 입력
	void SetMagicAreaTalentInfo( GTalentInfo* pTalentInfo, int nID=INVALID_ID);
	// 탤런트 이벤트 추가
	void AddTalentEventInfo(GTalentInfo* pTalentInfo, TALENT_EVENT nEvent, float fTime);
	// 탤런트 액트 이벤트 추가
	void AddTalentActEventInfo(GTalentInfo* pTalentInfo, TALENT_EVENT nEvent, float fTime);
	// TALENT_TARGET_INFO를 대상으로 탤런트를 사용하고, 올바로 실행 될 수 있게 필드를 update시킴
	void UseTalent(GEntityActor* pUser, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO target_info=TALENT_TARGET_INFO(), bool bCancelCurTalent=true);
	// MUID를 대상으로 탤런트를 사용하고, 올바로 실행 될 수 있게 필드를 update시킴
	void UseTalent(GEntityActor* pUser, GTalentInfo* pTalentInfo, MUID uidTarget, bool bCancelCurTalent=true);
	// 방어를 함 (방어에 필요한 탤런트 생성)
	void Guard( GEntityActor* pUser );
	// 현재 사용중인 탤런트를 취소
	void CancelTalent( GEntityActor* pUser );
	// 모든 장비아이템 라이센스 배우기
	void TrainEquipUsableAllTalent(GEntityPlayer* pPlayer);	
	// 장비아이템 라이센스 배우기
	void LearnExtraPassiveTalent(GEntityPlayer* pPlayer, TALENT_EXTRA_PASSIVE_TYPE nExtraPassive);
};
