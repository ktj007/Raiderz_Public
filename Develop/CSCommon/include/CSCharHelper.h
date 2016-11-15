#pragma once

#include "CSCommonLib.h"
#include "CSChrInfo.h"

/// 캐릭터 관련 헬퍼 클래스
class CSCOMMON_API CSCharHelper
{
public:
	static bool IsDeveloperGrade(GPlayerGrade nPlayerGrade)
	{
		if (nPlayerGrade == PLAYER_GRADE_TESTER ||
			nPlayerGrade == PLAYER_GRADE_DEVELOPER)
		{
			return true;
		}
		return false;
	}

	static bool IsGMGrade(GPlayerGrade nPlayerGrade)
	{
		if (PLAYER_GRADE_NORMAL < nPlayerGrade)
		{
			return true;
		}

		return false;
	}

	/// 인터랙션 가능한 거리 계산. nInteractionDistance가 0이면 Radius로 계산에 의해 구해지고, 0보다 크면 nInteractionDistance값이 가능한 거리
	static float CalcValidInteractionDistance(float fTarColRadius, int nInteractionDistance=0);
};