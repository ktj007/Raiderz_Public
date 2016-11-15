#pragma once

#include "GActorObserver.h"
#include "CSDef.h"

class GEntityPlayer;
class GTalentFocus;
class GTalentFocus_Counter;
class GTalentFocus_Berserk;
class GTalentFocus_Precision;
class GTalentFocus_Enlighten;
class GTalentFocus_Sneak;
class GTalentFocus_Advent;
class GItem;

class GTalentFocusMgr : private GActorObserver, public MTestMemPool<GTalentFocusMgr>
{
public:
	GTalentFocusMgr(GEntityPlayer* pOwner);
	~GTalentFocusMgr();

	// 매니저 초기화
	void Init();
	// 매니저 마무리정리
	void Fini();
	// 매틱마다 호출됨
	void Update(float fDelta);
	
	
	// 자신이 죽었을때 호출되는 이벤트
	void OnDie();
	// 저격으로 토글될때 호출되는 이벤트
	void OnTogglePrecision();
	// 은신으로 토글될때 호출되는 이벤트
	void OnToggleSneak(int nSneakBuffType);
	// 강신이 적용될때 호출되는 이벤트
	void OnApplyAdvent();

	// 포커스를 하나라도 가지고 있는지 여부를 반환
	bool HasFocus();
	// 특정 포커스를 가지고 있는지 여부를 반환
	bool HasFocus(TALENT_FOCUS_TYPE nFocus);
	// 광포 버블 갯수 반환
	int GetBerserkBubble();
	// 광포 최대 버블 갯수 반환
	int GetBerserkMaxBubble();
	// 최대 포커스 갯수 반환
	int GetMaxFocusQty();

	// 인터랙션 했을때
	void OnInteraction();

private:
	// 모든 피격 성공 이벤트 (히트캡슐판정 용도)
	virtual void OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) override;
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	// 방어 이벤트
	virtual void OnGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	// 절대 방어 이벤트
	virtual void OnAbsoluteGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	// 버프를 얻었을때 이벤트
	virtual void OnGainBuff(int nBuffID) override;
	// 버프를 잃었을때 이벤트
	virtual void OnLostBuff(int nBuffID) override;
	// 아이템 장착 (Player 전용)
	virtual void OnItemEquipped(GItem* pItem) override;
	// 아이템 장착해제 (Player 전용)
	virtual void OnItemUnequipped(GItem* pItem) override;
	// 장비셋 변경 (Player 전용)
	virtual void OnSwitchingWeaponSet(SH_ITEM_SWITCH_WEAPON val) override;


	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo) override;
	// 탤런트 사용이 효과가 발동될때 호출되는 이벤트
	virtual void OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;


	// 포커스를 바꿀 수 있는지 체크해본 후, 바뀔 조건이 되면 포커스를 바꾼다.
	void CheckGainFocus(TALENT_FOCUS_TYPE nFocus, int nBuffType=INVALID_BUFF_ID);
	// 포커스를 얻는다.
	void GainFocus(TALENT_FOCUS_TYPE nFocus, int nBuffType=INVALID_BUFF_ID);
	// 특정 포커스를 잃는다.
	void LostFocus(TALENT_FOCUS_TYPE nFocus);
	// 모든 포커스를 잃는다.
	void LostAllFocus();
	// 접근 공격을 명중시켰을때 호출
	void OnHitMeleeTalent(GTalentInfo* pHitTalentInfo);
	// 해당 포커스를 토글 시킴
	void ToggleFocus(TALENT_FOCUS_TYPE nFocus, int nBuffType=INVALID_BUFF_ID);
	// 변경된 버블 갯수 처리
	void UpdateBubbleQty();

	// 특정 포커스객체를 반환
	GTalentFocus*			GetFocus(TALENT_FOCUS_TYPE nFocus);
	// 반격 포커스객체를 반환
	GTalentFocus_Counter*	GetCounterFocus();
	// 광포 포커스객체를 반환
	GTalentFocus_Berserk*	GetBerserkFocus();
	// 저격 포커스객체를 반환
	GTalentFocus_Precision*	GetPrecisionFocus();
	// 각성 포커스객체를 반환
	GTalentFocus_Enlighten*	GetEnlightenFocus();
	// 스닉 포커스객체를 반환
	GTalentFocus_Sneak*		GetSneakFocus();
	// 강신 포커스객체를 반환
	GTalentFocus_Advent*	GetAdventFocus();

	// 모든 활성화된 포커스의 아이템 조건이 맞는지 검사후, 잘못됐다면 포커스해제
	void CheckEquipItemForAllFocus();

	// 포커스 얻었다고 클라에게 알려줌
	void Route_GainFocus(TALENT_FOCUS_TYPE nFocus, float fDurationTime);
	// 포커스 잃었다고 클라에게 알려줌
	void Route_LostFocus(TALENT_FOCUS_TYPE nFocus);
	// 포커스 버블의 갯수가 갱신될때 알려줌
	void Route_UpdateBubble(TALENT_FOCUS_TYPE nFocus, uint8 nBubbleQty, uint8 nMaxBubbleQty);	
private:
	// 포커스매니저 소유자
	GEntityPlayer*			m_pOwner;
	// 활성화된 포커스타입 목록
	set<TALENT_FOCUS_TYPE>	m_setActiveFocus;
	// 포커스객체 테이블
	GTalentFocus*			m_pFocusTable[TFT_SIZE];
	// 이전 틱의 광포 버블 수
	int						m_nLastBerserkBubble;
};
