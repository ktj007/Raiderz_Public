#include "StdAfx.h"
#include "GDef.h"
#include "GColtPicker.h"
#include "GColtChecker.h"
#include "GColtInfo.h"
#include "GColtDef.h"
#include "GColtActionInstMgr.h"
#include "GStrings.h" // for GStrings::ColtActionToString
#include "GAIProfiler.h" // for AI_PFI_ACTION_BLOCK
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GTalentInfoMgr.h"
#include "GModuleCombat.h"
#include "CCommandResultTable.h" // for CR_SUCCESS

GColtPicker::GColtPicker(SectionType nSectionType, ColtType nColtType, vector<GColtCheck*>* vecChecks, GColtCheck* pDefaultCheck )
: m_nType(nColtType)
, m_vecChecks(NULL)
, m_pDefaultCheck(NULL)
, m_vecMultiChecks(NULL)
, m_pMultiDefaultCheck(NULL)
{
	m_pConditioner	= new GColtChecker();

	switch(nSectionType)
	{
	case SECTION_SINGLE:
		m_bMultiSection = false;
		m_vecChecks = vecChecks;
		m_pDefaultCheck = pDefaultCheck;
		break;
	case SECTION_MULTI:
		m_bMultiSection = true;
		m_vecMultiChecks = vecChecks;
		m_pMultiDefaultCheck = pDefaultCheck;
		break;
	}
}

GColtPicker::~GColtPicker()
{
	SAFE_DELETE(m_pConditioner);
}

GColtCheck* GColtPicker::PickCheck(GEntityNPC* pOwnerNPC)
{
	PFI_BLOCK_LINE(801, "GColt::PickCheck");
	VALID_RET(pOwnerNPC, NULL);

	GColtCheck* pPickedCheck = NULL;
	if (m_bMultiSection)
	{
		pPickedCheck =
			PickCheckImpl(m_vecMultiChecks[pOwnerNPC->GetMode()], pOwnerNPC);

		return pPickedCheck?pPickedCheck:&m_pMultiDefaultCheck[pOwnerNPC->GetMode()];
	}
	else
	{
		VALID_RET(m_vecChecks, NULL);

		pPickedCheck =
			PickCheckImpl(*m_vecChecks, pOwnerNPC);

		return pPickedCheck?pPickedCheck:m_pDefaultCheck;
	}
	
	// 여기까지 올 수 없음
	return NULL;
}

GColtCheck* GColtPicker::PickCheckImpl(const vector<GColtCheck*>& vecChecks, GEntityNPC* pOwnerNPC)
{
	for (size_t i=0; i<vecChecks.size(); i++)
	{
		GColtCheck* pCheck = vecChecks[i];
		if (!pCheck)
			continue;

		if (CheckCondition(*pCheck, pOwnerNPC))
		{
			if (pCheck->vecChecks.empty())
			{
				// 중첩 체크가 없다면 해당 조건 만족
				return pCheck;
			}
			else
			{
				// 중첩 체크
				GColtCheck* pInnerCheck = PickCheckImpl(pCheck->vecChecks, pOwnerNPC);
				if (pInnerCheck)
					return pInnerCheck; // 하위에서 선택된 경우

				if (pCheck->HasActions())
					return pCheck; // 현재 뎁스 체크에 액션이 있는 경우 선택
			}
		}
	}

	return NULL;
}

RUNNING_ACTION_LIST GColtPicker::PickAction(const GColtCheck& check)  
{ 	
	PFI_BLOCK_THISFUNC(806);

	// 뽑힐확률 재계산
	m_ActionInstMgr.CalcActionRate(check);
	
	// 오토런은 처음에 등록
	RUNNING_ACTION_LIST vecRunningAction;
	{
		MakeAutorunActionList(vecRunningAction, check, m_ActionInstMgr.LookupAutorunActionInst(check));
	}
	
	// 확률 뽑기
	int nPickValue = RandomNumber(0, MAX_AI_RATE-1);

	// 액션 인스턴스 구하기
	vector<GColtActionInst>& vecActionInstInfos = m_ActionInstMgr.LookupActionInst(check);
	
	{
		PFI_BLOCK_THISFUNC(6613); // test
		for (size_t i=0; i<check.vecActions.size(); i++)
		{
			const GColtAction& action = check.vecActions[i];
			GColtActionInst& actioninst = vecActionInstInfos[i];
			int nModifedRate = actioninst.GetRate();

			// 이번 액션 확률 빼기
			nPickValue -= nModifedRate;

			if (action.bAutoRun)
				continue; // 오토런은 뽑지 않음

			if (!actioninst.IsEnableCycleTime())
				continue;  // 싸이클타임 끝나지 않음

			// 남은 값이 음수면 확률 통과
			if (nPickValue < 0)
			{
				// 액션 당첨
				actioninst.ResetCycleTime(action.fCycleTime);
				MakeActionList(vecRunningAction, action, actioninst);

				return vecRunningAction;
			}
		}
	}
	
	return vecRunningAction;
}


void GColtPicker::MakeActionListImpl( RUNNING_ACTION_LIST& vecRunningAction, const GColtAction& action, GColtActionInst& actioninst )
{
	PFI_BLOCK_THISFUNC(803);

	vecRunningAction.push_back(pair<const GColtAction*, GColtActionInst*>(&action, &actioninst));

	vector<GColtActionInst>& vecInnerInfos = actioninst.GetInnerInfos();

	DCHECK(action.vecSeqActions.size() == vecInnerInfos.size());

	for (size_t i=0; i<action.vecSeqActions.size(); i++)
	{
		vecRunningAction.push_back(pair<const GColtAction*, GColtActionInst*>(&action.vecSeqActions[i], &vecInnerInfos[i]));
	}
}

void GColtPicker::MakeActionList( RUNNING_ACTION_LIST& vecRunningAction, const GColtAction& action, GColtActionInst& actioninst )
{
	PFI_BLOCK_THISFUNC(803);

	AI_PFI_ACTION_BLOCK(action.nType, GStrings::ColtActionToString(action.nType));

	MakeActionListImpl(vecRunningAction, action, actioninst);
}

void GColtPicker::MakeAutorunActionList( RUNNING_ACTION_LIST& vecRunningAction, const GColtCheck& check, vector<GColtActionInst>& autorun_actioninst_list )
{
	PFI_BLOCK_THISFUNC(804);

	DCHECK(check.vecAutoRunActions.size() == autorun_actioninst_list.size());

	for (size_t i=0; i<check.vecAutoRunActions.size(); i++)
	{
		const GColtAction& autorun_action = check.vecAutoRunActions[i];
		GColtActionInst& autorun_actioninst = autorun_actioninst_list[i];

		if (!autorun_action.IsPickable(autorun_actioninst))	
			continue;

		MakeActionListImpl(vecRunningAction, autorun_action, autorun_actioninst );
	}
}

bool GColtPicker::CheckCondition( const GColtCheck& check, GEntityNPC* pOwnerNPC )
{
	// 액션 확률 재계산
	m_ActionInstMgr.CalcActionRate(check);

	const vector<GColtActionInst>& actioninst = 
		m_ActionInstMgr.LookupActionInst(check);

	const vector<GColtActionInst>& autorunactioninst = 
		m_ActionInstMgr.LookupAutorunActionInst(check);

	if (!m_pConditioner->CheckCondition(check, pOwnerNPC))
		return false; // 체크문 조건에 충족하지 않음

	GPercentDice dice;
	if (!dice.Dice(check.nPickRate))	
		return false; // 선택확률 실패

	if (check.HasInnerChecks())
		return true; // 하위 체크문이 있으면 액션 유효성 판단없이 선택됨

	if (CheckActions_AutorunActions(autorunactioninst, check) ||
		CheckActions(actioninst, check, pOwnerNPC))
	{
		// Action / Autorun 중 하나라도 실행 가능하면 유효한 체크문
		return true;
	}

	return false;
}

void GColtPicker::Reset()
{
	m_ActionInstMgr.Reset();
}

bool GColtPicker::CheckActions(const vector<GColtActionInst>& actioninst, const GColtCheck& check, GEntityNPC* pOwnerNPC)
{
	if (!CheckActions_CycleTime(actioninst))
		return false; // 싸이클 타임이 모두 활성화되었음

	if (!CheckActions_PickRate(actioninst, check))
		return false; // 사용할 액션 중에 확률이 있는게 없음

	if (!CheckActions_UseCount(actioninst, check.vecActions))
		return false; // 최대 사용 횟수 초과

	if (!check.CheckActionConditions())
		return false; // 검사할 필요가 없는 체크문

	if (!CheckActions_InnerConditions(pOwnerNPC, check))
		return false; // 하위 액션중에 유효한 액션이 없음

	return true;
}

bool GColtPicker::CheckActions_InnerConditions(GEntityNPC* pOwnerNPC, const GColtCheck& check)
{
	for each (const GColtAction& each in check.vecActions)
	{
		if (each.nType == CAT_TALENT)
		{
			int nTalentID = _wtoi(each.qParams[0].c_str());
			GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
			if (!pTalentInfo)	
				continue;

			if (pOwnerNPC->GetModuleCombat()->IsUsableTalent(pTalentInfo) == CR_SUCCESS)
				return true; // 사용 가능한 탤런트!
		}
		else
		{
			return true; // 탤런트가 아닌 액션은 사용 가능하다고 가정
		}
	}

	return false;
}

bool GColtPicker::CheckActions_CycleTime( const vector<GColtActionInst> &actioninst )
{
	for (size_t i=0; i<actioninst.size(); i++)
	{
		const GColtActionInst& ActionInst = actioninst[i];

		if (ActionInst.IsEnableCycleTime())
			return true;
	}	
	return false;
}

bool GColtPicker::CheckActions_PickRate( const vector<GColtActionInst> &actioninst, const GColtCheck &check )
{
	long nTotalRate=0;
	for (size_t i=0; i<actioninst.size(); i++)
	{
		const GColtActionInst& ActionInst = actioninst[i];
		nTotalRate += ActionInst.GetRate();
	}

	return (nTotalRate > 0);
}

bool GColtPicker::CheckActions_UseCount( const vector<GColtActionInst> &actioninst, const vector<GColtAction>& vecActions )
{
	long nTotalCurCount=0;
	for (size_t i=0; i<actioninst.size(); i++)
	{
		const GColtActionInst& ActionInst = actioninst[i];
		nTotalCurCount += ActionInst.GetUseCount();
	}

	long nTotalMaxCount=0;
	for (size_t i=0; i<vecActions.size(); i++)
	{
		const GColtAction& Action = vecActions[i];

		if (Action.nMaxCount == -1)
			return true; // 사용 가능한 액션이 있음

		nTotalMaxCount += Action.nMaxCount;
	}	

	return (nTotalCurCount < nTotalMaxCount);
}

bool GColtPicker::CheckActions_AutorunActions( const vector<GColtActionInst> &autorunactioninst, const GColtCheck &check )
{
	if (!CheckActions_CycleTime(autorunactioninst))
		return false;

	if (!CheckActions_UseCount(autorunactioninst, check.vecAutoRunActions))
		return false; // 사용 횟수 초과

	return true;
}

bool GColtPicker::Has() const
{
	if (m_pDefaultCheck)
	{
		if (!m_pDefaultCheck->vecActions.empty())
			return true;

		if (!m_pDefaultCheck->vecAutoRunActions.empty())
			return true;
	}

	if (m_pMultiDefaultCheck)
	{
		if (!m_pMultiDefaultCheck->vecActions.empty())
			return true;

		if (!m_pMultiDefaultCheck->vecAutoRunActions.empty())
			return true;
	}

	if (m_vecChecks)
	{
		if (!m_vecChecks->empty())
			return true;
	}
	
	if (m_vecMultiChecks)
	{
		if (!m_vecMultiChecks->empty())
			return true;
	}

	return false;
}
