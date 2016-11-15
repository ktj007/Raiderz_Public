#include "stdafx.h"
#include "GColtValidator.h"
#include "GDef.h"
#include "GServer.h"
#include "GStrings.h"
#include "GNPCInfo.h"
#include "GNPCInfoMgr.h"
#include "GTalentInfo.h"
#include "GAIState.h"
#include "GTalentInfoMgr.h"
#include <deque>
#include "GValidateLogger.h"
#include "GBuffInfo.h"
#include "MLocale.h"

struct GColtValidateInfo
{
	deque<wstring> qCallstack; 
	GNPCInfo* pNPCInfo;
};

wstring ToString(int nNumber)
{
	wchar_t szText[128];
	_itow(nNumber, szText, 10);
	return szText;
}

bool GColtValidator::Check()
{
	if (gmgr.pNPCInfoMgr == NULL) return true;

	GNPCInfoMgr* pNPCInfoMgr = gmgr.pNPCInfoMgr;
	for (GNPCInfoMgr::iterator itor = pNPCInfoMgr->begin(); itor != pNPCInfoMgr->end(); ++itor)
	{
		GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itor).second);
		_ASSERT(pNPCInfo);
		if (!pNPCInfo)	continue;
		
		// NPC별 Colt 파일명 조합
		{
			wchar_t szID[512];
			swprintf_s(szID, L"%07d", pNPCInfo->nID);

			wstring strPath;
			strPath += PATH_SCRIPT_NPC_COLT;
			strPath += szID;
			strPath += FILEEXT_COLT;

			_VLI(strPath.c_str());
		}

		for (int i=0; i<NPC_MODE_MAX; ++i)
		_CheckColtGroup(L"Combat", pNPCInfo, &pNPCInfo->ColtInfo.checkCombatDefault[i], pNPCInfo->ColtInfo.vecCombatChecks[i]);
		_CheckColtGroup(L"Reaction", pNPCInfo, NULL, pNPCInfo->ColtInfo.vecReactionChecks);
		_CheckColtGroup(L"Aggro", pNPCInfo, &pNPCInfo->ColtInfo.checkAggroDefault, pNPCInfo->ColtInfo.vecAggroChecks);
		_CheckColtGroup(L"Stress", pNPCInfo, NULL, pNPCInfo->ColtInfo.vecStressChecks);
		_CheckColtGroup(L"Idle", pNPCInfo, &pNPCInfo->ColtInfo.checkIdleDefault, pNPCInfo->ColtInfo.vecIdleChecks);

		{
			_CheckColtAttribute(pNPCInfo);

		}
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

bool GColtValidator::_CheckAction(wstring strName, const GColtAction& ActionInfo, GColtValidateInfo& ValidateInfo )
{	
	ValidateInfo.qCallstack.push_back(strName);
	ValidateInfo.qCallstack.push_back(GStrings::ColtActionToString(ActionInfo.nType));

	const vector<wstring>& qParams = ActionInfo.qParams;

	switch(ActionInfo.nType)
	{
	case CAT_GROUP:
		{
		}break;
	case CAT_NOTHING:	
		{
			const float fDelayTime = (float)_wtof(qParams[0].c_str());

			_CheckActionParam(1, qParams, ValidateInfo);
			_CheckRange(L"DelayTime",	(int)fDelayTime, ValidateInfo, 0, 100000);
		}break;
	case CAT_TALENT:	
		{
			const int nTalentID = _wtoi(qParams[0].c_str());

			_CheckActionParam(1, qParams, ValidateInfo);
			_CheckRange(L"nTalentID",	nTalentID, ValidateInfo, 0, INT_MAX);
			_CheckNPCTalent(nTalentID, ValidateInfo);

			vector<TALENT_SKILL_TYPE> vecTalents;
			vecTalents.push_back(ST_MELEE);
			vecTalents.push_back(ST_MAGIC);			
			vecTalents.push_back(ST_EXTRA_ACTIVE);
			vecTalents.push_back(ST_SOCIAL);
			_CheckTalentTypeList(nTalentID, vecTalents, L"액션 탤런트", ValidateInfo);
		}break;
	case CAT_LUAFUNC:	
		{
			const wstring strFirst = qParams[0];
			const wstring strSecond = qParams[1];

			_CheckActionParam(1, qParams, ValidateInfo);

			if (strFirst.length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);

			_CheckLuaFunction(strFirst, strSecond, ValidateInfo);
			
		}break;
	case CAT_BREAKPART:	
		{
			wchar_t nPartID = _wtoi(qParams[0].c_str());

			_CheckActionParam(1, qParams, ValidateInfo);
			_CheckRange(L"Type",	nPartID, ValidateInfo, 0, MAX_BREAKABLE_PARTS);
		}break;
	case CAT_DAMAGE:	
		{
			const uint32 nDamageType = _wtoi(qParams[0].c_str());
			const uint32 nDamage	 = _wtoi(qParams[1].c_str());

			_CheckActionParam(2, qParams, ValidateInfo);
			_CheckRange(L"DamageType",	nDamageType, ValidateInfo, 0, DA_MAX);
			_CheckRange(L"Damage", nDamage, ValidateInfo, 0, 100000);
		}break;
	case CAT_MOTIONFACTOR:	
		{
			const MF_STATE nType = (MF_STATE)_wtoi(qParams[0].c_str());;
			const int nWeight = _wtoi(qParams[1].c_str());

			_CheckActionParam(2, qParams, ValidateInfo);
			_CheckRange(L"Type",	nType, ValidateInfo, 0, MF_PRIORITY_SIZE);
			_CheckRange(L"Weight", nWeight, ValidateInfo, 0, 1000);
		}break;
	case CAT_FLEE:	
		{
			wstring strFleeType = qParams[0];
			const float fFleeTime = (float)_wtof(qParams[1].c_str());
			vector<wstring> vecFleeTypes;
			vecFleeTypes.push_back(L"from_enemy");
			vecFleeTypes.push_back(L"to_home");
			_CheckStringTypeList(strFleeType, vecFleeTypes, L"Flee 타입", ValidateInfo);
			_CheckRange(L"FleeTime", (int)fFleeTime, ValidateInfo, 0, 10000);
		}break;
	case CAT_YELL:	
		{
			if (qParams[0].length() != 0)
			{
				const int nRange = _wtoi(qParams[0].c_str());
				_CheckRange(L"Range", nRange, ValidateInfo, 0, 100000);
			}
		}break;
	case CAT_GUARD:	
		{
			const int nGuardTalentID = _wtoi(qParams[0].c_str());
			const float fGuardTime = (float)_wtof(qParams[1].c_str());

			_CheckActionParam(2, qParams, ValidateInfo);
			_CheckRange(L"GuardTalentID",	nGuardTalentID, ValidateInfo, 0, INT_MAX);
			_CheckRange(L"GuardTime", (int)fGuardTime, ValidateInfo, 0, 100000);
			_CheckNPCTalent(nGuardTalentID, ValidateInfo);
			_CheckTalentType(nGuardTalentID, ST_GUARD, L"가드 탤런트", ValidateInfo);
		}break;
	case CAT_ADJUST:	
		{
			const wstring strAdjustType = qParams[0];
			const int nVar = _wtoi(qParams[1].c_str());
			_CheckActionParam(2, qParams, ValidateInfo);

			if (strAdjustType.length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);

			vector<wstring> vecAdjustType;
			vecAdjustType.push_back(L"guard_rate");
			vecAdjustType.push_back(L"victory");
			_CheckStringTypeList(strAdjustType, vecAdjustType, L"ADJUST 타입", ValidateInfo);
			_CheckRange(L"Variable", nVar, ValidateInfo, 0, 100000);
		}break;
	case CAT_DISTANCE:	
		{
			const int nMin = _wtoi(qParams[0].c_str());
			const int nMax = _wtoi(qParams[1].c_str());

			_CheckActionParam(2, qParams, ValidateInfo);
			_CheckRange(L"Min", nMin, ValidateInfo, 0, 100000);
			_CheckRange(L"Max", nMax, ValidateInfo, 0, 100000);
		}break;
	case CAT_FOLLOW:	
		{
			const int nDistance = _wtoi(qParams[0].c_str());

			_CheckActionParam(1, qParams, ValidateInfo);
			_CheckRange(L"Distance", nDistance, ValidateInfo, 0, 100000);
		}break;
	case CAT_MOVE:	
		{
			const int nMin = _wtoi(qParams[0].c_str());
			const int nMax = _wtoi(qParams[1].c_str());

			_CheckActionParam(2, qParams, ValidateInfo);
			_CheckRange(L"Min", nMin, ValidateInfo, 0, 100000);
			_CheckRange(L"Max", nMax, ValidateInfo, 0, 100000);
		}break;
	case CAT_CHANGE_HIT_CAPSULE:
		{
			const int nIndex = _wtoi(qParams[0].c_str());

			_CheckActionParam(1, qParams, ValidateInfo);
			_CheckRange(L"Index",	nIndex, ValidateInfo, 1, 100);
		}break;
	case CAT_SAY:
		{
			_CheckActionParam(1, qParams, ValidateInfo);
			
			if (qParams[0].length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);
		}break;
	case CAT_BALLOON:
		{
			_CheckActionParam(1, qParams, ValidateInfo);

			if (qParams[0].length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);
		}break;
	case CAT_SHOUT:
		{
			_CheckActionParam(1, qParams, ValidateInfo);

			if (qParams[0].length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);
		}break;
	case CAT_NARRATION:
		{
			_CheckActionParam(1, qParams, ValidateInfo);

			if (qParams[0].length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);
		}break;
	case CAT_AGGRO:
		{
			const wstring strAggroType = qParams[0];
			const float fKeepTime = (float)_wtof(qParams[1].c_str());

			_CheckActionParam(1, qParams, ValidateInfo);

			if (qParams[0].length() >= UINT_MAX)
				_ColtLog(ValidateInfo, L"문자열 내용이 너무 깁니다. (최대길이: %u)\n", UINT_MAX);

			vector<wstring> vecAggroType;
			vecAggroType.push_back(L"flip");
			vecAggroType.push_back(L"random");
			vecAggroType.push_back(L"near");
			vecAggroType.push_back(L"far");
			vecAggroType.push_back(L"short");
			_CheckStringTypeList(strAggroType, vecAggroType, L"AGGRO 타입", ValidateInfo);
			_CheckRange(L"KeepTime", (int)fKeepTime, ValidateInfo, 0, 100000);
		}break;
	case CAT_FACETO:
		{
		}break;
	default:
		_ColtLog(ValidateInfo, L"알 수 없는 콜트 액션입니다.");
	}

	ValidateInfo.qCallstack.pop_back();
	ValidateInfo.qCallstack.pop_back();

	return true;
}

bool GColtValidator::_CheckCondition(GColtCheck* pColtCheckInfo, GColtValidateInfo& ValidateInfo, bool bIsDefaultCheck  )
{
	if (!pColtCheckInfo)		return false;

	ValidateInfo.qCallstack.push_back(GStrings::ColtConditionStateToString(pColtCheckInfo->nType));

	const COLT_PARAM_DATA& qParams = pColtCheckInfo->vecParams;

	switch(pColtCheckInfo->nType)
	{
	case AST_HP:					
		{
			_CheckMaxArg(2, qParams, ValidateInfo);
			_CheckRange(L"LowHP",	qParams[0], ValidateInfo, 0, 100);
			_CheckRange(L"HighHP",	qParams[1], ValidateInfo, 0, 100);
			
		}break;
	case AST_ENEMY_HP:					
		{
			_CheckMaxArg(2, qParams, ValidateInfo);
			_CheckRange(L"LowHP",	qParams[0], ValidateInfo, 0, 100);
			_CheckRange(L"HighHP",	qParams[1], ValidateInfo, 0, 100);

		}break;
	case AST_UNBREAKPART:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"PartID",	qParams[0], ValidateInfo, 0, MAX_BREAKABLE_PARTS);

		}break;
	case AST_BREAKPART:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"PartID",	qParams[0], ValidateInfo, 0, MAX_BREAKABLE_PARTS);

		}break;
	case AST_DAMAGE:					
		{
			_CheckMaxArg(3, qParams, ValidateInfo);
			_CheckRange(L"DamageType",	qParams[0], ValidateInfo, 0, DA_MAX);
			_CheckRange(L"LowDamage",	qParams[1], ValidateInfo, 0, 100000);
			_CheckRange(L"HighDamage",	qParams[2], ValidateInfo, 0, 100000);
		}break;
	case AST_MOTIONFACTOR:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"MotionFactorType",	qParams[0], ValidateInfo, 0, MF_SIZE);
		}break;
	case AST_ENEMY_MOTIONFACTOR:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"MotionFactorType",	qParams[0], ValidateInfo, 0, MF_SIZE);
		}break;
	case AST_MODE:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"Mode",	qParams[0], ValidateInfo, 0, 100);
		}break;
	case AST_DISTANCE:					
		{
			_CheckMaxArg(2, qParams, ValidateInfo);
			_CheckRange(L"LowDistance",	qParams[0], ValidateInfo, 0, 100000);
			_CheckRange(L"HighDistance",	qParams[1], ValidateInfo, 0, 100000);
		}break;
	case AST_ANGLE:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"LowAngle",		qParams[0], ValidateInfo, -180, 180);
		}break;
	case AST_STRESS:					
		{
			_CheckMaxArg(2, qParams, ValidateInfo);
			_CheckRange(L"LowStress",	qParams[0], ValidateInfo, 0, 100000);
			_CheckRange(L"HighStress",	qParams[1], ValidateInfo, 0, 100000);
		}break;
	case AST_HIT_CAPSULE:					
		{
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckRange(L"Index",	qParams[0], ValidateInfo, 1, 100);
		}break;
	case AST_USED_TALENT:					
		{
			const int nTalentID = qParams[0].second;
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckNPCTalent(nTalentID, ValidateInfo);
		}break;
	case AST_GAINED_BUFF:
		{
			const int nBuffID = qParams[0].second;
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckBuff(nBuffID, ValidateInfo);
		}break;
	case AST_ENEMY_GAINED_BUFF:
		{
			const int nBuffID = qParams[0].second;
			_CheckMaxArg(1, qParams, ValidateInfo);
			_CheckBuff(nBuffID, ValidateInfo);
		}break;
	case AST_REAR:
		// do nothing
		break;
	case AST_FRONT:
		// do nothing
		break;
	case AST_NONE:
		// do nothing
		break;
	case AST_RAGE:
		_CheckMaxArg(2, qParams, ValidateInfo);
		_CheckRange(L"LowRage",	qParams[0], ValidateInfo, 0, 10000);
		_CheckRange(L"HighRage",	qParams[1], ValidateInfo, 0, 10000);
		break;
	default:
		_ColtLog(ValidateInfo, L"알 수 없는 콜트 체크문입니다.");
	}

	ValidateInfo.qCallstack.pop_back();

	return true;
}

bool GColtValidator::_CheckNPCTalent(int nTalentID, const GColtValidateInfo& ValidateInfo, bool bIgnoreDefault)
{
	if (bIgnoreDefault && 
		nTalentID == INVALID_TALENT_ID)	
	{
		return true;
	}

	GTalentInfoMgr* pTalentInfoMgr = gmgr.pTalentInfoMgr;
	if (!pTalentInfoMgr->Find(nTalentID))
	{
		_ColtLog(ValidateInfo, L"유효하지 않은 TalentID(%d).\n", nTalentID );
		return false;
	}

	if (!ValidateInfo.pNPCInfo->HasTalent(nTalentID))
	{
		_ColtLog(ValidateInfo, L"NPC가 보유하지 않은 TalentID(%d)입니다.\n", nTalentID );
		return false;
	}

	return true;
}

bool GColtValidator::_CheckBuff(int nBuffID, const GColtValidateInfo& ValidateInfo)
{
	if (!gmgr.pBuffInfoMgr->Get(nBuffID))
	{
		_ColtLog(ValidateInfo, L"유효하지 않은 BuffID(%d).\n", nBuffID );
		return false;
	}

	return true;
}

void GColtValidator::_MakeColtValidateInfo( GNPCInfo* pNPCInfo, GColtValidateInfo& outValidateInfo )
{
	if (!pNPCInfo)	return;

	wchar_t szNPC[256];
	swprintf_s(szNPC, L"%s(%d)", pNPCInfo->strName.c_str(), pNPCInfo->nID);
	outValidateInfo.pNPCInfo			= pNPCInfo;

	outValidateInfo.qCallstack.clear();
	outValidateInfo.qCallstack.push_back(szNPC);
	outValidateInfo.qCallstack.push_back(L"Colt");
}

bool GColtValidator::_CheckColtGroup(wstring strName, GNPCInfo* pNPCInfo, GColtCheck* stDefault, vector<GColtCheck*> vecChecks)
{
	GColtValidateInfo ValidateInfo;
	_MakeColtValidateInfo(pNPCInfo, ValidateInfo);

	ValidateInfo.qCallstack.push_back(strName);

	if (stDefault)
	{
		_CheckCheck(L"Default", stDefault, ValidateInfo, 0, true);
	}

	size_t size = vecChecks.size();
	for (int i =0; i < (int)size; i++)
	{
		_CheckCheck(L"Check"+ToString(i+1), vecChecks[i], ValidateInfo, 0, false);
	}

	ValidateInfo.qCallstack.pop_back();

	return true;
}

bool GColtValidator::_CheckCheck(wstring strName, GColtCheck* pColtCheckInfo, GColtValidateInfo& ValidateInfo, int nDepth, bool bIsDefaultCheck)
{
	if (!pColtCheckInfo)		return false;

	ValidateInfo.qCallstack.push_back(strName);

	_CheckCondition(pColtCheckInfo, ValidateInfo, bIsDefaultCheck);

	for (int i=0; i<(int)pColtCheckInfo->vecActions.size(); i++)
	{
		_CheckAction(L"Action"+ToString(i+1), pColtCheckInfo->vecActions[i], ValidateInfo);
	}

	for (int i=0; i<(int)pColtCheckInfo->vecAutoRunActions.size(); i++)
	{
		_CheckAction(L"Action_Autorun"+ToString(i+1), pColtCheckInfo->vecAutoRunActions[i], ValidateInfo);
	}

	if (nDepth == 0)
		_CheckCheck(L"Default", pColtCheckInfo->pCheckDefault, ValidateInfo, 1, true);

	for (int i=0; i<(int)pColtCheckInfo->vecChecks.size(); i++)
	{
		_CheckCheck(L"Check"+ToString(i+1), pColtCheckInfo->vecChecks[i], ValidateInfo, nDepth+1, false);
	}

	ValidateInfo.qCallstack.pop_back();

	return true;
}

void GColtValidator::_CheckActionParam(int nArgQty, const vector<wstring>& qParams, const GColtValidateInfo& ValidateInfo)
{
	if (nArgQty == 0)	return;

	bool bInvalid=false;
	if (nArgQty > 0)
	{
		for (int i=0; i<nArgQty; i++)
		{
			if (qParams[i] == L"")
				bInvalid = true;
		}
	}
	else
		bInvalid = true;

	if (bInvalid)
		_ColtLog(ValidateInfo, L"인자가 모자랍니다.\n");
}

void GColtValidator::_CheckRange(wstring strName, int nVal, const GColtValidateInfo& ValidateInfo, int nMin, int nMax)
{
	if (nVal < nMin || nVal > nMax)
		_ColtLog(ValidateInfo, L"%s(%d)가 허용 범위(%d ~ %d)에서 벗어났습니다.\n", strName.c_str(), nVal, nMin, nMax);
}

void GColtValidator::_CheckRange(wstring strName, pair<bool,int> Val, const GColtValidateInfo& ValidateInfo, int nMin, int nMax)
{
	if (!Val.first)		return;

	if (Val.second < nMin || Val.second > nMax)
		_ColtLog(ValidateInfo, L"%s(%d)가 허용 범위(%d ~ %d)에서 벗어났습니다.\n", strName.c_str(), Val.second, nMin, nMax);
}

bool GColtValidator::_CheckMaxArg(int nMaxArgQty, const COLT_PARAM_DATA& qParams, const GColtValidateInfo& ValidateInfo)
{
	if (nMaxArgQty == 0)	return true;
	
	if (nMaxArgQty > 0)
	{
		for (int i=0; i<nMaxArgQty; i++)
		{
			if (qParams[i].first)
				return true;
		}
	}

	_ColtLog(ValidateInfo, L"인자가 없습니다.\n");

	return false;
}

void GColtValidator::_CheckTalentType(int nTalentID, TALENT_SKILL_TYPE nTalentType, wstring strTalentTypeName, GColtValidateInfo& ValidateInfo )
{
	GTalentInfoMgr* pTalentInfoMgr = gmgr.pTalentInfoMgr;
	GTalentInfo* pTalentInfo = NULL;
	pTalentInfo = pTalentInfoMgr->Find(nTalentID);
	if (pTalentInfo && pTalentInfo->m_nSkillType != nTalentType)
	{
		_ColtLog(ValidateInfo, L"TalentID(%d)는 %s 탤런트이므로 %s로 사용될 수 없습니다.\n", 
			nTalentID, GStrings::TalentSkillTypeToString(pTalentInfo->m_nSkillType), strTalentTypeName.c_str());
	}
}

void GColtValidator::_CheckTalentTypeList(int nTalentID, vector<TALENT_SKILL_TYPE>& vecTalentType, wstring strTalentTypeName, GColtValidateInfo& ValidateInfo )
{
	GTalentInfoMgr* pTalentInfoMgr = gmgr.pTalentInfoMgr;
	GTalentInfo* pTalentInfo = NULL;
	pTalentInfo = pTalentInfoMgr->Find(nTalentID);

	bool bValid = false;
	for (size_t i=0; i<vecTalentType.size(); i++)
	{
		if (pTalentInfo && pTalentInfo->m_nSkillType == vecTalentType[i])
		{
			bValid = true;
			return;
		}
	}

	if (!bValid)
	{
		TALENT_SKILL_TYPE nSkillType = ST_NONE;
		if (pTalentInfo)		nSkillType = pTalentInfo->m_nSkillType;

		_ColtLog(ValidateInfo, L"TalentID(%d)는 %s 탤런트이므로 %s로 사용될 수 없습니다.\n", 
			nTalentID, GStrings::TalentSkillTypeToString(nSkillType), strTalentTypeName.c_str());
	}
}

void GColtValidator::_CheckStringTypeList(wstring strToTestType, vector<wstring>& vecStringType, wstring strTypeName, GColtValidateInfo& ValidateInfo)
{
	bool bValid = false;
	for (size_t i=0; i<vecStringType.size(); i++)
	{
		if (strToTestType == vecStringType[i])
		{
			bValid = true;
			return;
		}
	}

	if (!bValid)
	{
		_ColtLog(ValidateInfo, L"\'%s\'은(는) \'%s\'이(가) 아닙니다.\n", 
			strToTestType.c_str(), strTypeName.c_str());
	}
}

void GColtValidator::_CheckLuaFunction(wstring strFirst, wstring strSecond, GColtValidateInfo& ValidateInfo)
{
	if (strSecond.empty())
	{
		if (WLUA->IsExistGlobalFunc(MLocale::ConvUTF16ToAnsi(strFirst.c_str()).c_str()) == false)
		{
			_ColtLog(ValidateInfo, L"정의되지 않은 LUA 함수 입니다. (%s)\n", strFirst.c_str());
		}
	}
	else
	{
		if (WLUA->IsExistMemberFunc(MLocale::ConvUTF16ToAnsi(strFirst.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(strSecond.c_str()).c_str()) == false)
		{
			_ColtLog(ValidateInfo, L"정의되지 않은 LUA 함수 입니다. (%s:%s)\n", strFirst.c_str(), strSecond.c_str());
		}
	}
}

void GColtValidator::_CheckColtAttribute( GNPCInfo* pNPCInfo)
{
	GColtValidateInfo ValidateInfo;
	_MakeColtValidateInfo(pNPCInfo, ValidateInfo);

	const GColtGroupInfo& ColtInfo = pNPCInfo->ColtInfo;

	for (int i=0; i<NPC_MODE_MAX; ++i)
	{
		const GColtGroupInfo::CombatAttr& combat_attr = ColtInfo.attrCombat[i];
		int nDownAttackTalentID = combat_attr.nDownAttackTalentID;
		int nVictoryTalentID = combat_attr.nVictoryTalentID;

		_CheckNPCTalent(nDownAttackTalentID, ValidateInfo, true);
		_CheckNPCTalent(nVictoryTalentID, ValidateInfo, true);
	}
}



void GColtValidator::_ColtLog( const GColtValidateInfo& ValidateInfo, const wchar_t * szFormat, ... )
{
	SetFailed();

	wchar_t szBuff[1024];

	va_list args;
	va_start(args,szFormat);
	vswprintf_s(szBuff,szFormat,args);
	va_end(args);

	wstring strLog;

	for (size_t i=0; i<ValidateInfo.qCallstack.size(); i++)
	{
		wstring strCall = ValidateInfo.qCallstack[i];

		strLog += strCall;
		if (i!=ValidateInfo.qCallstack.size()-1)
			strLog += L".";
	}

	strLog += L": ";
	strLog += szBuff;

	 _VLOGGER->Log(strLog.c_str());
}

