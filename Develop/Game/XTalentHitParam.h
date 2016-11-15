#pragma once

#include "MUID.h"
#include "CSMotionFactorInfo.h"
#include "MTypes.h"

class XTalentDelayParam
{
protected:
	uint32					m_nRecvTime;		// 커맨드를 받은 시간
	uint16					m_nHitTime;		// 실제로 히트될 시간

	void SetHitTime(uint32 nHitTime);
public:
	UIID					nAttackerUIID;
	int						nTalentID;

	XTalentDelayParam() : m_nRecvTime(0), m_nHitTime(0), nAttackerUIID(UIID_INVALID), nTalentID(0) {}

	uint32 MakeDelayTime(float fTalentActElapsedTime, uint32 nTolerTime=60);
	uint32 GetRecvTime() { return m_nRecvTime; }
	uint32 GetHitTime() { return m_nHitTime; }
};

class XTalentHitParam : public XTalentDelayParam
{
private:

public:
	UIID					nVictimUIID;
	vec3					vServerKnockbackDir;
	vec3					vServerKnockbackVictimPos;
	vec3					vServerKnockbackVictimTargetPos;
	MF_STATE				nMFState;
	int						nMFWeight;
	int						nDamage;
	uint32					nFlags;

	bool					bSecondaryEffect;


	XTalentHitParam();
	void Build(TD_TALENT_HIT* pHitInfo);
	void Build_KnockBack(TD_TALENT_HIT* pHitInfo, TD_TALENT_HIT_EX_KNOCKBACK* pKnockBackHitInfo);
	void Build_NoMotionFactor(UIID nAttackerUIID, UIID nVictimUIID, int nTalentID);
	void Build_BuffNoMotionFactor(UIID nVictimUIID, int nTalentID);

	XTalentHitParam* Clone();

	bool IsFlagMiss()			{ return (CheckBitSet(nFlags, CTR_MISS)) ? true : false;	}
	bool IsFlagRiposte()		{ return (CheckBitSet(nFlags, CTR_RIPOSTE)) ? true : false;	}
	bool IsFlagResistPerfect()	{ return (CheckBitSet(nFlags, CTR_RESIST_PERFECT)) ? true : false;	}
	bool IsFlagResistPartial()	{ return (CheckBitSet(nFlags, CTR_RESIST_PARTIAL)) ? true : false;	}
	bool IsFlagCritical()		{ return (CheckBitSet(nFlags, CTR_CRITICAL)) ? true : false;	}
	bool IsFlagDrain()			{ return (CheckBitSet(nFlags, CTR_DRAIN)) ? true : false;	}
	bool IsFlagZeroDamage()		{ return (CheckBitSet(nFlags, CTR_ZERO_TALENT_DAMAGE)) ? true : false;	}
	bool IsFlagImmune()			{ return (CheckBitSet(nFlags, CTR_IMMUNE)) ? true : false;	}
	bool IsFlagAvoid()			{ return (CheckBitSet(nFlags, CTR_AVOID)) ? true : false;	}
	bool IsFlagFalling()		{ return (CheckBitSet(nFlags, CTR_FALLING)) ? true : false;	}
	bool IsFlagDead()			{ return (CheckBitSet(nFlags, CTR_DEAD)) ? true : false;	}
	bool IsFlagBPart1()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART1)) ? true : false;	}
	bool IsFlagBPart2()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART2)) ? true : false;	}
	bool IsFlagBPart3()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART3)) ? true : false;	}
	bool IsFlagBPart4()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART4)) ? true : false;	}

};

class XTalentGuardDefenseParam : public XTalentDelayParam
{
public:
	int						nAttackerTalentID;
	int						nDamage;
	GUARD_TYPE				eGuardType;

	vec3					vGuarderDir; // 가드한 방향

	XTalentGuardDefenseParam() : nAttackerTalentID(0), vGuarderDir(vec3::AXISY){}
	void Assign(TD_TALENT_GUARD* pTalentGuardInfo);
};


struct _DamageRepeatEvent
{
	float				fTime;
	int					nDamage;
	wstring				strEffectName;
	wstring				strBone;

	_DamageRepeatEvent()
	{
		fTime			= 0.0f;
		nDamage			= 0;
		strEffectName.clear();
		strBone.clear();
	}
};

class _DamageRepeatInfo : public XTalentHitParam
{
public:
	vector<_DamageRepeatEvent> vecDamageRepeatEvents;

	_DamageRepeatInfo()
	{
		vecDamageRepeatEvents.clear();
	}
	~_DamageRepeatInfo()
	{
		vecDamageRepeatEvents.clear();
	}
};

