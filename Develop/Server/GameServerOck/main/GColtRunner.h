#ifndef _GCOLT_RUNNER_H
#define _GCOLT_RUNNER_H

#include "GTask_Broker.h"
#include "GColtDef.h"

class GColt;
class GEntityNPC;
class GEntityActor;
struct GColtCheck;
struct GColtAction;
class GColtActionInst;
class GColtActionInstMgr;
class GJobRunner;

#define DECL_COLT_JOB_RUN(F)			void F(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent);
#define NPC_XML_ATTR_VICTORY			L"victory"

/// 즉시성 액션만 수행하는 러너
class GColtRunner : public MTestMemPool<GColtRunner>
{
public:
	GColtRunner(GJobRunner* pJobRunner=NULL): m_pJobRunner(pJobRunner) {}

	void MakeActionList( RUNNING_ACTION_LIST& vecRunningAction, const GColtAction& action, GColtActionInst& actioninst );
	void MakeAutorunActionList( RUNNING_ACTION_LIST& vecRunningAction, GColtCheck& check, vector<GColtActionInst>& autorun_actioninst_list );
	
	
	void RunAction(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent);
	bool RunColtActionList(const RUNNING_ACTION_LIST& vecActions, GEntityNPC* pThisNPC, GEntityActor* pOpponent);
	
	
private:
	DECL_COLT_JOB_RUN(OnNothing);
	DECL_COLT_JOB_RUN(OnTalent);
	DECL_COLT_JOB_RUN(OnLuaFunc);
	DECL_COLT_JOB_RUN(OnBreakpart);
	DECL_COLT_JOB_RUN(OnDamage);
	DECL_COLT_JOB_RUN(OnMotionfactor);
	DECL_COLT_JOB_RUN(OnFlee);
	DECL_COLT_JOB_RUN(OnYell);
	DECL_COLT_JOB_RUN(OnGuard);
	DECL_COLT_JOB_RUN(OnAdjust);
	DECL_COLT_JOB_RUN(OnDistance);
	DECL_COLT_JOB_RUN(OnFollow);
	DECL_COLT_JOB_RUN(OnMove);
	DECL_COLT_JOB_RUN(OnChangeHitCapsule);
	DECL_COLT_JOB_RUN(OnSay);
	DECL_COLT_JOB_RUN(OnBalloon);
	DECL_COLT_JOB_RUN(OnShout);
	DECL_COLT_JOB_RUN(OnNarration);
	DECL_COLT_JOB_RUN(OnAggro);
	DECL_COLT_JOB_RUN(OnFaceTo);
	
private:
	void MakeActionListImpl( RUNNING_ACTION_LIST& vecRunningAction, const GColtAction& action, GColtActionInst& actioninst );
	
private:
	GJobRunner* m_pJobRunner;
};



#define DECL_COLT_TASK_RUN(F)			GTask* F(const GColtAction* pPickedAction, GEntityNPC* pThisNPC, GEntityActor* pOpponent);

/// task을 만들어주는 액션을 수행하는 러너
class GColtTaskRunner : public MTestMemPool<GColtTaskRunner>
{
public:
	GColtTaskRunner(GJobRunner* pJobRunner): m_pJobRunner(pJobRunner) {}
	GTask* RunAIAction(const GColtAction* pPickedAction, GColtActionInst* pPickedActionInstance, GEntityNPC* pThisNPC, GEntityActor* pOpponent );
private:
	DECL_COLT_TASK_RUN(OnNothing);
	DECL_COLT_TASK_RUN(OnTalent);
	
private:
	GJobRunner* m_pJobRunner;
	GTaskBroker	m_TaskBroker;
};



#endif // _GCOLT_RUNNER_H