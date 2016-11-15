#pragma once

#include "GColtInfo.h" // for GColtActionInstance

struct GColtCheck;

class GColtActionInstMgr
{
private:
	typedef map<const GColtCheck*, vector<GColtActionInst> >		ActionInstMap;
	typedef pair<const GColtAction*, size_t> ActionIndexPair;

public:
	GColtActionInstMgr(void);
	~GColtActionInstMgr(void);

	void Reset();

	void CalcActionRate( const GColtCheck& check );
	
	size_t GetActionInstanceSize() const			{ return m_mapActionInsts.size(); }
	size_t GetAutorunActionInstanceSize() const 	{ return m_mapAutorunActionInsts.size(); }

	vector<GColtActionInst>& LookupActionInst(const GColtCheck& check);
	vector<GColtActionInst>& LookupAutorunActionInst(const GColtCheck& check);
	
private:
	vector<GColtActionInst>& LookupActionInstImpl(const GColtCheck& check, ActionInstMap& mapActionInstance, const vector<GColtAction>& ActionInfos);
	void DistributeActionRate( const GColtCheck &check, vector<GColtActionInst> &vecActionInst );
	void DistributeOverActionRate( int nTotalRate, vector<ActionIndexPair> &vecUnratedActions, vector<GColtActionInst> & vecActionInst );
	void NormalizeActionRate( int nTotalRate, vector<ActionIndexPair> &vecNormalizableActions, vector<GColtActionInst> &vecActionInst );
private:
	ActionInstMap			m_mapActionInsts;							///< 상황마다의 액션 인스턴스 값
	ActionInstMap			m_mapAutorunActionInsts;					///< 상황마다의 오토런 액션 인스턴스 값
	vector<ActionIndexPair> m_vecNormalizableActions;
	vector<ActionIndexPair> m_vecUnratedActions;
};
