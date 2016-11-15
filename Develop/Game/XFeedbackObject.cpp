#include "stdafx.h"
#include "XFeedbackObject.h"
#include "XModuleActorControl.h"
#include "XMyPlayer.h"

MImplementRTTI(XFeedbackObject, XNonPlayer);

#define FEEDBACK_BALLOON_TICK		(10.0f)

XFeedbackObject::XFeedbackObject(MUID uid) : XNonPlayer(uid, 0)
, m_bBalloonVisible(false)
{

}

XFeedbackObject::~XFeedbackObject()
{

}

void XFeedbackObject::Init( vec3 vPos, vec3 vDir, wstring& strName, wstring& strComment )
{
	m_BalloonResetRegulator.SetTickTime(FEEDBACK_BALLOON_TICK);
	m_BalloonResetRegulator.Start();

	m_rgtDistanceCheck.SetTickTime(3.0f);
	m_rgtDistanceCheck.Start();

	m_strName = strName;
	m_strComment = strComment;

	XNPCInfo* pNPCInfo = info.npc->Get(20003);
	if (pNPCInfo == NULL) return;

	TD_UPDATE_CACHE_NPC cacheNPC;
	cacheNPC.uid = m_UID;
	cacheNPC.nNPCID = pNPCInfo->nID;
	cacheNPC.vPos = vPos;
	cacheNPC.svDir = vDir;
	cacheNPC.vTarPos = vPos;
	cacheNPC.nSpeed = 0;

	InNPC(pNPCInfo, &cacheNPC, false);
}

void XFeedbackObject::OnUpdate( float fDelta )
{
	XNonPlayer::OnUpdate(fDelta);

	UpdateBalloon(fDelta);
}

vec3 XFeedbackObject::GetNamePos()
{
	vec3 pos = GetPosition();
	pos.z += 165.0f;
	return pos;
}

void XFeedbackObject::UpdateBalloon( float fDelta )
{

	if (m_BalloonResetRegulator.IsReady(fDelta))
	{
		m_bBalloonVisible = false;
	}

	if (m_rgtDistanceCheck.IsReady(fDelta))
	{
		if (!m_bBalloonVisible)
		{
			vec3 vMyPos = gvar.Game.pMyPlayer->GetPosition();
			float fDist = vMyPos.DistanceTo(GetPosition());

			if (fDist <= 300.0f)
			{
				ShowBalloon();
			}
		}
	}
}

void XFeedbackObject::ShowBalloon()
{
	XModuleActorControl* pActorControl = GetModuleActorControl();
	if (pActorControl)
	{
		float fMaintainTime = FEEDBACK_BALLOON_TICK;
		pActorControl->OnBallonText(m_strName, m_strComment, fMaintainTime);
	}

	m_bBalloonVisible = true;
	m_BalloonResetRegulator.Reset();

	m_rgtDistanceCheck.Reset(true);
}