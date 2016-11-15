#include "stdafx.h"
#include "XDummyBot_Party.h"
#include "XBirdDummyAgent.h"

XDummyBot_Party::XDummyBot_Party(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot_Roam(pAgent, pAgentInfo)
{
}

XDummyBot_Party::~XDummyBot_Party()
{
}

MCommandResult XDummyBot_Party::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	MCommandResult nResult = XDummyBot_Roam::OnCommand(pAgent, pCommand);
	if (nResult == CR_TRUE)	return CR_TRUE;

	XBirdClient* pClient = pAgent->GetClient();	
	switch(pCommand->GetID())
	{
	case MC_FIELD_IN_PLAYER:
		{	
			TD_UPDATE_CACHE_PLAYER* pPlayer;
			if (!pCommand->GetSingleBlob(pPlayer, 0))	return CR_ERROR;

			ENTITY_PLAYER player;

			player.uid = pPlayer->uid;
			player.nUIID = pPlayer->nUIID;
			player.vPos = pPlayer->vPos;

			m_mapPlayers.insert(make_pair(player.nUIID, player));
		}
		break;
	case MC_FIELD_OUT_PLAYER:
		{
			UIID nUIID;
			if (!pCommand->GetParameter(&nUIID, 0, MPT_USHORT))	return CR_ERROR;
			
			m_mapPlayers.erase(nUIID);
		}
		break;
	case MC_FIELD_SECTOR_ENTITY_INFO:
		{
			vector<TD_UPDATE_CACHE_PLAYER> vecPlayer;
			if (!pCommand->GetBlob(vecPlayer, 0))	return CR_ERROR;

			for(vector<TD_UPDATE_CACHE_PLAYER>::iterator it = vecPlayer.begin(); it != vecPlayer.end(); it++)
			{
				ENTITY_PLAYER player;

				player.uid = it->uid;
				player.nUIID = it->nUIID;
				player.vPos = it->vPos;

				m_mapPlayers.insert(make_pair(player.nUIID, player));
			}
		}
		break;
	case MC_ACTION_MOVE:
		{
			UIID nUIID;
			TD_PC_MOVE* pMove;

			if (!pCommand->GetParameter(&nUIID, 0, MPT_USHORT))	return CR_ERROR;
			if (!pCommand->GetSingleBlob(pMove, 1))	return CR_ERROR;

			map<UIID, ENTITY_PLAYER>::iterator it = m_mapPlayers.find(nUIID);
			if (it != m_mapPlayers.end())
			{
				it->second.vPos = pMove->vTarPos;
			}
		}
		break;
	case MC_PARTY_INVITE_QUESTION:
		{
			// 파티수락요청
			MUID uidInvitor;
			if (!pCommand->GetParameter(&uidInvitor, 0, MPT_UID))	return CR_ERROR;

			m_uidInvitor = uidInvitor;
		}
		break;
	case MC_PARTY_REFRESH_INFO_ALL:
		{		
			// 파티원들 정보 갱신
			TD_PARTY* pParty;
			int nCount;
			vector<TD_PARTY_MEMBER> members;

			if (!pCommand->GetBlob(pParty, nCount, 0))	return CR_ERROR;
			if (!pCommand->GetBlob(members, 1))			return CR_ERROR;


			m_uidParty = pParty->m_uidParty;
			m_uidLeader = pParty->m_partySetting.m_uidLeader;

			m_vecMemberUID.clear();
			for(vector<TD_PARTY_MEMBER>::iterator it = members.begin(); it != members.end(); it++)
			{
				m_vecMemberUID.push_back(it->m_uidPlayer);
			}
		}
		break;
	case MC_PARTY_CHANGE_LEADER:		
		{
			// 파티장 변경
			MUID uidNewLeader;
			if (!pCommand->GetParameter(&uidNewLeader, 0, MPT_UID))	return CR_ERROR;

			m_uidLeader= uidNewLeader;
		}
		break;
	case MC_PARTY_CHANGE_NAME:
		// 이름변경; 무시한다
		break;
	case MC_PARTY_CHANGE_LOOTING_RULE:
		// 루팅룰 변경; 무시한다
		break;
	case MC_PARTY_INVITE:
		// 파티장 초대 요청에 대한 응답; 무시한다
		break;
	case MC_PARTY_NOTIFY_JOIN:
		// 파티 참가; 무시한다
		break;
	case MC_PARTY_NOTIFY_LEAVE:
		{
			MUID uidLeaveMember;
			if (!pCommand->GetParameter(&uidLeaveMember, 0, MPT_UID))	return CR_ERROR;

			if (m_pAgent->GetUID() == uidLeaveMember)
			{
				m_uidParty.SetInvalid();
				m_vecMemberUID.clear();
			}
		}
		break;
	case MC_PARTY_REFRESH_INFO:
		// 파티원 정보 갱신; 무시한다
		break;
	case MC_PARTY_ACCEPT_CANCEL:
		// 수락 거절; 무시한다
		break;
	case MC_PARTY_INVITE_FOR_ME_QUESTION:		
		// 초대요청; 사용안함		
		break;
	default:
		return CR_FALSE;
	}

	return CR_TRUE;
}

void XDummyBot_Party::OnRun(float fDelta)
{
	//XDummyBot_Roam::OnRun(fDelta);

	// 행동
	vector<PARTY_BEHAVIOR> vecBehavior;
	
	GetAllowedBehavior(vecBehavior);
	PARTY_BEHAVIOR nSelectedBehavior = SelectBehavior(vecBehavior);
	DoBehavior(nSelectedBehavior);
}

void XDummyBot_Party::PostInviteReq()
{
	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_PARTY_INVITE_REQ, MCommandParameterUID(m_uidAdjacencyPlayer));
}

void XDummyBot_Party::PostLeaveReq()
{
	if (m_uidParty.IsInvalid())	return;

	XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_PARTY_LEAVE_REQ);
}

void XDummyBot_Party::PostKickReq()
{
	if (m_uidLeader != m_pAgent->GetUID())	return;
	if (m_vecMemberUID.empty())	return;

	// 무작위로 킥
	int nMemberCount = m_vecMemberUID.size();
	int nRandomIndex = MMath::RandomNumber(0, nMemberCount - 1);

	MUID uidTargetPlayer = m_vecMemberUID[nRandomIndex];
	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_PARTY_KICK_REQ, MCommandParameterUID(uidTargetPlayer));
}

void XDummyBot_Party::PostInviteQuestionRespond()
{	
	if (m_uidInvitor.IsInvalid())	return;

	m_uidInvitor.SetZero();

	// 수락여부 결정; 70%
	bool isAccept = MMath::RandomNumber(0, 100) > 30 ? true : false;
	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_PARTY_INVITE_QUESTION_RESPOND, MCommandParameterInt(isAccept));
}

void XDummyBot_Party::PostInviteForMeReq()
{
	// 초대요청; 사용안함
}

void XDummyBot_Party::PostInviteForMeQuestionRespond()
{
	// 초대요청 응답; 사용안함
}

void XDummyBot_Party::PostChangeNameReq()
{
	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_PARTY_CHANGE_NAME_REQ, MCommandParameterString("TestName"));
}

void XDummyBot_Party::PostChangeLeaderReq()
{
	if (m_vecMemberUID.empty())				return;

	int nMemberCount = m_vecMemberUID.size();
	int nRandomIndex = MMath::RandomNumber(0, nMemberCount - 1);
	
	MUID uidNewLeader = m_vecMemberUID[nRandomIndex];

	XBIRDPOSTCMD1(m_pAgent->GetClient(), MC_PARTY_CHANGE_LEADER_REQ, MCommandParameterUID(uidNewLeader));
}

void XDummyBot_Party::PostChangeLootingRuleReq()
{
	char nCommand = LRC_FREE_FOR_ALL;
	char nRare = LRR_FREE_FOR_ALL;
	char nRareFor = LRRF_RARE_OR_HIGHER;
	
	XBIRDPOSTCMD3(m_pAgent->GetClient(), MC_PARTY_CHANGE_LOOTING_RULE_REQ, MCommandParameterChar(nCommand), MCommandParameterChar(nRare), MCommandParameterChar(nRareFor));
}

void XDummyBot_Party::GetAllowedBehavior( vector<PARTY_BEHAVIOR>& vecBehavior )
{
	vecBehavior.push_back(PARTY_BEHAVIOR_IDLE);

	if (m_uidParty.IsInvalid())
	{
		// 파티없음; 인접 플레이어 확인
		if (BeAdjacencyPlayer())
		{
			vecBehavior.push_back(PARTY_BEHAVIOR_INVITE);
		}

		// 수락대기 중
		if (m_uidInvitor.IsValid())
		{
			vecBehavior.push_back(PARTY_BEHAVIOR_RESPOND);
		}
	}
	else
	{
		// 파티원
		vecBehavior.push_back(PARTY_BEHAVIOR_LEAVE);

		// 파티장
		if (m_uidLeader == m_pAgent->GetUID())
		{
			if (BeAdjacencyPlayer())
			{
				vecBehavior.push_back(PARTY_BEHAVIOR_INVITE);
			}

			vecBehavior.push_back(PARTY_BEHAVIOR_CHANGE_NAME);
			vecBehavior.push_back(PARTY_BEHAVIOR_CHANGE_LEADER);
			vecBehavior.push_back(PARTY_BEHAVIOR_CHANGE_LOOTING_RULE);
			vecBehavior.push_back(PARTY_BEHAVIOR_KICK);
		}			
	}
}

PARTY_BEHAVIOR XDummyBot_Party::SelectBehavior( vector<PARTY_BEHAVIOR>& vecBehavior )
{
	if (vecBehavior.empty())	return PARTY_BEHAVIOR_IDLE;

#if 0
	
	int nSelectedIndex = MMath::RandomNumber(0, vecBehavior.size() - 1);
	return vecBehavior[nSelectedIndex];

#else
	// 행동 빈도 결정
	int nFactor[MAX_PARTY_MEMBER_COUNT] = {1, 1, 2, 5, 10};

	vector<int> vecRange;	
	for each(PARTY_BEHAVIOR behavior in vecBehavior)
	{
		switch(behavior)
		{
		case PARTY_BEHAVIOR_IDLE:					vecRange.push_back(50);	break;
		case PARTY_BEHAVIOR_INVITE:					vecRange.push_back(20);	break;
		case PARTY_BEHAVIOR_RESPOND:				vecRange.push_back(20);	break;
		case PARTY_BEHAVIOR_LEAVE:					vecRange.push_back(nFactor[m_vecMemberUID.size()]);	break;
		case PARTY_BEHAVIOR_KICK:					vecRange.push_back(nFactor[m_vecMemberUID.size()]);	break;		
		case PARTY_BEHAVIOR_CHANGE_NAME:			vecRange.push_back(10);	break;
		case PARTY_BEHAVIOR_CHANGE_LEADER:			vecRange.push_back(10);	break;
		case PARTY_BEHAVIOR_CHANGE_LOOTING_RULE:	vecRange.push_back(10);	break;
		default:
			vecRange.push_back(0);
			break;
		}
	}

	// 빈도합; 난수결정
	int nSum = 0;
	for each(int nRange in vecRange)
	{
		nSum += nRange;
	}

	int nNumber = MMath::RandomNumber(0, nSum);

	// 해당 행동 찾기
	for(size_t i = 0; i < vecRange.size(); i++)
	{
		if (nNumber < vecRange[i])
		{
			return vecBehavior[i];
		}

		nNumber -= vecRange[i];
	}

	return PARTY_BEHAVIOR_IDLE;
#endif
}

void XDummyBot_Party::DoBehavior( PARTY_BEHAVIOR nBehavior )
{
	switch(nBehavior)
	{
	case PARTY_BEHAVIOR_IDLE:
		// do nothing
		break;
	case PARTY_BEHAVIOR_INVITE:
		PostInviteReq();
		break;
	case PARTY_BEHAVIOR_RESPOND:
		PostInviteQuestionRespond();
		break;
	case PARTY_BEHAVIOR_LEAVE:
		PostLeaveReq();
		break;
	case PARTY_BEHAVIOR_KICK:
		PostKickReq();
		break;
	case PARTY_BEHAVIOR_CHANGE_NAME:
		PostChangeNameReq();
		break;
	case PARTY_BEHAVIOR_CHANGE_LEADER:
		PostChangeLeaderReq();
		break;
	case PARTY_BEHAVIOR_CHANGE_LOOTING_RULE:
		PostChangeLootingRuleReq();
		break;
	default:
		// do nothing
		break;
	}
}

bool XDummyBot_Party::BeAdjacencyPlayer()
{
	m_uidAdjacencyPlayer.SetZero();

	for(map<UIID, ENTITY_PLAYER>::iterator it = m_mapPlayers.begin(); it != m_mapPlayers.end(); it++)
	{
		// 인터랙션 거리 안에 있는지 확인
		vec3 vPos = it->second.vPos;
		float fDistance = m_vPosition.XYDistanceTo(vPos);
		
		if (fDistance < CHECK_INTERACTION_DISTANCE)
		{
			// 파티원은 무시한다.
			if (m_vecMemberUID.end() == find(m_vecMemberUID.begin(), m_vecMemberUID.end(), it->second.uid))
			{
				m_uidAdjacencyPlayer = it->second.uid;
				return true;
			}
		}
	}

	return false;
}
