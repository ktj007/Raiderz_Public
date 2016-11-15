#pragma once

#include "CSBuffInfo.h"
#include "CSTalentInfoEnum.h"


class GBuffInfoMgr;

class GBuffInfo : public CSBuffInfo, public MTestMemPool<GBuffInfo>
{
private:
	friend class GBuffInfoMgr;
public:
	GBuffInfo(); 
	// 모션팩터를 변화시키는지 여부
	bool			IsModified_MF() const;
	bool			IsExistModValue() const	{ return m_bExistModValue; }
	bool			IsFocusBuff() const		{ return m_bFocusBuff; }
	bool			HasUseTalent() const	{ return m_nTalentID != INVALID_TALENT_ID; }

	bool 			IsInvincibility() const;
	bool 			IsInvisibilityToNPC() const;
	bool 			IsInvisibility() const;
	TALENT_CATEGORY IsDisableTalent() const;
	bool			IsMesmerize() const;
	bool			IsCancelable() const;
	bool			IsDieBuff() const;
	bool			IsDespawnOnceBuff() const;
	bool			IsDespawnEternalBuff() const;
	bool			IsAbsoluteGuard() const;
	bool			IsInstantEffect() const;
	bool			IsDisableQuestPVPDrop() const;
	bool			HasHealEffect() const; 
	
	void			Cooking();

	float			GetInvisibilityPower();				

private:
	void			InitSuffix();
public:
	// 버프 유지 시간
	float					m_fDuration;	
	// 버프 주기효과 시간
	float					m_fPeriod;	
	// 모디파이어값이 있는지 여부
	bool					m_bExistModValue;		
	// 버프 효과를 받을 수 있는 조건
	CSTalentConditionInfo	m_Condition;
	// 버프 효과를 받을 수 있는 대상
	CSEffectInfo			m_EffectInfo;
	// 모션팩터 그룹 변경 효과
	MOTION_FACTOR_GROUP		m_MotionFactorGroup;	
	// 틱당 유지비용(HP), 0이면 유지비용없음
	int						m_nHPCost;
	// 틱당 유지비용(EN), 0이면 유지비용없음
	int						m_nENCost;
	// 틱당 유지비용(STA), 0이면 유지비용없음
	int						m_nSTACost;
	// 포커스 버프인지 여부
	bool					m_bFocusBuff;
	// 효과 받을 때 사용할 탤런트
	int						m_nTalentID;
	// 헤이트 변화량
	int						m_nHateAdj;
	// 헤이트 가중치
	float					m_fHateFactor;
	// DB에 저장할지 여부
	bool					m_bSaveToDB;
	// 버프 정보 업데이트시 패킷 전송여부
	bool					m_bUpdateRoute;
};

/// 스킬 정보 관리자
class GBuffInfoMgr : public CSBuffInfoMgr, public MTestMemPool<GBuffInfoMgr>
{
private:
	void					ParseBuff(MXmlElement* pElement, MXml* pXml);
	void					ParseIsFocusBuff(GBuffInfo* pBuffInfo);

public:
	virtual ~GBuffInfoMgr();

	bool					Load(const wchar_t* szFileName);
	void					Clear();
	GBuffInfo*				Get(int nID);
	bool					Exist(int nID);
	void					Insert(GBuffInfo* pBuffInfo );
};	
