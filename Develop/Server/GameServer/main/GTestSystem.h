#pragma once

#include "MCommand.h"
#include "GDespawnReporter.h"
#include "CSDef.h"
#include "CTransData.h"
#include "MTime.h"

using namespace minet;

class GFieldInfo;
class GEntitySync;
class GEntityActor;
class GEntityPlayer;
class GTalentInfo;
class CSLogListener;
struct GDamageRangedInfo;
struct GCombatTurnResult;


/// 테스트와 관련된 사항은 이쪽으로 몰아서 처리합시다. - birdkr
class GTestSystem : public MTestMemPool<GTestSystem>
{
private:
	struct PosSendInfo
	{
		MUID uidSubscriptors;
		MRegulator rgrTick;
	};

	GDebugReporter m_DespawnReporter;
	GDebugReporter m_PickingReporter;
	CSLogListener*	m_pListener;
	map<MUID, PosSendInfo>	m_mapSubscriptors_ActorPos;
	map<MUID, PosSendInfo>	m_mapSubscriptors_ActorHitCapsule;

private:
	void OnRuntimeLog(const wchar_t* szLog);
public:
	GTestSystem();
	virtual ~GTestSystem() {}

	void SetListener(CSLogListener* pListener);

	// Util ---
	bool IsPublishDefined();


	// Request Command --
	void SwitchAIRunning();
	void SwitchAIRunning(GEntityPlayer* pEntityPlayer=NULL);
	void RequestNPCAIRunning(MCommand* pCmd);
	void RequestAICommand(MCommand* pCmd);
	void RequestAISetMonitorNPC(MCommand* pCmd);
	void RequestDebugString(MCommand* pCmd);
	void RequestGMGetPlayerUID(MCommand* pCmd);

	void RouteAIDebugMessage(GEntitySync* pEntity, const wchar_t* szMessage);
	void RouteCombatCalcDebug(GEntityPlayer* pPlayer, const wchar_t* pszType, const wchar_t* pszText);
	void RouteDamageDebug( GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, const GDamageRangedInfo& DamageInfo, GCombatTurnResult &ret, float fResistRatePercent );
	void RouteToClientActorRenderCapsule( MUID uidRequester, GEntityActor* pOwner, const vector<MCapsule>& vecCapsules );
	// Mass Spawn Test --
	void CreateMassSpawnTestFields(GFieldInfo* pFieldInfo);

	// Despawn Reporter --
	void DespawnLog(MUID uidNPC, wstring strText);

	// Picking Reporter --
	void PickingLog(MUID uidNPC, wstring strText);

	// Route Client
	void ShowGuardFormula(GEntityActor* pGuarder, int nDamage, float fReduce, GUARD_TYPE nGuardType);
	// NPC
	void SetNPCMonitor(UIID nPlayerUIID, UIID nNPCUIID);
	// NaN 검사, 실패시 덤프 생성하고 프로그램 종료	
	void CheckNaN( const wchar_t* pszLocation, vec3 v );
	// 클라에 캡슐 그리기
	void RouteToClientRenderCapsule( MUID uidRequester, GEntityActor* pOwner, const MCapsule& capsule );
	// 클라에 히트캡슐 그리기
	void ClientRenderHitCapsule( GEntityActor* pOwner, const MCapsule& capsule );
	void ClientRenderActorCapsule( MUID uidPlayer, GEntityActor* pThisEntity );
	bool RouteActorPos( MUID uidPlayer );
	void Update(float fDelta);
	bool RouteAllNearActorCapsule( MUID uidPlayer );
	void RouteToClientActorRenderCapsuleClear(MUID uidRequester);
};

