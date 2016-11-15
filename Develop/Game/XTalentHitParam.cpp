#include "stdafx.h"
#include "XTalentHitParam.h"
#include "XSystem.h"

uint32 XTalentDelayParam::MakeDelayTime( float fTalentActElapsedTime, uint32 nTolerTime )
{
	uint32 ret_delay = 0;

	if (m_nHitTime <= 0 || m_nHitTime >= 3000)
	{
		ret_delay = 0;
	}

	uint32 nTalentActElapsedTime = (uint32)(fTalentActElapsedTime * 1000);
	if (this->m_nHitTime > nTalentActElapsedTime)
	{
		ret_delay = this->m_nHitTime - nTalentActElapsedTime;
	}

	if (ret_delay > nTolerTime)
	{
		ret_delay -= nTolerTime;
	}

	return ret_delay;
}

void XTalentDelayParam::SetHitTime( uint32 nHitTime )
{
	this->m_nRecvTime				= global.system->GetTimer().GetNowGlobalTime();
	this->m_nHitTime				= nHitTime;
}

//////////////////////////////////////////////////////////////////////////

XTalentHitParam::XTalentHitParam() : XTalentDelayParam(), nVictimUIID(UIID_INVALID)
{
	nTalentID	= 0;
	nMFState			= MF_NONE;
	nMFWeight			= 0;
	nDamage				= 0;
	nFlags				= 0;

	bSecondaryEffect	= true;

	vServerKnockbackDir.Set(vec3::AXISY);
	vServerKnockbackVictimPos.Set(vec3::ZERO);
	vServerKnockbackVictimTargetPos.Set(vec3::ZERO);
}

void XTalentHitParam::Build( TD_TALENT_HIT* pHitInfo )
{
	this->nAttackerUIID			= pHitInfo->nAttackerUIID;
	this->nVictimUIID			= pHitInfo->nVictimUIID;
	this->nTalentID				= pHitInfo->nTalentID;
	this->nMFState				= MF_STATE(pHitInfo->nMFState);
	this->nMFWeight				= pHitInfo->nMFWeight;
	this->nDamage				= pHitInfo->nDamage;
	this->nFlags				= (uint32)pHitInfo->nFlags;

	SetHitTime(pHitInfo->nHitTime);

}

void XTalentHitParam::Build_KnockBack( TD_TALENT_HIT* pHitInfo, TD_TALENT_HIT_EX_KNOCKBACK* pKnockBackHitInfo )
{
	Build(pHitInfo);

	this->vServerKnockbackDir	= pKnockBackHitInfo->vKnockbackDir;
	this->vServerKnockbackVictimPos = pKnockBackHitInfo->vKnockbackVictimPos;
	this->vServerKnockbackVictimTargetPos = pKnockBackHitInfo->vKnockbackTargetPos;
}

XTalentHitParam* XTalentHitParam::Clone()
{
	XTalentHitParam* pNewClone = new XTalentHitParam();

	pNewClone->m_nRecvTime = m_nRecvTime;
	pNewClone->m_nHitTime = m_nHitTime;
	pNewClone->nAttackerUIID = nAttackerUIID;
	pNewClone->nVictimUIID = nVictimUIID;

	pNewClone->nTalentID = nTalentID;
	pNewClone->vServerKnockbackDir = vServerKnockbackDir;
	pNewClone->vServerKnockbackVictimPos = vServerKnockbackVictimPos;
	pNewClone->vServerKnockbackVictimTargetPos = vServerKnockbackVictimTargetPos;
	pNewClone->nMFState = nMFState;
	pNewClone->nMFWeight = nMFWeight;
	pNewClone->nDamage = nDamage;
	pNewClone->nFlags = nFlags;

	return pNewClone;

}

void XTalentHitParam::Build_NoMotionFactor( UIID nAttackerUIID, UIID nVictimUIID, int nTalentID )
{
	this->nAttackerUIID = nAttackerUIID;
	this->nVictimUIID = nVictimUIID;
	this->nTalentID = nTalentID;

	this->nDamage = 0;
	this->nMFState = MF_NONE;
	this->nMFWeight = 0;
	
	SetHitTime(0);
}

void XTalentHitParam::Build_BuffNoMotionFactor( UIID nVictimUIID, int nTalentID )
{
	this->nAttackerUIID = UIID_INVALID;
	this->nVictimUIID = nVictimUIID;
	this->nTalentID = nTalentID;

	this->nDamage = 0;
	this->nMFState = MF_NONE;
	this->nMFWeight = 0;
	
	SetHitTime(0);

}

void XTalentGuardDefenseParam::Assign( TD_TALENT_GUARD* pTalentGuardInfo )
{
	this->nAttackerUIID			= pTalentGuardInfo->nAttackerUIID;
	this->nAttackerTalentID		= pTalentGuardInfo->nTalentID;
	this->eGuardType			= pTalentGuardInfo->nType;

	if(this->eGuardType == GT_PARTIAL)
	{
		TD_TALENT_PARTIAL_GUARD* pPartialGuard = static_cast<TD_TALENT_PARTIAL_GUARD*>(pTalentGuardInfo);
		this->nDamage			= pPartialGuard->nGuardedDamage;	
		this->vGuarderDir		= pPartialGuard->vGuarderDir;
	}
	SetHitTime(pTalentGuardInfo->nHitTime);
}