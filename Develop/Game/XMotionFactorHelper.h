#pragma once

class XMotionFactorHelper
{
public:
	static float CalcKnockbackTime(float fMFWeight)
	{
		fMFWeight = max(0.0f, fMFWeight);
		return (fMFWeight / 100.0f) * KNOCKBACK_TIME;
	}

	static float CalcThrowupTime(float fMFWeight)
	{
		fMFWeight = max(0.0f, fMFWeight);
		return (fMFWeight / 100.0f) * KNOCKBACK_TIME;	// ThrowUp도 Knockback과 동일
	}

	static float CalcThrowupMotionSpeed(float fMFWeight, float fMotionEventDistance, float fTU1AnimationFullTime)
	{
		float fDistance = max(10.0f, (fMFWeight - fMotionEventDistance));
		float fTime = (fDistance / 100.0f) * KNOCKBACK_TIME;

		float fRetSpeed = (fTU1AnimationFullTime / fTime);
		return fRetSpeed;
	}

	static float CalcUpperedTime(float fMFWeight)
	{
		fMFWeight = max(0.0f, fMFWeight);
		return (fMFWeight / 100.0f) * KNOCKBACK_TIME;
	}

	struct UPPERED_FACTOR
	{
		float fTopHeight;
		float fUpVelocity;
		float fDownTime;
	};

	static UPPERED_FACTOR CalcUpperedFactor(float fMFWeight)
	{
		UPPERED_FACTOR ret;

		if (fMFWeight < 1000)
		{
			ret.fTopHeight = fMFWeight * 0.5f;
		}
		else if (fMFWeight < 1500)
		{
			ret.fTopHeight = fMFWeight * 0.55f;
		}
		else if (fMFWeight < 2000)
		{
			ret.fTopHeight = fMFWeight * 0.65f;
		}
		else
		{
			ret.fTopHeight = fMFWeight * 0.7f;
		}

		float fTotalTime = CalcUpperedTime(fMFWeight);
		float fDownTime = min(fTotalTime*0.6f , ret.fTopHeight / abs(GRAVITY));
		fDownTime -= min(fDownTime * 0.5f, (fTotalTime * 0.25f));


		ret.fDownTime = fDownTime;

		float fWidth = fDownTime / KNOCKBACK_TIME * 100.0f;
		fWidth = max(1.0f, fWidth);

		float fTime = (fWidth / 100.0f) * KNOCKBACK_TIME;

		ret.fUpVelocity = (ret.fTopHeight / fTime);

		return ret;
	}
};