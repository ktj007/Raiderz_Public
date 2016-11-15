#include "StdAfx.h"
#include "ZFitnessCalculator.h"
#include "ZGameServerObject.h"
#include "ZConst.h"


int ZFitnessCalculator::CalcToCreateChannel(ZGameServerObject* pTargetServer, int nAdditionalValue)
{
	if (NULL == pTargetServer)
		return 0;

	if (0 >= ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT)
		return MAX_FITNESS_VALUE_TO_ALLOC_FIELD;

	int nCurPlayerCount = pTargetServer->GetPlayerMap().GetObjectCount();
	int nCurSharedFieldCount = pTargetServer->GetFieldMap().GetSharedFieldCount();
	int nAllSharedFieldCount = gmgr.pFieldManager->GetSharedFieldCount();

	if (0 == nAllSharedFieldCount)
		return 0;

	if (ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT <= nCurPlayerCount)
		return 0;

	float fTempPlayerCountValue	= 0.7f * (1.0f - CalcRate(nCurPlayerCount, ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT));
	float fTempFieldCountValue	= 0.3f * (1.0f - CalcRate(nCurSharedFieldCount, nAllSharedFieldCount));

	float fFitnessMod = fTempPlayerCountValue + fTempFieldCountValue;

	return CalcFitnessValue(fFitnessMod, nAdditionalValue);
}

int ZFitnessCalculator::CalcToCreateFieldGroup(ZGameServerObject* pTargetServer, int nAdditionalValue)
{
	if (NULL == pTargetServer)
		return 0;

	if (0 >= ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT)
		return MAX_FITNESS_VALUE_TO_ALLOC_FIELD;

	if (0 >= ZConst::GAME_SERVER_DYNAMIC_FIELD_COUNT_LIMIT)
		return MAX_FITNESS_VALUE_TO_ALLOC_FIELD;

	int nCurPlayerCount = pTargetServer->GetPlayerMap().GetObjectCount();
	int nCurFieldGroupCount = pTargetServer->GetFieldMap().GetDynamicFieldMasterCount();

	if (ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT <= nCurPlayerCount)
		return 0;

	if (ZConst::GAME_SERVER_DYNAMIC_FIELD_COUNT_LIMIT <= nCurFieldGroupCount)
		return 0;


	float fTempPlayerRate	= 0.7f * (1.0f - CalcRate(nCurPlayerCount, ZConst::GAME_SERVER_PLAYER_COUNT_LIMIT));
	float fTempFieldRate	= 0.3f * (1.0f - CalcRate(nCurFieldGroupCount, ZConst::GAME_SERVER_DYNAMIC_FIELD_COUNT_LIMIT));

	float fFitnessMod = fTempPlayerRate + fTempFieldRate;

	return CalcFitnessValue(fFitnessMod, nAdditionalValue);
}

float ZFitnessCalculator::CalcRate(int nCurValue, int nMaxValue)
{
	if (0 == nMaxValue) return .0f;
	return ((float)nCurValue / (float)nMaxValue);
}

int ZFitnessCalculator::CalcFitnessValue(float fModifier, int nAdditionalValue)
{
	int nFitnessValue = (int)((float)MAX_FITNESS_VALUE_TO_ALLOC_FIELD * fModifier + (float)nAdditionalValue);

	if (nFitnessValue > MAX_FITNESS_VALUE_TO_ALLOC_FIELD)
		nFitnessValue = MAX_FITNESS_VALUE_TO_ALLOC_FIELD;

	return nFitnessValue;
}