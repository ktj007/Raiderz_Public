#pragma once

#include "CSBuffInfo.h"
#include "GTickRegulator.h"
#include "MTime.h"
#include "GBuffObserver.h"
#include "GBuffObserverMgr.h"

class GEntitySync;
class GTalentInfo;
class GBuffInfo;
class GBuffReleaser;
class GBuffInstantApplier;
class GBuffModifierApplier;
class GBuffTriggerApplier;
class GBuffRouter;
class GBuffObserver;
class GEFfectTimingChecker;
class GBuffStack;
class GBuffScript;
class GBuffPaletteChanger;
class GBuffEquipmentChanger;


class GBuff : public GBuffObserverMgr, public MTestMemPool<GBuff>
{
	enum DESTROY_TYPE
	{
		DESTROY_NONE=0,
		DESTROY_EXPIRED,
		DESTROY_CANCELLED,
		DESTROY_DISPELLED,
	};
public:
	GBuff(GEntitySync* pOwner, GBuffInfo* pBuffInfo, float fDurationTime, float fPeriodTime, GTalentInfo* pTalentInfo=NULL, MUID uidUser=MUID::Invalid());
	virtual ~GBuff();

public:
	// 버프가 걸렸을 때 이벤트, 반환값을 소유자가 죽었는지 여부
	bool Start();
	// 버프가 소멸될때 이벤트, 반환값을 소유자가 죽었는지 여부
	bool Finish(bool bRoute);
	// 모듈 버프가 취소 결정하기 전의 틱 이벤트,반환값은 버프가 만료됐는지 여부
	void PreUpdate( float fDelta );
	// 틱 이벤트,반환값은 버프가 만료됐는지 여부
	bool Update(float fDelta);
	// 버프시간 변경
	void UpdateBuffTime(float fDurationTime, float fPeriodTime);
	// 버프 소멸 이벤트
	void OnDestry();
	// 버프가 중복되서 걸린 경우
	void OnDuplicated(float fDurationTime, float fPeriodTime, MUID uidUser);
	// 버프가 스택되서 걸린 경우
	void OnStacked(float fDurationTime, float fPeriodTime);
	// 버프 취소시킴, 스택이 있으면 스택을 하나 줄임 (lazy하게 처리)
	void Cancel();
	// 버프 취소시킴, 스택도 전부 취소  (lazy하게 처리)
	void CancelForced();
	// 버프를 디스펠시킴 (lazy하게 처리)
	void Dispel();

	// 버프 아이디
	int GetBuffID();
	// 사용자 UID 반환
	MUID GetUserUID();
	// 사용된 탤런트정보 반환
	GTalentInfo* GetUserTalentInfo();
	// 버프만료 타입 반환
	DESTROY_TYPE GetDestroyType() const;
	// 버프가 주기적효과가 있는지 여부
	bool HasPeriodEffect();
	// 버프가 영원히 지속될지 여부
	bool IsInfinite() const;
	// 버프가 소멸되는지 여부
	bool IsGone() const;
	// 버프의 스택 슬롯
	int	GetBuffStackSlot();
	// 버프의 스택 파워
	int	GetBuffStackPower();	
	// 지속시간
	float GetDurationTime();
	// 남은 시간
	float GetRemainTime();
	// 주기적시간
	float GetPeriodTime();
	// 스택 갯수 반환
	int GetStackCount() const;
	// 다음 주기까지 남은 시간
	float GetRemainNextPeriodTime();
	// 소유자 반환
	GEntitySync* GetOwner() const { return m_pOwner; }
	// 버프 정보
	GBuffInfo* GetInfo() const { return m_pInfo; }

	// 디버프인지 여부
	bool IsDebuff() const;
	// 스택가능한 버프인지 여부
	bool IsStackable() const;
	// 강화 버프인지 여부
	bool IsEchant() const;
	// 장비 변경 버프인지 여부
	bool IsChangeEquipment() const;
	
private:
	// 유지비용 갱신
	void Update_Maintenance( float fDelta );
	// 주기효과 처리
	bool Update_Period( float fDelta );
	// 만료시간 처리
	void Update_Expired( float fDelta );

	// 버프 얻을때 효과처리
	bool OnGain();
	// 버프 잃을때 효과처리
	bool OnLost(bool bRoute);

	// 액터 이벤트 검사
	bool CheckEvent(TALENT_CONDITION nCondition);
	// 주기 효과
	bool OnPeriod();
private:
	// 걸린 버프의 정보
	GBuffInfo*			m_pInfo;
	// 버프를 받은 액터
	GEntitySync*		m_pOwner;
	// 버프 만료 알리미
	MRegulator			m_rgrExpire;
	// 버프 주기적효과 알리미
	GTickRegulator		m_rgrPeriod;
	// 틱당 유지 비용 알리미
	GTickRegulator		m_rgrTickCost;
	// 버프가 영원히 지속될지 여부
	bool				m_bInifinity;
	// 버프 해제 처리자
	GBuffReleaser*		m_pReleaser;
	// 버프 스택 처리자
	GBuffStack*			m_pStack;
	// 액터 이벤트 알림이
	GEFfectTimingChecker*	m_pEffectTimingChecker;
	// 버프를 걸리게 만든 탤런트 포인터
	GTalentInfo*		m_pTalentInfo;
	// 버프를 걸리게 만든 액터 UID
	MUID				m_uidUser;
	// 버프 즉시효과 적용자
	GBuffInstantApplier* m_pInstantApplyer;
	// 버프 보정효과 적용자
	GBuffModifierApplier* m_pModifierApplyer;
	// 버프 트리거 적용자
	GBuffTriggerApplier* m_pTriggerApplyer;
	// 버프 패킷 전송자
	GBuffRouter*		m_pRouter;
	// 버프 만료타입
	DESTROY_TYPE		m_nDestroyType;
	// 버프 스크립트
	GBuffScript*		m_pBuffScript;
	// 탤런트 팔레트 변경자
	GBuffPaletteChanger*	m_pPaletteChanger;
	// 탤런트 장비 변경자
	GBuffEquipmentChanger*	m_pEquipmentChanger;
	// Gain 효과 받았는지 여부
	bool				m_bGainEffectOccured;
};
