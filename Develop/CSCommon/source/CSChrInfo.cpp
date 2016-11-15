#include "stdafx.h"
#include "CSChrInfo.h"
#include "CSDef_Fatigue.h"

CSNPCInfoMgr::CSNPCInfoMgr()
{



}

CSNPCInfoMgr::~CSNPCInfoMgr()
{

}


bool CSAltitudeHelper::IsAreaHittable( ALTITUDE_TYPE nAltitude )
{
	// 근접 공격시 공중,물속,땅속은 판정 불가
	if (nAltitude == AT_SKY || nAltitude == AT_UNDERGROUND) return false;
	return true;
}

bool CSAltitudeHelper::IsApplicableToMotionFactor( ALTITUDE_TYPE nAltitude )
{
	if (nAltitude == AT_SKY) return false;
	return true;
}

bool CSAltitudeHelper::IsColliable( ALTITUDE_TYPE nAltitude )
{
	if (nAltitude == AT_SKY ||
		nAltitude == AT_UNDERGROUND)
	{
		return false;
	}
	return true;
}

CHR_INFO::CHR_INFO() : bRooted(false)
, nLevel(1)
, nMaxHP(PLAYER_DEFAULT_HPMAX)
, nMaxEN(PLAYER_DEFAULT_ENMAX)
, nMaxSTA(PLAYER_DEFAULT_SPMAX)
, nAP(0)
, nABS(0)
, nHPRegen(5)
, nENRegen(5)
, nSTARegen(5)
, fSpeed(PLAYER_DEFAULT_SPEED)
, nAltitude(AT_NORMAL)
{

}

CSNPCInfo::CSNPCInfo()
: nID(-1)
, nGrade(DEFAULT_NPC_GRADE)
, bHuge(false)
, fRunSpeed(0.0f)
, fFleeSpeedAmp(1.0f)
, fRotateSpeed(0.0f)
, fScale(1.f)
, nMode(NPC_MODE_DEFAULT)
, nSpawningMode(NPC_MODE_DEFAULT)
, nHitTestIndex(0)
, nMaxLevel(nLevel)
, bShowName(true)
, bInteract(true)
, nNpcType(NPC_TYPE_MONSTER)
, nArmorType(ARMOR_ATT_NEUTRAL)
, nDefaultGuardType(GT_PERFECT)
, fTitleHeight(0.0f)
, nCHA(0)
, fInteractionLootTime(0.0f)
, m_bColTestObject(true)
, bValidation(true)
, nEatOwnerBuffID(INVALID_BUFF_ID)
, nEatTargetBuffID(INVALID_BUFF_ID)
, fEatPeriod(0.0f)
, m_nFactionLossID(0)
, nInteractionDistance(0)
, nGatherType(GATHER_NONE)
, bShowEnemyInfo(true)
{
	Clear();
}

void CSNPCInfo::Clear()
{
	strName = _T("");
	strNameInLua = _T("");
	strSubtitle = _T("");
	strTitleBone = _T("");

	SoundInfo.Clear();
}

PLAYER_INFO::PLAYER_INFO() : nCID(0)
, nRace(RACE_NA)
, nSex(SEX_NA)
, nFeatureHair(0)
, nFeatureFace(0)
, nFeatureHairColor(0)
, nFeatureSkinColor(0)
, nMakeUp(0)
, nTattooType(0)
, nTattooPosX(0)
, nTattooPosY(0)
, nTattooScale(0)
, nXP(0)
, nSoulBindingID(0)
, nSTR(PLAYER_DEFAULT_STR)
, nDEX(PLAYER_DEFAULT_DEX)
, nINT(PLAYER_DEFAULT_INT)
, nCHA(PLAYER_DEFAULT_CHA)
, nCON(PLAYER_DEFAULT_CON)
, nGrab(0)
, nBodyRun(0)
, fMerchandize(0.0f)
, nFatigueQuantity(NORMAL_FATIGUE)
, m_bGod(false)
, nInnRoomID(0)
, nGID (0)
, nPlayTimeSec(0)
, nConnPlayTimeSec(0)
, nDeadTime(0)
, nElapsedDeadTimeSec(0)
, nPlayerGrade(PLAYER_GRADE_NORMAL)
, m_nMoney(0)
, m_bGhost(false)
, nLootGainMoney(0)
{
	szName[0] = 0;
	szSurname[0] = 0;
}

void PLAYER_INFO::SetMoney(int nMoney)
{
	if (MAX_MONEY < nMoney)
	{
		m_nMoney = MAX_MONEY;
	}
	else if (0 > nMoney)
	{
		m_nMoney = 0;
	}
	else
	{
		m_nMoney = nMoney;
	}	
}

int PLAYER_INFO::GetMoney()
{
	return m_nMoney;
}

void PLAYER_INFO::SetStorageMoney( int nMoney )
{
	if (MAX_MONEY < nMoney)
	{
		m_nStorageMoney = MAX_MONEY;
	}
	else if (0 > nMoney)
	{
		m_nStorageMoney = 0;
	}
	else
	{
		m_nStorageMoney = nMoney;
	}	
}

int PLAYER_INFO::GetStorageMoney()
{
	return m_nStorageMoney;
}

int PLAYER_INFO::GetDeltaPlayTime()
{
	return nPlayTimeSec - nConnPlayTimeSec;
}

float PLAYER_INFO::CalcAutoRebirthRemainTime()
{
	float fAutoRebirthRemainTime = max(0.0f, GAME_AUTO_REBIRTH_CHECK_TICK - static_cast<float>(this->nElapsedDeadTimeSec));
	return fAutoRebirthRemainTime;
}

CHR_STATUS::CHR_STATUS() : nHP(0)
, nEN(0)
, nSTA(0)
{
	;
}

CSActorResistances::CSActorResistances()
{
	nValue[TRT_FR] = PLAYER_DEFAULT_FR;
	nValue[TRT_CR] = PLAYER_DEFAULT_CR;
	nValue[TRT_PR] = PLAYER_DEFAULT_PR;
	nValue[TRT_LR] = PLAYER_DEFAULT_LR;
	nValue[TRT_HR] = PLAYER_DEFAULT_HR;
	nValue[TRT_UR] = PLAYER_DEFAULT_UR;
}

void NPCSoundInfo::Clear()
{
	HitSound1.clear();
	HitSound2.clear();
	HitSound3.clear();
	DyingSound.clear();
	EntitySound.clear();
	CombatBGM.clear();
	Aggro.clear();
}