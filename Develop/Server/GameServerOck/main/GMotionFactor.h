#ifndef _GMOTION_FACTOR_H
#define _GMOTION_FACTOR_H

#include "MTime.h"
#include "CSMotionFactorInfo.h"
#include "GDef.h"
#include "CSModifier.h"

class GField;
class GEntityActor;


/// 모션 팩터
class GMotionFactor
{
protected:
	MRegulator			m_DecreaseRegulator;
	MOTION_FACTOR_GROUP m_Factors;												///< 모션 팩터값
	MF_STATE			m_nCurrentMF;
	int					m_nCurrentWeight;
	bool				m_bActive;

protected:
	bool				CheckActivateMF(MF_STATE& outState, int& outWeight, bool bIsSticky=false);	///< 활성화될 모션 팩터값이 있는지 확인
	void				IncreaseVars( MOTION_FACTOR_GROUP &MotionFactorGroup, PerMulVar<short>* pValueModifiers, PerMulVar<short>* pWeightModifiers, int nNotApplyMF, vector<int>* pvecAdditionIgnoreMF );
	void				DecreaseVars();											///< 모션 팩터값 감소
	bool				CheckOverridable(MF_STATE nCurrMF, MF_STATE nNewMF);
	bool				CheckFakeMF(MF_STATE& outState, int& outWeight, PerMulVar<short>* pValueModifiers, bool bIsSticky=false);		///< Fake 모션팩터가 있는지 확인
public:
	GMotionFactor();
	virtual ~GMotionFactor(void);
	void Update(float fDelta);
	void ClearVars();												///< 모션 팩터값 초기화
	bool Apply(MOTION_FACTOR_GROUP& MotionFactorGroup, PerMulVar<short>* pValueModifiers, PerMulVar<short>* pWeightModifiers, int nNotApplyMF=MF_NONE, vector<int>* pvecAdditionIgnoreMF=NULL, bool bIsSticky=false );

	void OnReleased();
	void ReleaseFakes();
	void Force(MF_STATE nState, int nWeight);
	MF_STATE	GetCurrMF() const	{ return m_nCurrentMF; }
	int			GetCurrWeight() const	{ return m_nCurrentWeight; }
	wstring GetDebugString();		///< 디버그 정보 반환
	MOTION_FACTOR_GROUP& GetFactorsForTest()	{ return m_Factors; }

	void SetActive(bool bActive)	{ m_bActive = bActive; }
	bool IsActive()					{ return m_bActive; }
};




#endif // _GMOTION_FACTOR_H