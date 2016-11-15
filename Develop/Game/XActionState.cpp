#include "stdafx.h"
#include "XMyActionState.h"
#include "XModuleAction.h"
#include "XModuleMotion.h"
#include "XModuleMovable.h"
#include "XModuleSkill.h"
#include "XModuleEntity.h"
#include "XObject.h"
#include "XPost_Char.h"
#include "XMessageID.h"

static string _ActionStateStr[] = 
{
	"ACTION_STATE_IDLE",
	"ACTION_STATE_RUN",
	"ACTION_STATE_JUMP",
	"ACTION_STATE_FALLING",
	"ACTION_STATE_CHANGE_STANCE",
	"ACTION_STATE_DAMAGED",
	"ACTION_STATE_TACKLE",
	"ACTION_STATE_GUARD",
	"ACTION_STATE_NORMAL_ATTACK",
	"ACTION_STATE_SPECIAL_ATTACK",
	"ACTION_STATE_USE_TALENT",
	"ACTION_STATE_USE_ARCHERY",
	"ACTION_STATE_USE_GUN",
};

void XMyActionState::Enter()
{
	int nID = GetID();
	if ((nID >= 0) && (nID < MAX_ACTION_STATE)) 
		mlog_filter("birdkr", "%s : Enter\n", _ActionStateStr[nID].c_str());
}

void XMyActionState::Exit()
{
	int nID = GetID();
	if ((nID >= 0) && (nID < MAX_ACTION_STATE)) 
		mlog_filter("birdkr", "%s : Exit\n", _ActionStateStr[nID].c_str());
}

// Idle //////////////////////////////////////////////////////////
void XMyActionIdle::Enter()
{
	XMyActionState::Enter();

	if (m_pOwner->m_pModuleMotion) m_pOwner->m_pModuleMotion->ChangeMotion(MOTION_NAME_IDLE);
}


// Jump //////////////////////////////////////////////////////////
void XMyActionJump::Enter()
{
	XMyActionState::Enter();

	bool bDoAction = false;
	XMotionTypes nMotionTypes = MT_DEFAULT;

	XModuleMovable* pModuleMovable = m_pOwner->GetOwner()->GetModuleMovable();
	vec3 dir = pModuleMovable->GetVelocity();
	dir.Normalize();
	
	bDoAction = m_pOwner->Jump(dir, nMotionTypes);

	if (bDoAction)
	{
		XPostAction(AT_JUMP, dir, (unsigned int)nMotionTypes );
	}
}

XMessageResult XMyActionJump::Message(XMessage& msg)
{
	switch (msg.m_nID)
	{
	case XMSGL_ON_CHANGE_MOTION:
		{
			XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
			if (pModuleMotion)
			{
				if (!stricmp(pModuleMotion->GetCurrMotion().c_str(), MOTION_NAME_IDLE))
				{
					m_pOwner->DoAction(ACTION_STATE_IDLE);
				}
			}
		}
		break;
	}
	
	return MR_FALSE;
}

// Change State //////////////////////////////////////////////////////////
void XMyActionChangeState::Enter()
{
	XMyActionState::Enter();

	bool bDoAction = false;
	XMotionTypes nMotionTypes = MT_DEFAULT; //GetCurrMotionType();

	char nStanceTo = CS_NORMAL;
	if (m_pOwner->IsCurrStanceBattle())
	{
		nStanceTo = CS_NORMAL;
	}
	else 
	{
		nStanceTo = CS_BATTLE;
	}

	if (m_pOwner->ValidateChangeStance((CHAR_STANCE)(nStanceTo)))
	{
		XMessage msg(XMSGA_CHANGE_STANCE, &nStanceTo);
		m_pOwner->Message(msg);
	}
}

void XMyActionChangeState::Exit()
{
	XMyActionState::Exit();
}

XMessageResult XMyActionChangeState::Message(XMessage& msg)
{
	switch (msg.m_nID)
	{
	case XMSGL_ON_CHANGE_MOTION:
		{
			XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
			if (pModuleMotion)
			{
				if (!stricmp(pModuleMotion->GetCurrMotion().c_str(), MOTION_NAME_IDLE))
				{
					m_pOwner->DoAction(ACTION_STATE_IDLE);
				}
			}
		}
		break;
	}

	return MR_FALSE;
}


// Use Talent //////////////////////////////////////////////////////////
void XMyActionUseTalent::Enter()
{
	XMyActionState::Enter();

	int nSkillID = 1;		// 기냥 테스트니까 1번 스킬
	//nSkillID=100000;
	XModuleSkill* pModuleSkill = m_pOwner->GetOwner()->GetModuleSkill();
	if (pModuleSkill)
	{
		if (pModuleSkill->ValidateUse(nSkillID))
		{
			XModuleEntity* pModuleEntity = m_pOwner->GetOwner()->GetModuleEntity();

			XMSG_PARAM_USE_SKILL param;
			param.nSkillID = nSkillID;
			param.vDir = pModuleEntity->GetDirection();
			XMessage msg(XMSGA_USE_SKILL, &param);
			m_pOwner->GetOwner()->Message(msg);
		}
	}
}

void XMyActionUseTalent::Exit()
{
	XMyActionState::Exit();


}


// Change State //////////////////////////////////////////////////////////
//void XMyActionChangeState::Enter()
//{
//	XMyActionState::Enter();
//}
//
//void XMyActionChangeState::Exit()
//{
//	XMyActionState::Exit();
//}