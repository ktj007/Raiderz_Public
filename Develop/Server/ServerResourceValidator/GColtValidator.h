#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"
#include "GSystem.h"
#include "GServerSystems.h"
#include "GColtInfo.h"
#include "CSTalentInfo.h"

struct GColtCheck;
struct GColtAction;
struct GColtValidateInfo;
class GNPCInfo;


class GVALIDATOR_API GColtValidator : public CSBaseValidator
{
private:
	bool _CheckColtGroup(wstring strName, GNPCInfo* pNPCInfo, GColtCheck* stDefault, vector<GColtCheck*> vecChecks);
	bool _CheckCheck(wstring strName, GColtCheck* pColtCheckInfo, GColtValidateInfo& ValidateInfo, int nDepth, bool bIsDefaultCheck);
	bool _CheckAction(wstring strName, const GColtAction& ActionInfo, GColtValidateInfo& ValidateInfo);
	bool _CheckCondition(GColtCheck* pColtCheckInfo, GColtValidateInfo& ValidateInfo, bool bIsDefaultCheck );
	bool _CheckMaxArg(int nMinArgQty, const COLT_PARAM_DATA& qParams, const GColtValidateInfo& ValidateInfo);
	void _CheckRange(wstring strName, pair<bool,int> Val, const GColtValidateInfo& ValidateInfo, int nMin, int nMax);
	void _CheckRange(wstring strName, int nVal, const GColtValidateInfo& ValidateInfo, int nMin, int nMax);
	void _CheckActionParam(int nArgQty, const vector<wstring>& qParams, const GColtValidateInfo& ValidateInfo);
	void _MakeColtValidateInfo(GNPCInfo* pNPCInfo, GColtValidateInfo& outValidateInfo);
	bool _CheckNPCTalent(int nTalentID, const GColtValidateInfo& ValidateInfo, bool bIgnoreDefault=false);
	bool _CheckBuff(int nBuffID, const GColtValidateInfo& ValidateInfo);
	void _CheckColtAttribute( GNPCInfo* pNPCInfo );
	void _CheckTalentType(int nTalentID, TALENT_SKILL_TYPE nTalentType, wstring strTalentTypeName, GColtValidateInfo& ValidateInfo);
	void _CheckTalentTypeList(int nTalentID, vector<TALENT_SKILL_TYPE>& vecTalentType, wstring strTalentTypeName, GColtValidateInfo& ValidateInfo);
	void _CheckStringTypeList(wstring strType, vector<wstring>& vecStringType, wstring strTypeName, GColtValidateInfo& ValidateInfo);
	void _CheckLuaFunction(wstring strFirst, wstring strSecond, GColtValidateInfo& ValidateInfo);
	// Log
	void _ColtLog(const GColtValidateInfo& ValidateInfo, const wchar_t * szFormat, ...);
public:
	virtual bool Check();
	
	
};
