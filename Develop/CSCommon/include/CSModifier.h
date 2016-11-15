#pragma once

#include "CSCommonLib.h"
#include "CSMotionFactorInfo.h"
#include "CSTalentInfoEnum.h"
#include "MTypes.h"
#include "CSDef.h"

/// nMod와 fPer이 있는 Modifier. fPer는 100%에서 누적된다.
template<typename T>
struct ModVar
{
	T	  nMod; // 상수 보정치
	float fPer; // 백분율 누적 보정치

	ModVar()	
	{ 
		Clear();
	}
	ModVar(T nMod, float fPer)
	: nMod(nMod), fPer(fPer)
	{ 
		Clear();
	}
	bool IsModified() const
	{
		if (nMod != (T)(0))	return true;
		if (fPer != 0.0f)	return true;
		return false;
	}
	T Calc(T nOriginal) const
	{
		return (T)((nOriginal + nMod) * (1.0f+fPer));
	}
	T CalcDiff(T nOriginal) const
	{
		return (T)(((nOriginal + nMod) * (1.0f+fPer)) - nOriginal);
	}
	void Modify(T& noutVar) const
	{
		noutVar = Calc(noutVar);
	}
	void Clear()
	{
		nMod = (T)(0);
		fPer = 0.0f;
	}
	ModVar&	operator*=( const int& rhs )
	{
		fPer += rhs;
		return *this;
	}
	ModVar&	operator+=( const ModVar& rhs )
	{
		nMod += rhs.nMod;
		fPer += rhs.fPer;
		return *this;
	}
	ModVar&	operator-=( const ModVar& rhs )
	{
		nMod -= rhs.nMod;
		fPer -= rhs.fPer;
		return *this;
	}
	bool operator==( const ModVar& rhs ) const
	{
		if (nMod == rhs.nMod && fPer == rhs.fPer) return true;
		return false;
	}
};

template<typename T>
struct PerModVar
{
	float fPer; // 백분율 누적 보정치

	PerModVar()
	{
		Clear();
	}
	bool IsModified() const
	{
		if (fPer != 0.0f)	return true;
		return false;
	}
	T	Calc(T nOriginal) const
	{
		return (T)(nOriginal * (1.0f+fPer));
	}
	void Modify(T& noutVar) const
	{
		noutVar = Calc(noutVar);
	}
	void Clear()
	{
		fPer = 0.0f;
	}
	PerModVar&	operator+=( const PerModVar& rhs )
	{
		fPer			+= rhs.fPer;
		return *this;
	}
	PerModVar&	operator-=( const PerModVar& rhs )
	{
		fPer			-= rhs.fPer;
		return *this;
	}
	bool		operator==( const PerModVar& rhs ) const
	{
		if (fPer == rhs.fPer) return true;
		return false;
	}
};

template<typename T>
struct PerMulVar
{
	float fPer; // 백분율 보정치. 누적하지 않음

	PerMulVar()
	{
		Clear();
	}
	bool	IsModified() const
	{
		if (fPer != 1.0f)	return true;
		return false;
	}
	T		Calc(T nOriginal) const
	{
		return (T)(nOriginal * fPer);
	}
	void	Modify(T& noutVar) const
	{
		noutVar = Calc(noutVar);
	}
	void	Clear()
	{
		fPer = 1.0f;
	}
	PerMulVar&	operator+=( const PerMulVar& rhs )
	{
		// 누적하지 않음
		return *this;
	}
	PerMulVar&	operator*=( const PerMulVar& rhs )
	{
		fPer			*= rhs.fPer;
		return *this;
	}
	PerMulVar&	operator-=( const PerMulVar& rhs )
	{
		// 누적하지 않음
		return *this;
	}
	bool		operator==( const PerMulVar& rhs ) const
	{
		if (fPer == rhs.fPer) return true;
		return false;
	}
};

class CSCOMMON_API CSInstantModifier
{
public:
	ModVar<int>		nHP;
	ModVar<int>		nEN;
	ModVar<int>		nSTA;	

	CSInstantModifier();

	CSInstantModifier& operator+=( const CSInstantModifier& rhs );
	CSInstantModifier& operator-=( const CSInstantModifier& rhs );

	bool IsModified() const;
	void Clear();
};

class CSCOMMON_API CSActorModifier
{
public:
	ModVar<int>				nHPMax;
	ModVar<int>				nENMax;
	ModVar<int>				nSTAMax;

	float					fMoveSpeed;					// (모두)
	float					fRideSpeed;
	ModVar<int>				nHPRegen,					// (서버)
							nENRegen,					// (서버)
							nSTARegen;					// (서버)
	

	// 전투 관련
	ModVar<int>				nAP,						///< 방어력
							nABS;						///< 피해 흡수

	ModVar<float>			fGuardRate;					///< 방어율

	float					fMeleeDodgeAmp;
	float					fRangeDodgeAmp;
	float					fMagicDodgeAmp;

	float					fMeleeHitRateAmp;
	float					fRangeHitRateAmp;
	float					fMagicHitRateAmp;

	float					fCriticalAmp;
	float					fCriticalMeleeAmp;
	float					fCriticalRangeAmp;	
	float					fCriticalMagicAmp;

	float					fCriticalMeleeAmpForMe;

	float					fCriticalMeleeDamageAmp;	///< 추가 치명 대미지율 ( 1 = 100% )
	float					fCriticalRangeDamageAmp;	///< 추가 치명 대미지율 ( 1 = 100% )
	float					fCriticalMagicDamageAmp;	///< 추가 치명 대미지율 ( 1 = 100% )

	float					fMeleeDamageAmp;			///< 추가 근접 대미지율 (1 = 100%)
	float					fRangeDamageAmp;			///< 추가 원거리 대미지율 (1 = 100%)
	float					fMagicDamageAmp;			///< 추가 마법 대미지율 (1 = 100%)


	float					fDefenceAmp[DA_MAX];		///< 추가 방어율(1 = 100%)

	ModVar<int>				nResistances[TRT_MAX];		///< 저항

	ModVar<short>			MotionFactorValue[MF_PRIORITY_SIZE];
	ModVar<short>			MotionFactorWeight[MF_PRIORITY_SIZE];

	PerModVar<float>		fGatherTimeAmp[MAX_GATHER_TYPE];				///< 채집 시간 보정치


	float					fModMeleePene;				///< 근접 공격 관통율 보너스
	float					fModRangePene;				///< 원거리 공격 관통율 보너스
	float					fModMagicPene;				///< 마법 공격 관통율 보너스

public:
	CSActorModifier();

	CSActorModifier& operator+=( const CSActorModifier& rhs );
	CSActorModifier& operator-=( const CSActorModifier& rhs );

	bool IsMotionfactorModified() const;
	bool IsModified() const;
	void Clear();
};

class CSCOMMON_API CSPlayerModifier
{
public:
	// 기본 스탯
	ModVar<int>				nSTR,						// (모두)-
							nDEX,						// (모두)-
							nINT,						// (모두)-
							nCHA,						// (모두)-
							nCON;						// (모두)-							

public:
	CSPlayerModifier();

	CSPlayerModifier& operator+=( const CSPlayerModifier& rhs );
	CSPlayerModifier& operator-=( const CSPlayerModifier& rhs );

	bool IsModified() const;
	void Clear();
	void CutInstantMod();								///< 즉시효과를 주는 속성만 남기기
};
