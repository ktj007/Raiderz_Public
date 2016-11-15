#pragma once

#include "CSTriggerInfo.h"
#include "GEntityActor.h"
#include "GGlueActor.h"	// for GHitInfo
#include "GScriptUtil.h"

class GGlueNPC;
class GGlueActor;
class GGluePlayer;
class GGlueField;
struct SPAWN_INFO;
class GNPCInfo;
class GNPCScriptBuilder;


class GScriptGroup_NPC : public MTestMemPool<GScriptGroup_NPC>
{
public:
	GScriptGroup_NPC(void);
	~GScriptGroup_NPC(void);

	void RegisterGlues();

	bool Reload(GNPCInfo* pInfo);
	bool OnInit(GNPCInfo* pNewInfo);

	bool OnSpawn(GGlueNPC* pNPC, GGlueField* pField, int nSpawnID);
	bool OnDie(GGlueNPC* pNPC, GGlueField* pField, int nSpawnID);
	bool OnYell(GGlueNPC* pNPC, GGlueNPC* pRequester, GGlueActor* pTarget);
	bool OnInteract( GGlueNPC* pNPC, GGluePlayer* pInteractor);
	bool OnDialogExit( GGlueNPC* pNPC, GGluePlayer* pPlayer, int nDialogID, int nExit);
	bool OnAggro(GGlueNPC* pNPC, GGlueActor* pTarget);
	bool OnTimer(GGlueNPC* pNPC, int nTimerID);
	bool OnUserdataChanged( GGlueNPC* pNPC, int nIndex, int nValue);
	bool OnHitCapsule( GGlueNPC* pNPC, GHitInfo& infoHit);
	bool OnSwalloweBegin( GGlueNPC* pNPC, GGlueActor* pTarget);
	bool OnHitSwallowed( GGlueNPC* pNPC, GHitInfo& infoHit);
	bool OnTryHit(GGlueNPC* pNPC, GGlueActor* pActor, const int nTalentID);
	bool OnChangeMode(GGlueNPC* pNPC, NPC_MODE nBeforeMode, NPC_MODE nMode);
	bool OnTalentEnd(GGlueNPC* pNPC, int nTalentID);
	bool OnVictory(GGlueNPC* pNPC);
	bool OnCombatBegin(GGlueNPC* pNPC);
	bool OnCombatEnd(GGlueNPC* pNPC);
	bool OnBPartRecver(GGlueNPC* pNPC);

private:
	bool PreCheck();
private:
	GScriptUtil m_ScriptUtil;
};
