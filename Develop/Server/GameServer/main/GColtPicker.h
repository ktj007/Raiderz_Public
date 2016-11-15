#pragma once

#include "GColtDef.h"
#include "GColtActionInstMgr.h"

struct GColtCheck;
struct GColtAction;
class GColtActionInst;

class GEntityNPC;
class GColtChecker;
class GColtActionInstMgr;

//////////////////////////////////////////////////////////////////////////
//
//	GColtPicker
//
//////////////////////////////////////////////////////////////////////////
class GColtPicker : public MTestMemPool<GColtPicker>
{
public:
	enum SectionType
	{
		SECTION_SINGLE=0,
		SECTION_MULTI=1,
	};
public:
	GColtPicker(SectionType nSectionType, ColtType nColtType, vector<GColtCheck*>* vecChecks, GColtCheck* pDefaultCheck );
	~GColtPicker();

	GColtCheck* PickCheck(GEntityNPC* pOwnerNPC);
	RUNNING_ACTION_LIST PickAction(const GColtCheck& check);
	void	 Reset();							

	bool CheckCondition( const GColtCheck& check, GEntityNPC* pOwnerNPC );

	ColtType GetType() const					{ return m_nType; }
	size_t	 GetActionInstanceSize()			{ return m_ActionInstMgr.GetActionInstanceSize(); }
	size_t	 GetAutorunActionInstanceSize()		{ return m_ActionInstMgr.GetAutorunActionInstanceSize(); }
	GColtActionInstMgr& GetActionInstMgr()		{ return m_ActionInstMgr; }

	const vector<GColtActionInst>&	LookupActionInst(const GColtCheck& vecCheck)		{ return m_ActionInstMgr.LookupActionInst(vecCheck); }
	const vector<GColtActionInst>&	LookupAutorunActionInst(const GColtCheck& vecCheck)	{ return m_ActionInstMgr.LookupAutorunActionInst(vecCheck); }

	void CalcActionRate( const GColtCheck& vecCheck )									{ m_ActionInstMgr.CalcActionRate(vecCheck); }

	bool Has() const;

private:
	void MakeActionListImpl( RUNNING_ACTION_LIST& vecRunningAction, const GColtAction& action, GColtActionInst& actioninst );
	void MakeActionList( RUNNING_ACTION_LIST& vecRunningAction, const GColtAction& action, GColtActionInst& actioninst );
	void MakeAutorunActionList( RUNNING_ACTION_LIST& vecRunningAction, const GColtCheck& check, vector<GColtActionInst>& autorun_actioninst_list );

	bool CheckActions(const vector<GColtActionInst>& actioninst, const GColtCheck& check, GEntityNPC* pOwnerNPC);
	bool CheckActions_CycleTime( const vector<GColtActionInst> &actioninst );
	bool CheckActions_PickRate( const vector<GColtActionInst> &actioninst, const GColtCheck &check );
	bool CheckActions_UseCount( const vector<GColtActionInst> &actioninst, const vector<GColtAction>& vecActions );
	bool CheckActions_InnerConditions(GEntityNPC* pOwnerNPC, const GColtCheck& check);
	bool CheckActions_AutorunActions( const vector<GColtActionInst> &autorunactioninst, const GColtCheck &check );

	GColtCheck* PickCheckImpl(const vector<GColtCheck*>& vecChecks, GEntityNPC* pOwnerNPC);
private:
	ColtType				m_nType;
	GColtChecker*		m_pConditioner;
	GColtActionInstMgr		m_ActionInstMgr;
	
protected:
	// 여러가지 섹션을 동시에 쓸지 여부
	bool					m_bMultiSection;
	vector<GColtCheck*>*	m_vecChecks;
	vector<GColtCheck*>*	m_vecMultiChecks;
	GColtCheck*				m_pDefaultCheck; 
	GColtCheck*				m_pMultiDefaultCheck;
};
