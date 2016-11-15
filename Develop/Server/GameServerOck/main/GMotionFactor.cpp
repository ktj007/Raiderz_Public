#include "stdafx.h"
#include "GMotionFactor.h"
#include "GEntityActor.h"
#include "GMath.h"
#include "GField.h"
#include "CSMotionFactorHelper.h"

#define MOTION_FACTOR_DECREASE_TICK		0.5f	///< 모션 팩터 틱(0.5초)
#define MOTION_FACTOR_DECREASE_AMOUNT	10		///< 모션팩터 틱당 weight 감소량

GMotionFactor::GMotionFactor()
: m_nCurrentMF(MF_NONE), m_nCurrentWeight(0), m_bActive(true)
{
	ClearVars();

	m_DecreaseRegulator.SetTickTime(MOTION_FACTOR_DECREASE_TICK);
}

GMotionFactor::~GMotionFactor(void)
{

}

void GMotionFactor::Update(float fDelta)
{
	// 모션 팩터값 틱당 감소
	if (m_bActive &&
		m_DecreaseRegulator.IsReady(fDelta))
	{
		DecreaseVars();
	}
}


void GMotionFactor::ClearVars()
{
	for (int i = 0; i < MF_VALUABLE_SIZE; i++)
	{
		m_Factors.var[i].nValue = 0;
		m_Factors.var[i].nWeight = 0;
	}
}

void GMotionFactor::DecreaseVars()
{
	for (int i = 0; i < MF_VALUABLE_SIZE; i++)
	{
		// TODO: 원랜 캐릭터마다 MOD가 다르다.
		int nMFModifier = MOTION_FACTOR_DECREASE_AMOUNT;	

		m_Factors.var[i].nValue -= nMFModifier;
		if (m_Factors.var[i].nValue <= 0)
		{
			m_Factors.var[i].nValue = 0;
			m_Factors.var[i].nWeight = 0;
		}
	}
}

bool GMotionFactor::CheckActivateMF(MF_STATE& outState, int& outWeight, bool bIsSticky)
{
	const int MAX_MOTION_FACTOR_PICK_VALUE = 100;

	bool bExist = false;

	for (int i = 0; i < MF_VALUABLE_SIZE; i++)
	{
		if (m_Factors.var[i].nValue >= MAX_MOTION_FACTOR_PICK_VALUE)
		{
			if (bIsSticky)
			{
				switch (MF_STATE(i))
				{
					case MF_KNOCKBACK:
					case MF_THROWUP:
					case MF_UPPERED:
					case MF_DRAG:
					case MF_FAKE_KNOCKBACK:
						continue; // 움직일 수 없는 상태에서는 걸릴 수 없는 모션팩터
				}
			}

			if (CSMotionFactorHelper::GetMFPriority(MF_STATE(i)) <= CSMotionFactorHelper::GetMFPriority(outState))
			{
				outState = MF_STATE(i);
				outWeight = m_Factors.var[i].nWeight;
				bExist = true;
			}
		}
	}

	// 모션팩터의 중첩일 경우 적용되는 것이 각각 다르다.
	if (bExist)
	{
		if (CheckOverridable(m_nCurrentMF, outState) == false)
		{
			bExist = false;
		}
	}

	return bExist;
}

bool GMotionFactor::CheckFakeMF(MF_STATE& outState, int& outWeight, PerMulVar<short>* pValueModifiers, bool bIsSticky)
{
	if (m_Factors.var[MF_FAKE_BEATEN].nWeight > 0)
	{
		outState = MF_FAKE_BEATEN;
		outWeight = m_Factors.var[MF_FAKE_BEATEN].nWeight;
		return true;
	}
	else if (m_Factors.var[MF_FAKE_KNOCKBACK].nWeight > 0)
	{
		// npc의 Knockback value Modifier가 0에 가까우면 FAKE_KNOCKBACK는 안한다.
		if (pValueModifiers && pValueModifiers[MF_FAKE_KNOCKBACK].fPer <= 0.2f)
		{
			return false;
		}

		outState = MF_FAKE_KNOCKBACK;
		outWeight = m_Factors.var[MF_FAKE_KNOCKBACK].nWeight;
		return true;
	}

	return false;
}

bool GMotionFactor::Apply( MOTION_FACTOR_GROUP& MotionFactorGroup, PerMulVar<short>* pValueModifiers, PerMulVar<short>* pWeightModifiers, int nNotApplyMF/*=MF_NONE */, vector<int>* pvecAdditionIgnoreMF/*=NULL */, bool bIsSticky/*=false*/ )
{
	// Increase Vars 
	IncreaseVars(MotionFactorGroup, pValueModifiers, pWeightModifiers, nNotApplyMF, pvecAdditionIgnoreMF);

	MF_STATE noutMF = MF_BEATEN;
	int noutWeight;

	if (CheckActivateMF(noutMF, noutWeight, bIsSticky))
	{
		m_nCurrentMF = noutMF;
		m_nCurrentWeight = noutWeight;
		m_Factors.var[noutMF].nValue = 0;
		m_Factors.var[noutMF].nWeight = 0;
		return true;
	}

	if (CheckFakeMF(noutMF, noutWeight, pValueModifiers, bIsSticky))
	{
		m_nCurrentMF = noutMF;
		m_nCurrentWeight = noutWeight;
		return true;
	}

	return false;
}


wstring GMotionFactor::GetDebugString()
{
	wstring strRet;

	const wchar_t* mf_name[MF_SIZE] =
	{
		L"bt",
		L"kb",
		L"st",
		L"df",		
		L"tu",
		L"kd",
		L"up",
		L"fbt",
		L"fkb",
		L"gp",
		L"et",
		L"pl"
	};

	for (int i = MF_BEATEN; i<MF_SIZE; i++)
	{
		wchar_t text[128];
		swprintf_s(text, L"%s(%3d:%3d)", mf_name[i], m_Factors.var[i].nValue, m_Factors.var[i].nValue);
		strRet += wstring(text);
	}

	return strRet;
}

void GMotionFactor::OnReleased()
{
	if (m_nCurrentMF == MF_THROWUP ||
		m_nCurrentMF == MF_KNOCKDOWN ||
		m_nCurrentMF == MF_KNOCKBACK ||
		m_nCurrentMF == MF_STUN ||
		m_nCurrentMF == MF_UPPERED ||
		m_nCurrentMF == MF_DRAG)
	{
		ClearVars();
	}

	m_nCurrentMF = MF_NONE;
	m_nCurrentWeight = 0;
}

void GMotionFactor::Force( MF_STATE nState, int nWeight )
{
	m_nCurrentMF = nState;
	m_nCurrentWeight = nWeight;
}

bool GMotionFactor::CheckOverridable( MF_STATE nCurrMF, MF_STATE nNewMF )
{
	switch (nCurrMF)
	{
	case MF_THROWUP:
	case MF_KNOCKDOWN:
		{
			return false;
		}
		break;
	case MF_KNOCKBACK:
	case MF_STUN:
	case MF_UPPERED:
		{
			if (CSMotionFactorHelper::GetMFPriority(nCurrMF) <= CSMotionFactorHelper::GetMFPriority(nNewMF))
			{
				return false;
			}
		}
		break;
	case MF_DEFLECT:	
	case MF_BEATEN:
		{
			if (CSMotionFactorHelper::GetMFPriority(nCurrMF) <= CSMotionFactorHelper::GetMFPriority(nNewMF) && nNewMF != MF_BEATEN)
			{
				return false;
			}
		}
		break;
	}

	return true;
}

void GMotionFactor::IncreaseVars( MOTION_FACTOR_GROUP &MotionFactorGroup, PerMulVar<short>* pValueModifiers, PerMulVar<short>* pWeightModifiers, int nNotApplyMF, vector<int>* pvecAdditionIgnoreMF )
{
	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		if (nNotApplyMF == i) continue;
		if (pvecAdditionIgnoreMF)
		{
			bool bSkip = false;
			for each (int nAdditionIgnoreMF in *pvecAdditionIgnoreMF)
			{
				if (nAdditionIgnoreMF == i)
				{
					bSkip = true;
					break;
				}
			}

			if (bSkip)
				continue;
		}

		m_Factors.var[i].nValue += (pValueModifiers[i].Calc(MotionFactorGroup.var[i].nValue));

		short in_weight = pWeightModifiers[i].Calc(MotionFactorGroup.var[i].nWeight);
		short weight_avg = (m_Factors.var[i].nWeight + in_weight) / 2;
		m_Factors.var[i].nWeight = max(weight_avg, in_weight);
	}
}

void GMotionFactor::ReleaseFakes()
{
	m_nCurrentMF = MF_NONE;
	m_nCurrentWeight = 0;
}