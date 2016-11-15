#pragma once

struct GColtAction;
class GColtActionInst;
typedef vector<pair<const GColtAction*, GColtActionInst*> >			RUNNING_ACTION_LIST;

#define		MAX_AI_RATE	100


enum ColtType
{
	COLT_INVALID = -1,
	COLT_COMBAT = 0,
	COLT_AGGRO,
	COLT_IDLE,
	COLT_REACTION,
	COLT_STRESS,
	COLT_PROLOGUE,
	COLT_EPILOGUE,

	COLT_MAX,
};

static const wchar_t* s_pszColtTypeName[]  = 
{
	L"Combat",
	L"Aggro",
	L"Idle",
	L"Reaction",
	L"Stress",
	L"Prologue",
	L"Epilogue",
};

static const wchar_t* ConvrtColtTypeString( ColtType nType )
{
	if (nType <= COLT_INVALID)		
		return L"invalid_colt";
	if (nType >= COLT_MAX)		
		return L"invalid_colt";

	return s_pszColtTypeName[nType];
}
