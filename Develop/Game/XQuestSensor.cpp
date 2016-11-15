#include "stdafx.h"
#include "XQuestSensor.h"
#include "XPost_Comm.h"

XQuestSensor::XQuestSensor( unsigned long nSensorID, XSensorInfo* pInfo, XQuestInfo* pQuestInfo, GATE_TYPE nGateType )
: XSensor(nSensorID, pInfo, nGateType)
, m_pQuestInfo(pQuestInfo)
, m_bPostEnter(false)
, m_bPostLeave(false)
{
	m_nOldState = gvar.MyInfo.Party.GetPartyState();
}

XQuestSensor::~XQuestSensor()
{

}

SENSOR_RESULT XQuestSensor::OnUpdate( bool bEnter, vec3 vPos )
{
	if(IsEnteredSensor(vPos))
	{
		if (!bEnter)
		{
			if (OnEnter(vPos))
			{
				return SENSOR_RESULT_ENTER;
			}
		}
		else
		{
			AUTOPARTY_STATE nState = gvar.MyInfo.Party.GetPartyState();
			if (m_nOldState != nState)
			{
				if ( nState == AUTOPARTY_LOOKUP)
				{
					PostEnter();
				}
				else
				{
					PostLeave();
				}

				m_nOldState = nState;
				return SENSOR_RESULT_NONE;
			}
		}
	}
	else
	{
		if (bEnter)
		{
			if (OnLeave(vPos))
			{
				return SENSOR_RESULT_LEAVE;
			}
		}
	}

	return SENSOR_RESULT_NONE;
}

bool XQuestSensor::OnEnter(vec3 vPos)
{
	_ASSERT(m_pQuestInfo);

	m_nOldState = gvar.MyInfo.Party.GetPartyState();

	if (XGETCFG_GAME_USE_AUTOPARTY == false)			return true;

	bool bHave = gvar.MyInfo.Quests.IsPlayerQuest(m_pQuestInfo->nID);

	if (bHave)
	{
		if ( gvar.MyInfo.Party.IsJoined() == false)
		{
			PostEnter();
		}
		else
		{
			if ( gvar.MyInfo.Party.GetLeader() == gvar.MyInfo.MyUID)
			{
				if ( m_pQuestInfo->nAutoPartyMemberLimit > gvar.MyInfo.Party.GetMemberCounter())
				{
					PostEnter();
				}
			}
		}
	}
	else
	{
		if (gvar.MyInfo.Party.GetPartyState() != AUTOPARTY_PAUSE)
		{
			if ( gvar.MyInfo.Party.GetLeader() == gvar.MyInfo.MyUID)
				XPostRequestAutoPartyChangeState(m_pQuestInfo->nID, AUTOPARTY_PAUSE);
			else
				gvar.MyInfo.Party.SetPartyState(m_pQuestInfo->nID, AUTOPARTY_PAUSE);
		}
	}

	Enter();

	gvar.MyInfo.Party.nQuestIDWithCurrentSensor = m_pQuestInfo->nID;

	return true;
}

void XQuestSensor::PostEnter()
{
	if (m_bPostEnter) return;

	_ASSERT(m_pQuestInfo);
	XPostRequestAutoPartyEnter(m_pQuestInfo->nID);

	m_bPostEnter = true;
	m_bPostLeave = false;
}

void XQuestSensor::Enter()
{
	//wstring strMsg;
	//strMsg = L"Äù½ºÆ®Áö¿ª: "+m_pQuestInfo->strTitle;
	//global.script->GetGlueGameEvent().OnGameEvent( "MAPSIGN", MLocale::ConvUTF16ToAnsi(strMsg.c_str()));

	global.script->GetGlueGameEvent().OnGameEvent( "AUTOPARTY", "ENTER", m_pQuestInfo->nID);
}

bool XQuestSensor::OnLeave(vec3 vPos)
{
	_ASSERT(m_pQuestInfo);

	Leave();

	if (XGETCFG_GAME_USE_AUTOPARTY == false) return true;

	if (gvar.MyInfo.Party.IsJoined() == false)
	{
		PostLeave();
		gvar.MyInfo.Party.SetPartyState(m_pQuestInfo->nID, AUTOPARTY_STANDBY);
	}
	else if (gvar.MyInfo.Party.GetLeader() == gvar.MyInfo.MyUID)
	{
		PostLeave();
	}

	gvar.MyInfo.Party.nQuestIDWithCurrentSensor = 0;

	return true;
}

void XQuestSensor::PostLeave()
{
	if(m_bPostLeave) return;

	_ASSERT(m_pQuestInfo);
	XPostRequestAutoPartyLeave(m_pQuestInfo->nID);

	m_bPostEnter = false;
	m_bPostLeave = true;
}

void XQuestSensor::Leave()
{
	global.script->GetGlueGameEvent().OnGameEvent( "AUTOPARTY", "LEAVE");
}
