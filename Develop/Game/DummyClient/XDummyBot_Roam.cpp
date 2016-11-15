#include "stdafx.h"
#include "XDummyBot_Roam.h"
#include "XDummyMaster.h"
#include "XDummyCollision.h"

XDummyBot_Roam::XDummyBot_Roam(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo) 
: XDummyBot(pAgent, pAgentInfo)
{
	USES_CONVERSION_EX;

	m_fStopTime = 0.0f;
	
	wstring strSpeed = pAgentInfo->GetValue(L"speed");
	if (!strSpeed.empty())
	{
		m_fSpeed = (float)_wtoi(strSpeed.c_str());
	}
	else
	{
		m_fSpeed = 150.0f;
	}

	m_vPosition = pAgent->GetCenter();
	m_fElapsedPostTime = 0.0f;

	SetNextRandomGoalPos();
}

void XDummyBot_Roam::OnRun(float fDelta)
{
	if (m_pAgent->GetStatus() != XBirdDummyAgent::DAS_RUNNING) return;

	if (m_fStopTime != 10000.0f)
	{
		if (m_fStopTime > 10.0f)
		{
//			m_pAgent->GetClient()->SetActiveRecv(false);
			m_fStopTime = 10000.0f;
		}
		else	
		{
			m_fStopTime += fDelta;
		}
	}

	Roam();

	UpdateMovingPost(fDelta);
}

minet::MCommandResult XDummyBot_Roam::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	XDummyBot::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
			TD_MYINFO vecTDMyInfo;

			if (pCommand->GetSingleBlob(vecTDMyInfo, 0)==false) return CR_ERROR;

			if (3 != vecTDMyInfo.nPlayerGrade)
			{
				XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			}
			break;
		}
	case MC_FIELD_START_GAME:
		{
			m_fElapsedPostTime = 0.0f;
		}
		break;
	case MC_FIELD_CHANGE_FIELD:
		{
			XBIRDPOSTCMD1(pClient, MC_GM_REBIRTH_REQ, MCommandParameterVector(m_vPosition));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
		}
		break;
	}
	return CR_FALSE;
}

void XDummyBot_Roam::SetNextRandomGoalPos()
{
	float fRadius = RandomNumber(0.0f, MMath::PI*2.0f);
	float fDistance = RandomNumber(0.0f, 1000.0f);

	MVector2 vTarPos = MMath::GetArcPosition(fRadius, fDistance);

	m_vGoalPos.x = m_pAgent->GetCenter().x + vTarPos.x;
	m_vGoalPos.y = m_pAgent->GetCenter().y + vTarPos.y;
	m_vGoalPos.z = m_pAgent->GetCenter().z;
}

void XDummyBot_Roam::UpdateMovingPost( float fDeltaTime )
{
	if (m_pAgent->GetStatus() != XBirdDummyAgent::DAS_RUNNING) return;

	m_fElapsedPostTime += fDeltaTime;

	vec3 vDir = m_vGoalPos - m_vPosition;
	vDir.Normalize();

	if (m_vGoalPos.DistanceTo(m_vPosition) < (fDeltaTime * m_fSpeed))
	{
		m_vPosition = m_vGoalPos;
	}
	else
	{
		m_vPosition += (vDir * m_fSpeed * fDeltaTime);
	}

	m_vPosition.z = m_pAgent->GetGroundZ(m_vPosition);

	if (m_fElapsedPostTime > 0.1f)
	{
		TD_PC_MOVE mi;
		mi.vTarPos = m_vPosition;
		vec3 vTransDir = vDir;
		vTransDir.z = 0.0f;
		vTransDir.Normalize();
		mi.svDir.SetX(vTransDir.x);
		mi.svDir.SetY(vTransDir.y);
		mi.nFlags = MOVEMENTFLAG_FORWARD;

		XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_ACTION_MOVE_REQ, MCommandParameterSingleBlob(&mi,sizeof(TD_PC_MOVE)));

		m_fElapsedPostTime = 0.0f;
	}
}

void XDummyBot_Roam::Roam()
{
	if (m_vPosition.DistanceTo(m_vGoalPos) < 100.0f)
	{
		SetNextRandomGoalPos();
	}
}