#pragma once

#include "GModule.h"
#include "MMemPool.h"
#include "MReferenceCounted.h"
#include "CSTalentInfoEnum.h"
#include "CSTalentInfo.h"

// 디스펠 타입
enum DISPEL_TYPE
{
	DISPEL_NONE	= 0,

	DISPEL_MAGIC,			// BuffID 100 ~ 199
	DISPEL_POISON,			// BuffID 200 ~ 299
	DISPEL_DISEASE,			// BuffID 300 ~ 399
	DISPEL_CURSE,			// BuffID 400 ~ 499
	DISPEL_MESMERIZE,		// BuffID 500 ~ 599
	DISPEL_DEATH_PENALTY,	// BuffID 600 ~ 699
	
	DISPEL_BUFF,
	DISPEL_FOCUS,		// 사용안함 - Focus는 별도 제거
	DISPEL_BPART,

	MAX_DISPEL_TYPE	
};


class GTalentInfo;
class GBuffInfo;
class GEntitySync;
class GEntityActor;
class GBuff;
class GField;

TEST_FORWARD_FT(Buff_Stack, FBuff, IsStackable);

/// 버프 모듈
class GModuleBuff : public GModule, public MMemPool<GModuleBuff>
{
TEST_FRIEND_FT(Buff_Stack, FBuff, IsStackable);

	friend GEntityActor;

	DECLARE_ID(GMID_BUFF);

	struct LazyGainInfo
	{
		GBuffInfo* pBuffInfo;
		float fDurationTime;
		float fPeriodTime;
		MUID uidUser;
	};
public:
	GModuleBuff(GEntity* pOwner=NULL);
	virtual ~GModuleBuff(void);

	// 버프를 하나라도 가지고 있는지 여부
	bool			HasBuff() const;
	// 버프를 얻음 (저항 체크)
	bool			GainBuff( GBuffInfo* pBuffInfo, const CSBuffEnchantInfo& BuffInvokeInfo, GTalentInfo* pTalentInfo/*=NULL*/, GEntityActor* pUser );
	// 버프를 얻음 (저항 체크)
	bool			GainBuffDetail( GBuffInfo* pBuffInfo, const CSBuffEnchantInfo& BuffInvokeInfo, GTalentInfo* pTalentInfo/*=NULL*/, GEntityActor* pUser, float fDuration, float fPeriod );
	// 버프를 조건 없이 얻음
	bool			GainBuffForced(GBuffInfo* pBuffInfo, float fDurationTime, float fPeriodTime, GTalentInfo* pTalentInfo=NULL, MUID uidUser=MUID::Invalid());
	// 다음 틱에 버프를 얻음
	void			LazyGainBuff( GBuffInfo* pBuffInfo, float fDurationTime, float fPeriodTime, MUID uidUser );
	// 특정 버프를 잃음
	void			CancelBuff(int nBuffID);
	// 이로운 버프 갯수 반환
	int				GetBuffQty();
	// 해로운 버프 갯수 반환
	int				GetDebuffQty();
	// 해당 버프스택슬롯에 맞는 모든 버프 객체를 반환
	GBuff*			FindBuffBySlot(int BuffStackSlot);
	// 해당 버프스택슬롯에 맞는 모든 버프 객체를 반환
	GBuff*			FindBuffByID(int BuffID);
	// 모든 버프 아이디 리스트 반환
	vector<int>		GetAllBuffs();
	// 특정 버프가 걸려있는 여부를 반환
	bool			IsGained(int nBuffID);
	// 가장최근 마법류 디버프 해제
	void			Dispel(int nLimitPower);
	// 가장최근 독 디버프 해제
	void			CurePoison(int nLimitPower);
	// 가장최근 질병류 디버프 해제
	void			CureDisease(int nLimitPower);
	// 가장최근 저주 디버프 해제
	void			CureCurse(int nLimitPower);
	// 가장최근 이동불가형 디버프 해제
	void			DispelMesmerize(int nLimitPower);
	// 가장최근 부활 페널티 디버프 해제
	void			DispelDeathPenalty(int nLimitPower);
	// 가장최근 부활 페널티 디버프 해제
	void			DispelBPart();
	// 대상자의 이로운 버프 해제
	void			DispelBuff(int nLimitPower);
	// 대상자의 모든 포커스 해제
	void			DispelFocus();
	


	// 버프의 남은 시간 반환
	float			GetBuffRemainTime(int nBuffID);
	// 모든 버프의 아이디와 남은 시간 반환
	void			GetBuffRemainTimes(vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, bool bExceptFocusBuff = false, bool bExceptNoSaveDB = false);
	// 모든 버프의 아이디와 남은 시간 반환시 중복되는 버프가 있는지 검사.
	void			CheckDuplicatedRemainBuff(GEntity* pOwner, vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, const int nBuffID, const float fRemainTime);
	// 버프 남은 시간 추가하기
	void			InsertBuffRemainTimes(const vector<REMAIN_BUFF_TIME>& vecBuffRemainTime);
	// 현재 걸려있는 버프로 무적인지 여부
	bool			IsNowInvincibility()					{ return m_bInvincibility; }
	// 현재 걸려있는 버프로 모든 탤런트가 사용 불가능 여부 확인
	bool			IsDisableAllTalent()					{ return m_bDisableAllTalent; }
	// 현재 걸려있는 버프로 모든 탤런트가 사용 불가능 여부 확인
	bool			IsDisableTalent(TALENT_CATEGORY nCategory)	{ return m_bDisableTalent[nCategory]; }
	// 현재 걸려있는 버프로 최면상태인지 여부 확인
	bool			IsMesmerize()							{ return m_bMesmerize; }
	// 현재 걸려있는 버프로 투명화인지 여부
	bool			IsNowInvisibility()						{ return m_bInvisibility; }
	// 현재 걸려있는 버프로 NPC에게 투명인지 여부
	bool			IsNowInvisibilityToNPC()				{ return m_bInvisibilityToNPC; }
	// 현재 걸려있는 버프로 절대방어가 가능한지 여부
	bool			IsNowAbsoluteGuard()					{ return m_bAbsoluteGuard; }
	// 현재 걸려있는 버프로 QuestPVP로 죽었을때 아이템이 드랍안되는지 여부
	bool			IsNowDisableQuestPVPDrop()				{ return m_bDisableQuestPVPDrop; }
	
	// 특정 버프 여러개를 디스펠
	void			DispelBuff(int nDispellQty, int nLimitPower);	

	// 무적 버프를 얻음
	bool			GainInvincibleBuff_Rebirth();
	bool			GainInvincibleBuff_EnterField();
	bool			GainInvincibleBuff_Infinity();
	bool			GainInvisibleToNPCBuff_Infinity();

	// 다음틱에 모든 버프 삭제
	void			LazyLostAll();
	// 무적 버프를 잃음
	bool			LostInvincibleBuff();
	// NPC에게 투명 버프를 잃음
	bool			LostInvisibleToNPCBuff();

	// 액티브 특수속성 활성화
	void			EnableActiveExtraAttrib(GBuff* pBuff);
	// 특정 버프스택슬롯에 맞는 모든 버프를 잃음
	void			LostBuffStack(int BuffStackSlot);
	
	// 모든 디버프를 강제로 제거
	void RemoveAllDebuff();

	// 장비/팔레트변경 버프 강제로 제거
	void RemoveAllChangeEquipmentBuffs(int nExceptBuff);
	bool IsGained_ForTest( int nBuffID );
private:	
	// 모든 버프를 잃음 (외부로 노출되면 안됨, 크래쉬 위험)
	void			LostAll(bool bRoute);
	// 주기적 틱 처리
	virtual void	OnUpdate(float fDelta);	
	// 개별적인 버프 얻음 효과 처리
	bool			GainEffect( GBuff* pBuff );
	// 개별적인 버프 잃음 효과 처리
	void			DestroyBuff(GBuff* pBuff, bool bRoute);	
	// 해당 버프슬롯보다 더 강한 버프가 걸려있는지 여부를 반환
	bool			IsGainedPowerfulBuff(int nNewBuffSlot, int nNewBuffPower);
	// 특성 버프 1개를 디스펠
	void			DispelSingleBuffImpl(int nBuffID);
	// 가장최근 디버프 하나 해제
	void			DispelLatestBuff(DISPEL_TYPE dispelType, int nLimitPower );
	// 디스펠 가능 버프 확인
	bool			IsDispelTypeBuff(DISPEL_TYPE dispelType, int nStackSlot);
	// 스택가능한 버프인지 여부
	bool			IsStackable(int nBuffID);
private:	
	// 패시브 특수속성 갱신
	void			RefreshPassiveExtraAttrib();
	// 패시브 특수속성 활성화
	void			EnablePassiveExtraAttrib(GBuff* pBuff);
	// 모든 패시브 특수속성 비활성화
	void			DisablePassiveExtraAttrib();
	
	// 활성화된 액티브 특수속성 적용
	void			ApplyActiveExtraAttrib();	
	// 활성화된 패시브 특수속성 적용
	void			ApplyPassiveExtraAttrib(GBuff* pBuff);
	// 모든 액티브 특수속성 비활성화
	void			DisableActiveExtraAttrib();		
	// 버프 목록 업데이트, 반환값은 만료됐는지 여부
	void BuffUpdate( vector<GBuff*>& vecBuff, float fDelta );
	// lazy하게 얻을 버프들 추가
	void UpdateLazyGainBuffs();
	// 버프 얻기에 대한 저항처리
	bool ProcessGainResist( const CSBuffEnchantInfo &BuffInvokeInfo, GEntityActor* pUser, GTalentInfo* pTalentInfo );
	
private:
	// 이동속도 보정값의 합
	float					m_fMoveSpeedMod;
	// 활성화된 버프목록
	vector<GBuff*>			m_vecBuff;
	// 레퍼런스 카운터 매니저
	MRefMgr					m_RefMgr;
	// 버프로 인해 무적인지 여부
	bool					m_bInvincibility;
	// 버프로 인해 탤런트 사용가능 여부
	bool					m_bDisableTalent[TC_MAX];
	bool					m_bDisableAllTalent;
	// 버프로 인해 최면 상태인지 여부
	bool					m_bMesmerize;
	// 버프로 인해 죽어야 하는지 여부
	bool					m_bDieable;
	// 버프로 인해 소멸되야 하는지 여부 (재스폰O)
	bool					m_bDespawnOnceable;
	// 버프로 인해 소멸되야 하는지 여부 (재스폰X)
	bool					m_bDespawnEternalable;
	// 버프로 인해 투명인지 여부
	bool					m_bInvisibility;
	// 버프로 인해 NPC에게 투명인지 여부
	bool					m_bInvisibilityToNPC;
	// 버프로 인해 절대방어 상태가능한지 여부
	bool					m_bAbsoluteGuard;
	// 버프로 QuestPVP로 죽었을때 아이템이 드랍안되는지 여부
	bool					m_bDisableQuestPVPDrop;
	// 업데이트 틱
	MRegulator				m_rgrTick;
	// 누적된 틱 타임
	float					m_fSumTickTime;
	// 다음 틱에 얻을 버프 목록
	vector<LazyGainInfo>	m_vecLazyGainBuffs;

};
