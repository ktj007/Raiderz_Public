#include "stdafx.h"
#include "TTalentActAnimationTime.h"
#include "MXml.h"
#include "TAnimationResourceController.h"

TTalentActAnimationTime::TTalentActAnimationTime()
{
	m_pAniResourceController = new TAnimationResourceController(&(REngine::GetSceneManager()));
}

TTalentActAnimationTime::~TTalentActAnimationTime()
{
	SAFE_DELETE(m_pAniResourceController);
}

bool TTalentActAnimationTime::CalTalentActAnimationTime( CSTalentInfo* pTalentInfo )
{
	if(CalTalentUseAnimationTime(pTalentInfo) |
		CalTalentExtraAnimationTime(pTalentInfo, TAT_EXTRA) |
		CalTalentExtraAnimationTime(pTalentInfo, TAT_EXTRA2) |
		CalTalentExtraAnimationTime(pTalentInfo, TAT_EXTRA3))
		return true;

	return false;
}

void TTalentActAnimationTime::MakeTalentActAnimationTime( TALENT_ANIMATION_INFO& talentAniInfo, vector<CSTalentActAnimationTimeInfo>& vecActAniTimeInfo, TALENT_ANIMATION_TYPE eType /*= TAT_USE*/ )
{
	vecActAniTimeInfo.clear();

	for(vector<WEAPON_ANIMATION_INFO>::iterator itActAniInfo = talentAniInfo.vecTalentAniInfo.begin(); itActAniInfo != talentAniInfo.vecTalentAniInfo.end(); itActAniInfo++)
	{
		CSTalentActAnimationTimeInfo newActAniTimeInfo;
		newActAniTimeInfo.nWeaponType = itActAniInfo->eWeaponType;

		float fTime = 0.0f;
		for(vector<ANIMATION_INFO>::iterator itActAniList = itActAniInfo->vecAniInfo.begin(); itActAniList != itActAniInfo->vecAniInfo.end(); itActAniList++)
		{
			// Extra, Extra2는 loop형 애니메이션인경우는 시간에 포함을 하지 않느다.(먹기때문에...)
			if((eType != TAT_EXTRA && eType != TAT_EXTRA2) ||
				itActAniList->loopType != RAniLoopType_Loop)
			{
				fTime += GetAnimationTime(*itActAniList);
			}
		}

		if(fTime > 0.0f)
		{
			newActAniTimeInfo.fAniTime = fTime;

			vecActAniTimeInfo.push_back(newActAniTimeInfo);
		}
	}
}

float TTalentActAnimationTime::GetAnimationTime( ANIMATION_INFO& aniInfo )
{
	// 애니메이션 정보 가져오기
	int nMaxFrame = m_pAniResourceController->GetAnimationMaxFrame(aniInfo.strAnimationPathName + aniInfo.strAnimationFileName);

	// 시간 가져오기
	return GetAnimationRealTime(nMaxFrame, aniInfo.fAnimationSpeed);
}

float TTalentActAnimationTime::GetAnimationRealTime( int nMaxFrame, float fAnimationSpeed )
{
	return (nMaxFrame / 4.8f / (fAnimationSpeed * 1000.f));
}

bool TTalentActAnimationTime::CheckDifferentData( CSTalentInfo* pTalentInfo, vector<CSTalentActAnimationTimeInfo>& vecActAniTimeInfo )
{
	if(pTalentInfo->m_ActAnimationTime.vecInfo.size() != vecActAniTimeInfo.size())	// 추가 및 삭제가 되었다면...
		return true;

	for(vector<CSTalentActAnimationTimeInfo>::iterator itActAniTimeInfo = vecActAniTimeInfo.begin(); itActAniTimeInfo != vecActAniTimeInfo.end(); itActAniTimeInfo++)
	{
		if(MMath::Equals(pTalentInfo->m_ActAnimationTime.GetTime(itActAniTimeInfo->nWeaponType), itActAniTimeInfo->fAniTime, 0.000001f) == false)
			return true;
	}

	return false;
}

bool TTalentActAnimationTime::CalTalentUseAnimationTime( CSTalentInfo* pTalentInfo )
{
	TALENT_ANIMATION_INFO talentAniInfo;
	TAnimationHelper talentAnimationHelper;
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, TAT_USE);

	vector<CSTalentActAnimationTimeInfo> vecInfo;
	MakeTalentActAnimationTime(talentAniInfo, vecInfo);

	if(CheckDifferentData(pTalentInfo, vecInfo))
	{
		// 다르다. 다시 셋팅
		pTalentInfo->m_ActAnimationTime.vecInfo.clear();
		pTalentInfo->m_ActAnimationTime.vecInfo = vecInfo;

		return true;
	}

	return false;
}

bool TTalentActAnimationTime::CalTalentExtraAnimationTime( CSTalentInfo* pTalentInfo, TALENT_ANIMATION_TYPE eType )
{
	// Extra Animation Time 추출은 NPC만 해두자
	// 나중에 Player도 하게 되면 그때 하자!!!
	if(pTalentInfo->m_setNPCID.size() == 0)
	{
		return false;
	}

	float * pExtraTime = &(pTalentInfo->m_fExtraMotionTime);
	if(eType == TAT_EXTRA2)
		pExtraTime = &(pTalentInfo->m_fExtraMotionTime2);
	else if(eType == TAT_EXTRA3)
		pExtraTime = &(pTalentInfo->m_fExtraMotionTime3);

	TALENT_ANIMATION_INFO talentAniInfo;
	TAnimationHelper talentAnimationHelper;
	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, eType);

	vector<CSTalentActAnimationTimeInfo> vecInfo;
	MakeTalentActAnimationTime(talentAniInfo, vecInfo, eType);

	// NPC 이므로 분명히 하나 일것이다.
	if(vecInfo.empty() == false)
	{
		if(MMath::Equals(*pExtraTime, vecInfo[0].fAniTime, 0.000001f) == false)
		{
			*pExtraTime = vecInfo[0].fAniTime;
			return true;
		}
	}
	else if(*pExtraTime > 0.0f)
	{
		*pExtraTime = 0.0f;
		return true;
	}

	return false;
}
