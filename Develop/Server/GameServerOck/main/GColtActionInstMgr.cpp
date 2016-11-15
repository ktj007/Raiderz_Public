#include "StdAfx.h"
#include "GColtActionInstMgr.h"
#include "GDef.h"
#include "GColtDef.h"

GColtActionInstMgr::GColtActionInstMgr(void)
{
}

GColtActionInstMgr::~GColtActionInstMgr(void)
{
}

void GColtActionInstMgr::Reset()
{
	m_mapActionInsts.clear();
	m_mapAutorunActionInsts.clear();
}

void GColtActionInstMgr::CalcActionRate( const GColtCheck& check )
{
	vector<GColtActionInst>& vecActionInst=
		LookupActionInst(check);

	DistributeActionRate(check, vecActionInst);
}

vector<GColtActionInst>& GColtActionInstMgr::LookupActionInst(const GColtCheck& check)
{
	PFI_BLOCK_THISFUNC(810);

	return LookupActionInstImpl(check, m_mapActionInsts, check.vecActions);
}

vector<GColtActionInst>& GColtActionInstMgr::LookupAutorunActionInst(const GColtCheck& check)
{
	PFI_BLOCK_THISFUNC(811);
	return LookupActionInstImpl(check, m_mapAutorunActionInsts, check.vecAutoRunActions);
}

vector<GColtActionInst>& GColtActionInstMgr::LookupActionInstImpl(const GColtCheck& check, 
																  ActionInstMap& mapActionInstance, 
																  const vector<GColtAction>& ActionInfos)
{
	PFI_BLOCK_THISFUNC(812);

	const GColtCheck* pvecCheck = &check;

	ActionInstMap::iterator iter;
	{
		PFI_BLOCK_THISFUNC(6620);
		iter = mapActionInstance.find(pvecCheck);
	}
	
	if (iter == mapActionInstance.end())
	{
		// 인스턴스를 새로 생성
		mapActionInstance.insert(ActionInstMap::value_type(pvecCheck, vector<GColtActionInst>()));
		vector<GColtActionInst>& vecInst = mapActionInstance[pvecCheck];

		for (size_t i=0; i<ActionInfos.size(); i++)
		{
			const GColtAction& eachAction = ActionInfos[i];
			vecInst.push_back(GColtActionInst(eachAction));
			DCHECK(vecInst.back().GetInnerInfos().size() == eachAction.vecSeqActions.size());
		}

		DCHECK(ActionInfos.size() == vecInst.size());
		return vecInst;
	}

	// 기존 인스턴스 참조
	return (iter->second);
}

void GColtActionInstMgr::DistributeActionRate( const GColtCheck &check, vector<GColtActionInst> &vecActionInst )
{
	PFI_BLOCK_THISFUNC(6610); // test

	int nTotalRate=0;
	m_vecNormalizableActions.clear();
	m_vecUnratedActions.clear();

	for (size_t i=0; i<check.vecActions.size(); i++)
	{
		const GColtAction& Action = check.vecActions[i];
		GColtActionInst& inst = vecActionInst[i];
		inst.SetRate(0);

		if (!Action.IsPickable(inst))
			continue;	

		if (Action.nRate == 0)		
			m_vecUnratedActions.push_back(ActionIndexPair(&Action, i));
		else
			m_vecNormalizableActions.push_back(ActionIndexPair(&Action, i));

		int nActionRate = max(Action.nRate, 0);
		inst.SetRate(nActionRate);
		nTotalRate += nActionRate;
	}

	if (nTotalRate < MAX_AI_RATE &&
		!m_vecUnratedActions.empty())
	{
		// 잉여값 분배
		DistributeOverActionRate(nTotalRate, m_vecUnratedActions, vecActionInst);
	}
	else
	{
		// 정규화
		NormalizeActionRate(nTotalRate, m_vecNormalizableActions, vecActionInst);
	}
}

void GColtActionInstMgr::DistributeOverActionRate( int nTotalRate, vector<ActionIndexPair> &vecUnratedActions, vector<GColtActionInst>& vecActionInst )
{
	PFI_BLOCK_THISFUNC(6611); // test
	int nRemainRate = MAX_AI_RATE-nTotalRate;
	int nEachRate=nRemainRate/(int)vecUnratedActions.size();

	for (size_t i=0; i<vecUnratedActions.size(); i++)
	{	
		size_t prevIndex		= vecUnratedActions[i].second;
		const GColtAction* pAction	= vecUnratedActions[i].first;
		GColtActionInst& action_inst = vecActionInst[prevIndex];
		action_inst.SetRate(nEachRate);
		nRemainRate -= action_inst.GetRate();

		// 나머지 값을 취함
		if (i == vecUnratedActions.size()-1)
			action_inst.SetRate(action_inst.GetRate() + nRemainRate);
	}
}

void GColtActionInstMgr::NormalizeActionRate( int nTotalRate, vector<ActionIndexPair> &vecNormalizableActions, vector<GColtActionInst>& vecActionInst )
{
	PFI_BLOCK_THISFUNC(6612); // test
	float fScale = 100.0f/nTotalRate;

	int nRemainRate = MAX_AI_RATE;
	for (size_t i=0; i<vecNormalizableActions.size(); i++)
	{
		size_t prevIndex			= vecNormalizableActions[i].second;
		const GColtAction* Action	= vecNormalizableActions[i].first;
		if (Action->nRate > 0)
		{
			GColtActionInst& action_inst = vecActionInst[prevIndex];
			action_inst.SetRate((int)((float)Action->nRate * fScale));
			nRemainRate -= action_inst.GetRate();
		}

		// 나머지 값을 취함
		if (i == vecNormalizableActions.size()-1)
		{
			GColtActionInst& front_action_inst = vecActionInst[vecNormalizableActions.front().second];
			front_action_inst.SetRate(front_action_inst.GetRate() + nRemainRate);
		}
	}
}