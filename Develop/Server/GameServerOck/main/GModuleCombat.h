#ifndef _GMODULE_COMBAT_H
#define _GMODULE_COMBAT_H

#include "GModule.h"
#include "GTalentInfo.h"
#include "GTalentHitter.h"
#include "CSdef.h"
#include "MMemPool.h"
#include "MTime.h"

enum CCommandResultTable;
class GTalent;
class GEntityActor;
class GTalentHitter;
class GTalentFocusMgr;
class GItem;
class GEntityPlayer;

////////////////////////////////////////////////////////////////

class GInvincibleProcessor
{
private:
	bool		m_bInvincible;
	MRegulator	m_Regulator;
public:
	GInvincibleProcessor();

	bool		IsInvincible();
	void		DoInvincible(float fDurationTime);
	void		Update(float fDelta);
};


class GTalentRangeChecker
{
public:
	bool Check(GTalentInfo* pAttackTalentInfo, const vec3& vAttackerPos, const vec3& vAttackerDir, float fAttackerRadius, const vec3& vTargetPos, float fTargetRadius);
};

/// 전투상태
class GModuleCombat : public GModule, public MMemPool<GModuleCombat>
{
private:
	friend class GEntityActor;
	DECLARE_ID(GMID_COMBAT);
protected:
	GTalent*				m_pTalent;
	GTalentHitterList		m_TargetHitterList;
	GEntityActor*			m_pOwnerActor;
	GInvincibleProcessor	m_Invincible;						///< 무적 여부
	GTalentFocusMgr*		m_pTalentFocusMgr;

	virtual void			OnUpdate(float fDelta);
	void					UpdateActiveTalents(float fDelta);

	bool IsDeletable();

private:
	// 스킬 관련
	CCommandResultTable CheckPlayerTalentEnable(GEntityPlayer* pOwnerPlayer, GTalentInfo* pTalentInfo);

public:
	GModuleCombat(GEntity* pOwner);
	virtual ~GModuleCombat(void);

	virtual void			OnInitialized();	///< 초기화될 때 호출
	virtual void			OnFinalized();		///< 마무리될 때 호출

	void Clear();
	// 탤런트 사용 관련 ------------------
	CCommandResultTable UseTalent(GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bCheckEnabled, bool bGainStress=true);
	CCommandResultTable IsUsableTalent(GTalentInfo* pTalentInfo);
	// 탤런트를 취소 (탤런트정보의 cancelable속성이 맞을 경우에만 해당)
	void CancelTalent(bool bPostCommand=true);
	// 탤런트를 강제로 취소
	void CancelTalentForce(bool bPostCommand=true);
	void AddTargetHitter(GTalentHitter* pNewHitter);
	vector<GTalentHitter*> GetTargetHitter( TARGEHITTER_TYPE nType );

	void ActTalent(TALENT_TARGET_INFO Target);
	void DeleteCurTalent();

	void SetInvincible(float fDurationTime)				{ m_Invincible.DoInvincible(fDurationTime); }
	bool IsUsingMovingTalent();
	bool IsUsingNotCancelableTalent();
	bool IsUsingTalent();
	int GetUsingTalentID();
	bool IsNowInvincibleTime();
	bool IsNowSuperarmorTime();
	bool IsNowAvoidTime();

	// 특정 포커스가 걸려있는지 여부
	bool HasFocus(TALENT_FOCUS_TYPE nFocus);
	// 포커스가 하나라도 걸려있는지 여부
	bool HasFocus();
	// 광포 버블 갯수 반환
	int GetBerserkBubble();

	// 전투 판정 관련


	// for Test
	GTalent* GetTalent()		{ return m_pTalent; }

	bool CheckTalentRange(GEntityActor* pTargetActor, int nTalentID);

	float GetTalentElapsedTime();
	float GetTalentActElapsedTime();
	
};


#endif // _GMODULE_COMBATSTATUS_H