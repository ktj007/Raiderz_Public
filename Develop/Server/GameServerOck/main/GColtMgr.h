#pragma once

#include "GDef.h"
#include "GColtDef.h"

struct GColtCheck;
struct GColtGroupInfo;

class GColtPicker;
class GColtRunner;
class GColtTaskRunner;

class GJobMgr;
class GJobRunner;
class GTask;

class GEntityNPC;
class GEntityActor;



class GColtMgr : public MTestMemPool<GColtMgr>
{
public:
	GColtMgr(GJobMgr* pJobMgr, GJobRunner* pJobRunner);
	~GColtMgr(void);

	void Init(GColtGroupInfo* pColtInfo);
	void Clear();

	GTask*	RunColtIdleForJob(GEntityNPC* pThisNPC);
	
	bool	RunColtReaction(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor);
	bool	RunColtAggro(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor);
	bool	RunColtCombat(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor);
	bool	RunColtStress(GEntityNPC* pThisNPC);
	bool	RunColtIdle(GEntityNPC* pThisNPC);
	bool	RunColtPrologue(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor=NULL);
	bool	RunColtEpilogue(GEntityNPC* pThisNPC, GEntityActor* pOpponentActor=NULL);
	
	GColtPicker*	GetColtIdle()		{ return m_pColtIdle; }
	GColtPicker*	GetColtCombat()		{ return m_pColtCombat; }

	bool	HasPrologueColt() const;
	bool	HasEpilogueColt() const;

private:
	bool	RunColt(GColtPicker* pColt, GEntityNPC* pThisNPC, GEntityActor* pOpponentActor);	
	bool	RunColtActionList(const RUNNING_ACTION_LIST& vecRunningAction, GEntityNPC* pThisNPC, GEntityActor* pOpponentActor);

	bool	IsUsableDownAttack(int nTalentID, GEntityNPC* pThisNPC, GEntityActor* pTargetActor);

private:
	GColtPicker*		m_pColtReaction;
	GColtPicker*		m_pColtIdle;	
	GColtPicker*		m_pColtCombat;
	GColtPicker*		m_pColtAggro;
	GColtPicker*		m_pColtStress;
	GColtPicker*		m_pColtPrologue;
	GColtPicker*		m_pColtEpilogue;
	
	GColtRunner*		m_pColtJobRunner;
	GColtTaskRunner*	m_pColtTaskRunner;
	GJobMgr*			m_pJobMgr;
	GJobRunner*			m_pJobRunner;
};
