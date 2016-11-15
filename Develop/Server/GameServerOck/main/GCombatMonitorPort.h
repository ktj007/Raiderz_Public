#pragma once

#include "GActorObserver.h"
#include "GMonitorPortBase.h"

class GField;
class GEntityActor;

class GCombatMonitorPort : public GActorObserver, public GMonitorPortBase
{
public:
	GCombatMonitorPort(void);
	virtual ~GCombatMonitorPort(void);

	static GCombatMonitorPort& Inst();
	void	OnMsg( PCOPYDATASTRUCT pcds);

	void Prepare();
	void Start();
	void Stop();
	void Update(float fDelta);
	void Reset();
	void SetField(GField* pField);
	void AttachSubject(GEntityActor* pActor);
	void DettachSubject(GEntityActor* pActor);

	size_t GetSubjectQty() { return m_setSubjects.size(); }

private:
	void StopImpl();
	string ExtractFromActor( GEntityActor* pActor );
	void SendCommand(string strType);
	virtual void Send( string strMessage ) override;
	virtual void OnSendFailed() override;
	void SendStats( const char* szType, GEntityActor* pUser, GEntityActor* pAttacker, const char* szSourceType, int nSourceID, int nDamage=0);
	int GetActorID(GEntityActor* pActor);
private:
	virtual bool IsEnable() override;
	virtual const wchar_t* GetName() override { return L"Combat Monitor"; }
	virtual const char* GetFilePath() override { return "C:\\Works\\RaiderZ\\Develop\\Tools\\CombatMonitor\\bin\\Debug\\CombatMonitor.exe"; }
private:
	// 전투 시작
	virtual void OnCombatBegin(GEntityActor* pOwner) override;
	// 전투 종료
	virtual void OnCombatEnd(GEntityActor* pOwner) override;
	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 탤런트 사용이 효과가 발동될때 호출되는 이벤트
	virtual void OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 피격 이벤트
	virtual void OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 피격 성공 이벤트 (탤런트 종료될 때 1회 호출)
	virtual void OnHitSuccess( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) override;
	// 모든 피격 성공 이벤트 (히트캡슐판정 용도)
	virtual void OnHitAll( GEntityActor* pOwner, GTalentInfo* pTalentInfo ) override;
	// 치명타 피격 이벤트
	virtual void OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo ) override;
	// 빗맞음 이벤트
	virtual void OnMiss(GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  override;
	// 방어 이벤트 (모든 방어)
	virtual void OnGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  override;
	// 완전 방어 이벤트
	virtual void OnPerfectGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  override;
	// 부분 방어 이벤트
	virtual void OnPartialGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  override;
	// 절대 방어 이벤트
	virtual void OnAbsoluteGuard(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo)  override;
	// 탤런트를 무효시켰을때 호출되는 이벤트
	virtual void OnTalentImmuned(GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	// 데미지를 얻었을 때 이벤트
	virtual void OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID) override;
private:
	GField*	m_pField;
	set<MUID> m_setSubjects;
	bool m_bRunning;
	bool m_bWillStop;

};
