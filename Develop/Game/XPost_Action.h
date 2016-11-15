#ifndef _XPOST_ACTION_H
#define _XPOST_ACTION_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "CTransData.h"
#include "XMovableDebugger.h"

inline bool IsLockOnMultiplePartTarget(const vector<TD_TALENT_TARGET_DETAIL>& vTargets)
{
	for (size_t i=0; i<vTargets.size(); i++)
		if (vTargets[i].nCapsuleGroupIndex != -1)
			return true;

	return false;
}

inline void XPostChangeStance(int8 nValue)
{
	XPOSTCMD1(MC_ACTION_CHANGE_STANCE_REQ, MCommandParameterChar(nValue));
}

inline void XPostAttackMelee(vec3 vPos, vec3 vDir, TALENT_NORMAL_ATTACK_TYPE nType)
{
	__Report_Movable_Debug_PostMessage(L"attack_melee", vPos);

	TD_REQUEST_ATTACK ra;
	ra.vPos = vPos;
	ra.svDir = vDir;
	ra.nAttackType = nType;

	XPOSTCMD1(MC_ACTION_ATTACK_MELEE_REQ, MCommandParameterSingleBlob(&ra,sizeof(TD_REQUEST_ATTACK)));
};

inline void XPostAttackArchery(vec3 vPos, vec3 vDir, MUID uidTarget)
{
	XPOSTCMD3(MC_ACTION_ATTACK_ARCHERY_REQ, MCommandParameterVector(vPos), MCommandParameterVector(vDir), MCommandParameterUID(uidTarget));
};

inline void XPostAttackArcheryDetail(vec3 vPos, vec3 vDir, MUID uidTarget, int8 nCapsuleGroupIndex, int8 nCapsuleIndex)
{
	XPOSTCMD5(MC_ACTION_ATTACK_ARCHERY_DETAIL_REQ, 
		MCommandParameterVector(vPos), 
		MCommandParameterVector(vDir), 
		MCommandParameterUID(uidTarget),
		MCommandParameterChar(nCapsuleGroupIndex),
		MCommandParameterChar(nCapsuleIndex)
		);
};

inline void XPostAttackGun(MUID& TargetUID, vec3& vPos, vec3& vDir)
{
//	if (nCount > 32) return;	// ±×³É 32..-_-¤»

	MCommand* pNewCmd = XNewCmd(MC_ACTION_ATTACK_GUN_REQ);
	pNewCmd->AddParameter(new MCommandParameterVector(vPos));
	pNewCmd->AddParameter(new MCommandParameterVector(vDir));
	pNewCmd->AddParameter(new MCommandParameterUID(TargetUID));
	XPostCommand(pNewCmd);
};

inline void XPostAttackPrepare()
{
	XPOSTCMD0(MC_ACTION_PREPARE_ATTACK_REQ);
};

inline void XPostAttackCancel()
{
	XPOSTCMD0(MC_ACTION_ATTACK_CANCEL_REQ);
};

inline void XPostGuard()
{
	XPOSTCMD0(MC_ACTION_GUARD_REQ);
};

inline void XPostGuardReleased()
{
	XPOSTCMD0(MC_ACTION_GUARD_RELEASED_REQ);
};

inline void XPostJump(vec3& pos, vec3& dir)
{
	XPOSTCMD2(MC_ACTION_JUMP_REQ, MCommandParameterVector(pos), MCommandParameterVector(dir));
}

inline void XPostFalling(vec3 pos, vec3 dir)
{
	dir.Normalize();
	XPOSTCMD2(MC_ACTION_FALLING_REQ, MCommandParameterVector(pos), MCommandParameterShortVector(dir));
}

inline void XPostFallingEnd(bool bIsFallenOnWater, float fFallingHeight)
{
	XPOSTCMD2(MC_ACTION_END_FALLING, MCommandParameterBool(bIsFallenOnWater), MCommandParameterFloat(fFallingHeight));
}

inline void XPostUseTalent(int nTalentID, vec3 vCharDir, vec3 vCharPos)
{
	__Report_Movable_Debug_PostMessage(L"useTalent", vCharPos);

	vCharDir.Normalize();

	XPOSTCMD3(MC_ACTION_USE_TALENT_REQ, MCommandParameterInt(nTalentID), MCommandParameterShortVector(vCharDir), MCommandParameterVector(vCharPos));

	gvar.MyInfo.Talent.CoolTime.RequestUseTalent( nTalentID);
}

inline void XPostActTalent(int nTalentID, vec3 vDir, MUID uidTarget)
{
	vDir.Normalize();
	XPOSTCMD2(MC_ACTION_ACT_TALENT_REQ, MCommandParameterShortVector(vDir), MCommandParameterUID(uidTarget)); 

	gvar.MyInfo.Talent.CoolTime.RequestActTalent( nTalentID);
}

inline void XPostActTalentWithHitCapsule(int nTalentID, vec3 vDir, TD_TALENT_TARGET_DETAIL& target)
{
	vDir.Normalize();
	XPOSTCMD2(MC_ACTION_ACT_TALENT_WITH_HITCAPSULE_REQ, MCommandParameterShortVector(vDir), MCommandParameterSingleBlob(&target, sizeof(TD_TALENT_TARGET_DETAIL))); 

	gvar.MyInfo.Talent.CoolTime.RequestActTalent( nTalentID);
}

inline void XPostActTalentWithGround(int nTalentID, vec3 vDir, vec3 vGroundTarget)
{
	vDir.Normalize();
	XPOSTCMD2(MC_ACTION_ACT_TALENT_WITH_GROUND_REQ, MCommandParameterShortVector(vDir), MCommandParameterVector(vGroundTarget)); 

	gvar.MyInfo.Talent.CoolTime.RequestActTalent( nTalentID);
}

inline void XPostCancelTalent()
{
	XPOSTCMD0(MC_ACTION_CANCEL_TALENT_REQ);
}

inline void XPostStandUp()
{
	XPOSTCMD0(MC_ACTION_STANDUP_REQ);
}


inline void XPostRequestPlayerExtraInfo(const MUID& uidTarPlayer)
{
	XPOSTCMD1(MC_FIELD_PLAYER_EXTRA_INFO_REQ, MCommandParameterUID(uidTarPlayer));
}

inline void XPostPartyRequestInvite(const MUID& uidTarPlayer)
{
	XPOSTCMD1(MC_PARTY_INVITE_REQ, MCommandParameterUID(uidTarPlayer));
}

inline void XPostPartyRequestInviteForMe(const MUID& uidTarParty)
{
	XPOSTCMD1(MC_PARTY_INVITE_FOR_ME_REQ, MCommandParameterUID(uidTarParty));
}

inline void XPostRequestSwitchingWeaponSet(int8 nWeaponSet)
{
	XPOSTCMD1(MC_ACTION_SWITCHING_WEAPON_REQ, MCommandParameterChar(nWeaponSet));
}

inline void XPostRequestSwitchingWeaponSetBegin()
{
	XPOSTCMD0(MC_ACTION_SWITCHING_WEAPON_BEGIN_REQ);
}

inline void XPostPartyRequestInviteQuestionRespond(int nRespond)
{
	XPOSTCMD1(MC_PARTY_INVITE_QUESTION_RESPOND, MCommandParameterInt(nRespond));
}

inline void XPostPartyRequestInviteForMeQuestionRespond(int nRespond)
{
	XPOSTCMD1(MC_PARTY_INVITE_FOR_ME_QUESTION_RESPOND, MCommandParameterInt(nRespond));
}

inline void XPostPartyRequestLeave()
{
	XPOSTCMD0(MC_PARTY_LEAVE_REQ);
}

inline void XPostPartyRequestKick(const MUID& uidTarPlayer)
{
	XPOSTCMD1(MC_PARTY_KICK_REQ, MCommandParameterUID(uidTarPlayer));
}

//inline void XPostPartyChangeName(const wchar_t* szName)
//{
//	XPOSTCMD1(MC_PARTY_CHANGE_NAME_REQ, MCommandParameterWideString(szName));
//}

inline void XPostPartyChangeLeader(MUID uidMember)
{
	XPOSTCMD1(MC_PARTY_CHANGE_LEADER_REQ, MCommandParameterUID(uidMember));
}

inline void XPostPartyChangeLootingRule(LOOTING_RULE_DATA rule)
{
	XPOSTCMD3(MC_PARTY_CHANGE_LOOTING_RULE_REQ, MCommandParameterChar(rule.m_nLRC), MCommandParameterChar(rule.m_nLRR), MCommandParameterChar(rule.m_nLRRF));
}

inline void XPostSwim(bool bIsSwimming)
{
	XPOSTCMD1(MC_ACTION_SWIM_REQ, MCommandParameterBool(bIsSwimming));
}

inline void XPostSit()
{
	XPOSTCMD0(MC_ACTION_SIT_REQ);
}

inline void XPostSitRise()
{
	XPOSTCMD0(MC_ACTION_SIT_RISE_REQ);
}


#endif // _XPOST_ACTION_H