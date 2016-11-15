#include "stdafx.h"
#include "ZPartyLogic.h"
#include "ZPlayerManager.h"
#include "ZPartyCommandRouter.h"
#include "ZGameServerObjectManager.h"
#include "ZGameServerObject.h"
#include "ZPartyManager.h"
#include "ZParty.h"
#include "ZDynamicFieldMaster.h"
#include "STransData_M2G.h"
#include "ZPartyMember.h"
#include "ZPlayerCommandRouter.h"
#include "CSDef_Party.h"
#include "ZFixedPartyLogic.h"
#include "ZConfig.h"

ZPartyLogic::ZPartyLogic()
{
	m_pRouter = NewRouter();
	m_pFixedPartyLogic = new ZFixedPartyLogic();
}

ZPartyLogic::~ZPartyLogic()
{
	SAFE_DELETE(m_pRouter);
	SAFE_DELETE(m_pFixedPartyLogic);
}

ZPartyCommandRouter* ZPartyLogic::NewRouter(void)
{
	return new ZPartyCommandRouter();
}

void ZPartyLogic::InviteReq(MUID uidSender, MUID uidTargetPlayer, MUID uidRequestPlayer)
{
	// 대상 확인
	ZPlayer* pTargetPlayer = gmgr.pPlayerManager->FindByUID(uidTargetPlayer);
	if (pTargetPlayer == NULL)
	{
		m_pRouter->InviteRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}


	// 대상 서버 확인
	int nServerID = pTargetPlayer->GetGameServerID();
	ZGameServerObject* pTargetGameServer = gmgr.pGameServerObjectManager->FindGameServer(nServerID);
	if (pTargetGameServer == NULL)
	{
		m_pRouter->InviteRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}
	

	// 대상 파티 확인
	MUID uidTargetParty = pTargetPlayer->GetPartyUID();
	if (uidTargetParty.IsValid())
	{
		ZParty* pParty = gmgr.pPartyManager->Find(uidTargetParty);
		if (pParty != NULL)
		{
			m_pRouter->InviteRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
			return;
		}
	}
	

	// 요청자 확인
	ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pRequestPlayer == NULL)
	{
		m_pRouter->InviteRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}
	

	// 요청자 파티 확인
	MUID uidRequestParty = pRequestPlayer->GetPartyUID();
	if (uidRequestParty.IsValid())
	{
		ZParty* pParty = gmgr.pPartyManager->Find(uidRequestParty);
		if (pParty != NULL)
		{
			if (uidRequestPlayer != pParty->GetLeader())
			{
				m_pRouter->InviteRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_NOT_PARTY_LEADER);
				return;
			}

			if (pParty->IsFull())
			{
				m_pRouter->InviteRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_FULL_PARTY);
				return;
			}
		}		
	}


	// 초대 수락 요청
	MUID uidTarget = pTargetGameServer->GetUID();
	m_pRouter->AcceptReq(uidTarget, uidTargetPlayer, uidRequestPlayer, pRequestPlayer->GetPlayerName());
}

void ZPartyLogic::AcceptRes(MUID uidSender, MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	// 요청자 확인
	ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pRequestPlayer == NULL)
	{
		m_pRouter->AcceptCancel(uidSender, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}


	// 요청자 서버 확인
	int nServerID = pRequestPlayer->GetGameServerID();
	ZGameServerObject* pRequestGameServer = gmgr.pGameServerObjectManager->FindGameServer(nServerID);
	if (pRequestGameServer == NULL)
	{
		m_pRouter->AcceptCancel(uidSender, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}
	

	// 요청자 파티 확인
	bool isExistParty = false;
	MUID uidRequestParty = pRequestPlayer->GetPartyUID();	
	if (uidRequestParty.IsValid())
	{
		ZParty* pParty = gmgr.pPartyManager->Find(uidRequestParty);
		if (pParty != NULL)
		{
			isExistParty = true;

			if (uidRequestPlayer != pParty->GetLeader())
			{
				MUID uidTarget = pRequestGameServer->GetUID();
				m_pRouter->InviteRes(uidTarget, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_NOT_PARTY_LEADER);
				m_pRouter->AcceptCancel(uidSender, uidTargetPlayer, CR_FAIL_PARTY_NOT_PARTY_LEADER);
				return;
			}

			if (pParty->IsFull())
			{
				MUID uidTarget = pRequestGameServer->GetUID();
				m_pRouter->InviteRes(uidTarget, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_FULL_PARTY);
				m_pRouter->AcceptCancel(uidSender, uidTargetPlayer, CR_FAIL_PARTY_FULL_PARTY);
				return;
			}
		}
	}


	// 대상 확인
	ZPlayer* pTargetPlayer = gmgr.pPlayerManager->FindByUID(uidTargetPlayer);
	if (pTargetPlayer == NULL)
	{
		MUID uidTarget = pRequestGameServer->GetUID();
		m_pRouter->InviteRes(uidTarget, uidRequestPlayer, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		m_pRouter->AcceptCancel(uidSender, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);				
		return;
	}

	
	// 대상 파티 확인
	MUID uidTargetParty = pTargetPlayer->GetPartyUID();	
	if (uidTargetParty.IsValid())
	{
		ZParty* pParty = gmgr.pPartyManager->Find(uidTargetParty);
		if (pParty != NULL)
		{
			MUID uidTarget = pRequestGameServer->GetUID();
			m_pRouter->InviteRes(uidTarget, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
			m_pRouter->AcceptCancel(uidSender, uidTargetPlayer, CR_FAIL_PARTY_ALEADY_HAS_PARTY);
			return;
		}
	}


	// 초대 수락 응답 확인
	if (nResult != CR_SUCCESS)
	{
		MUID uidTarget = pRequestGameServer->GetUID();
		m_pRouter->InviteRes(uidTarget, uidRequestPlayer, uidTargetPlayer, nResult);
		return;
	}


	// 파티 생성	
	MUID uidParty = uidRequestParty;
	ZParty* pParty = NULL;

	if (isExistParty)
	{
		pParty = gmgr.pPartyManager->Find(uidParty);
		_ASSERT(pParty != NULL);
	}
	else
	{
		int nLeaderServerID = pRequestPlayer->GetGameServerID();
		pParty = gmgr.pPartyManager->AddParty(uidRequestPlayer, pRequestPlayer->GetPlayerName(), nLeaderServerID, pRequestPlayer->GetCID());
		_ASSERT(pParty != NULL);

		uidParty = pParty->GetUID();
		pRequestPlayer->SetPartyUID(uidParty);
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pRequestPlayer);
	}	

	int nMemberServerID = pTargetPlayer->GetGameServerID();
	pParty->AddMember(uidTargetPlayer, pTargetPlayer->GetPlayerName(), nMemberServerID, pTargetPlayer->GetCID());
	pTargetPlayer->SetPartyUID(uidParty);
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pTargetPlayer);


	// 초대 수락 응답
	MUID uidTarget = pRequestGameServer->GetUID();
	m_pRouter->InviteRes(uidTarget, uidRequestPlayer, uidTargetPlayer, nResult);


	// 파티 정보 동기화	
	if (!isExistParty)
	{
		m_pRouter->PartyAdd(uidParty, uidRequestPlayer, pRequestPlayer->GetPlayerName(), pRequestPlayer->GetCID());
	}

	m_pRouter->AddMember(uidParty, uidTargetPlayer, pTargetPlayer->GetPlayerName(), pTargetPlayer->GetCID());	
}

void ZPartyLogic::LeaveReq(MUID uidSender, MUID uidParty, MUID uidLeaveMember)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->LeaveRes(uidSender, uidLeaveMember, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 파티원 확인
	if (!pParty->IsExistMember(uidLeaveMember))
	{
		m_pRouter->LeaveRes(uidSender, uidLeaveMember, CR_FAIL_PARTY_NOT_IN_PARTY);
		return;
	}


	// 퇴장자 확인
	ZPlayer* pLeavePlayer = gmgr.pPlayerManager->FindByUID(uidLeaveMember);
	if (pLeavePlayer == NULL)
	{
		m_pRouter->LeaveRes(uidSender, uidLeaveMember, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}
	

	// 마지막 온라인 파티원
	if (uidLeaveMember == pParty->GetAloneMember())
	{
		gmgr.pPartyManager->RemoveParty(uidParty);
		pLeavePlayer->SetPartyUID(MUID::Invalid());
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pLeavePlayer);
		m_pRouter->LeaveRes(uidSender, uidLeaveMember, CR_SUCCESS);
		m_pRouter->RemoveMember(uidParty, uidLeaveMember);
		m_pRouter->PartyRemove(uidParty);		
		return;
	}


	// 파티장의 경우 파티장 위임
	if (uidLeaveMember == pParty->GetLeader())
	{
		MUID uidNewLeader = pParty->FindCandidateForLeader();
		pParty->ChangeLeader(uidNewLeader);

		m_pRouter->ChangeLeaderRes(uidParty, uidNewLeader);
	}

	// 탈퇴
	pParty->RemoveMember(uidLeaveMember);

	pLeavePlayer->SetPartyUID(MUID::Invalid());	
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pLeavePlayer);
	m_pRouter->LeaveRes(uidSender, uidLeaveMember, CR_SUCCESS);
	m_pRouter->RemoveMember(uidParty, uidLeaveMember);


	// 파티원이 혼자 남으면, 추가 탈퇴
	if (1 == pParty->GetCount())
	{
		MUID uidAloneMember = pParty->GetAloneMember();
		gmgr.pPartyManager->RemoveParty(uidParty);

		ZPlayer* pAloneMember = gmgr.pPlayerManager->FindByUID(uidAloneMember);
		if (pAloneMember != NULL)
		{
			pAloneMember->SetPartyUID(MUID::Invalid());
			ZPlayerCommandRouter::RouteProxyPlayerUpdate(pAloneMember);
		}

		m_pRouter->RemoveMember(uidParty, uidAloneMember);
		m_pRouter->PartyRemove(uidParty);
		return;
	}


	// 파티 동기화	
	m_pRouter->EraseQuestRunnerSync(uidParty, uidLeaveMember);
	RouteRemoveMemberSync(pParty);
}

void ZPartyLogic::KickReq(MUID uidSender, MUID uidParty, MUID uidRequestPlayer, MUID uidTargetPlayer)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->KickRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 요청자 확인
	if (!pParty->IsExistMember(uidRequestPlayer))
	{
		m_pRouter->KickRes(uidSender, uidRequestPlayer, uidTargetPlayer,CR_FAIL_PARTY_NOT_IN_PARTY);
		return;
	}


	// 요청자 파티장 권한 확인
	if (uidRequestPlayer != pParty->GetLeader())
	{
		m_pRouter->KickRes(uidSender, uidRequestPlayer, uidTargetPlayer,CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}


	// 자기자신 강퇴 금지
	if (uidRequestPlayer == uidTargetPlayer)
	{
		m_pRouter->KickRes(uidSender, uidRequestPlayer, uidTargetPlayer,CR_FAIL_PARTY_KICK_SELF);
		return;
	}


	// 대상 파티원 확인
	if (!pParty->IsExistMember(uidTargetPlayer))
	{
		m_pRouter->KickRes(uidSender, uidRequestPlayer, uidTargetPlayer,CR_FAIL_PARTY_TARGET_NOT_IN_PARTY);
		return;
	}


	// 대상이 오프라인 아니라면 Party 정보 초기화
	ZPlayer* pTargetPlayer = gmgr.pPlayerManager->FindByUID(uidTargetPlayer);
	if (pTargetPlayer != NULL)
	{
		pTargetPlayer->SetPartyUID(MUID::Invalid());
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pTargetPlayer);
	}


	// 강퇴
	pParty->RemoveMember(uidTargetPlayer);

	m_pRouter->KickRes(uidSender, uidRequestPlayer, uidTargetPlayer, CR_SUCCESS);
	m_pRouter->RemoveMember(uidParty, uidTargetPlayer);


	// 파티원이 혼자 남으면, 추가 탈퇴
	if (1 == pParty->GetCount())
	{
		MUID uidAloneMember = pParty->GetAloneMember();		
		gmgr.pPartyManager->RemoveParty(uidParty);

		ZPlayer* pAloneMember = gmgr.pPlayerManager->FindByUID(uidAloneMember);
		if (pAloneMember != NULL)
		{
			pAloneMember->SetPartyUID(MUID::Invalid());
			ZPlayerCommandRouter::RouteProxyPlayerUpdate(pAloneMember);
		}

		m_pRouter->RemoveMember(uidParty, uidAloneMember);
		m_pRouter->PartyRemove(uidParty);
		return;
	}

	// 파티 동기화
	m_pRouter->EraseQuestRunnerSync(uidParty, uidTargetPlayer);
	RouteRemoveMemberSync(pParty);
}

void ZPartyLogic::JoinInviteReq(MUID uidSender, MUID uidParty, MUID uidRequestPlayer)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->JoinInviteRes(uidSender, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 풀파티 확인
	if (pParty->IsFull())
	{
		m_pRouter->JoinInviteRes(uidSender, uidRequestPlayer, CR_FAIL_PARTY_FULL_PARTY);
		return;
	}


	// 파티원 확인
	if (pParty->IsExistMember(uidRequestPlayer))
	{
		m_pRouter->JoinInviteRes(uidSender, uidRequestPlayer, CR_FAIL_PARTY_ALEADY_HAS_PARTY);
		return;
	}

	
	// 파티장 확인
	MUID uidLeader = pParty->GetLeader();
	ZPlayer* pLeader = gmgr.pPlayerManager->FindByUID(uidLeader);
	if (pLeader == NULL)
	{
		m_pRouter->JoinInviteRes(uidSender, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 파티장 서버 확인
	int nServerID = pLeader->GetGameServerID();
	ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(nServerID);
	if (pGameServer == NULL)
	{
		m_pRouter->JoinInviteRes(uidSender, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 요청자 확인
	ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pRequestPlayer == NULL)
	{
		m_pRouter->JoinInviteRes(uidSender, uidRequestPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}
	

	// 가입 요청
	MUID uidTarget = pGameServer->GetUID();
	wstring strRequestPlayerName = pRequestPlayer->GetPlayerName();
	m_pRouter->JoinAcceptReq(uidTarget, uidParty, uidLeader, uidRequestPlayer, strRequestPlayerName);
}

void ZPartyLogic::JoinAcceptRes(MUID uidSender, MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, CCommandResultTable nResult)
{
	// 요청자 확인
	ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pRequestPlayer == NULL)
	{
		m_pRouter->JoinAcceptCancel(uidSender, uidLeader, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}

	
	// 요청자 서버 확인
	int nServerID = pRequestPlayer->GetGameServerID();	
	ZGameServerObject* pGameServer = gmgr.pGameServerObjectManager->FindGameServer(nServerID);
	if (pGameServer == NULL)
	{
		m_pRouter->JoinAcceptCancel(uidSender, uidLeader, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}

	
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		MUID uidTarget = pGameServer->GetUID();
		m_pRouter->JoinInviteRes(uidTarget, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		m_pRouter->JoinAcceptCancel(uidSender, uidLeader, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}

	
	// 파티장 확인
	if (uidLeader != pParty->GetLeader())
	{
		MUID uidTarget = pGameServer->GetUID();
		m_pRouter->JoinInviteRes(uidTarget, uidRequestPlayer, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		m_pRouter->JoinAcceptCancel(uidSender, uidLeader, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}


	// 풀파티 확인
	if (pParty->IsFull())
	{
		MUID uidTarget = pGameServer->GetUID();
		m_pRouter->JoinInviteRes(uidTarget, uidRequestPlayer, CR_FAIL_PARTY_FULL_PARTY);
		m_pRouter->JoinAcceptCancel(uidSender, uidLeader, CR_FAIL_PARTY_FULL_PARTY);
		return;
	}


	// 응답 확인
	if (nResult != CR_SUCCESS)
	{
		MUID uidTarget = pGameServer->GetUID();
		m_pRouter->JoinInviteRes(uidTarget, uidRequestPlayer, nResult);
		return;
	}


	// 가입 처리
	int nMemberServerID = pRequestPlayer->GetGameServerID();
	pParty->AddMember(uidRequestPlayer, pRequestPlayer->GetPlayerName(), nMemberServerID, pRequestPlayer->GetCID());
	pRequestPlayer->SetPartyUID(uidParty);
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pRequestPlayer);


	// 수락 응답
	MUID uidTarget = pGameServer->GetUID();
	m_pRouter->JoinInviteRes(uidTarget, uidRequestPlayer, nResult);


	// 파티 정보 동기화	
	m_pRouter->AddMember(uidParty, uidRequestPlayer, pRequestPlayer->GetPlayerName(), pRequestPlayer->GetCID());
}

void ZPartyLogic::CreateSingleReq(MUID uidSender, MUID uidRequestPlayer)
{
	// 요청자 확인
	ZPlayer* pRequestPlayer = gmgr.pPlayerManager->FindByUID(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;


	// 파티 확인
	MUID uidParty = pRequestPlayer->GetPartyUID();
	if (uidParty.IsValid())		return;


	// 파티 생성
	int nLeaderServerID = pRequestPlayer->GetGameServerID();
	ZParty* pParty = gmgr.pPartyManager->AddParty(uidRequestPlayer, pRequestPlayer->GetPlayerName(), nLeaderServerID, pRequestPlayer->GetCID());
	_ASSERT(pParty != NULL);

	uidParty = pParty->GetUID();
	pRequestPlayer->SetPartyUID(uidParty);
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pRequestPlayer);


	// 파티 정보 동기화	
	m_pRouter->PartyAdd(uidParty, uidRequestPlayer, pRequestPlayer->GetPlayerName(), pRequestPlayer->GetCID());	
}

void ZPartyLogic::PartyInfoAllReq(MUID uidTargetServer)
{
	// 전체 파티정보 수집
	vector<TD_PARTY_INFO> vecPartyInfo;

	const ZPartyManager::ZPartyMap& mapPartys = gmgr.pPartyManager->GetPartyMap();
	for each(ZPartyManager::ZPartyMap::value_type party in mapPartys)
	{
		ZParty* pParty = party.second;
		TD_PARTY_INFO tdPartyInfo;
		
		tdPartyInfo.uidParty = pParty->GetUID();		
		tdPartyInfo.nLeaderIndex = 0;

		int nIndex = 0;
		ZParty::member_iterator it = pParty->BeginMember();
		while(it != pParty->EndMember())
		{
			MUID uidMember = it->first;
			ZPartyMember* pPartyMember = it->second;

			TD_PARTY_INFO::MEMBER_INFO& rMemberInfo = tdPartyInfo.members[nIndex];
			rMemberInfo.uidMember = uidMember;
			rMemberInfo.nMemberCID = pPartyMember->GetCID();

			wcsncpy_s(rMemberInfo.szMemberName, pPartyMember->GetName().c_str(), _TRUNCATE);			
			
			if (uidMember == pParty->GetLeader())
			{
				tdPartyInfo.nLeaderIndex = nIndex;
			}

			nIndex++;
			it++;			
		}

		tdPartyInfo.nMemberCount = nIndex;

		vecPartyInfo.push_back(tdPartyInfo);
	}


	// 파티정보 전달
	m_pRouter->PartyInfoAllRes(uidTargetServer, vecPartyInfo);
}

void ZPartyLogic::AllMemberOfflineInGameServer(int nGameServerID)
{
	const ZPartyManager::ZPartyMap& mapPartys = gmgr.pPartyManager->GetPartyMap();

	ZPartyManager::ZPartyMap::const_iterator it = mapPartys.begin();
	while(it != mapPartys.end())
	{
		// RemoveParty에서 현재 이터레이터가 제거될 수 있기때문에, 값을 받아놓고 미리 증가시킨다.
		MUID uidParty = it->first;
		ZParty* pParty = it->second;
		it++;

		
		// 모든 온라인 멤버가 해당 서버에 있다면, 파티 제거	
		if (pParty->IsExistAllOnlineMembersIn(nGameServerID))
		{			
			gmgr.pPartyManager->RemoveParty(uidParty);
			m_pRouter->PartyRemove(uidParty);
		}
		else
		{
			bool beOffline = false;

			// 파티장이 해당 서버에 있다면, 다른 서버에 있는 파티원에게 파티장 위임
			MUID uidLeader = pParty->GetLeader();
			ZPartyMember* pLeaderMember = pParty->GetMember(uidLeader);
			_ASSERT(pLeaderMember != NULL);
						
			if (nGameServerID == pLeaderMember->GetGameServerID())
			{
				MUID uidNewLeader = pParty->FindCandidateForLeaderOtherServer();
				pParty->ChangeLeader(uidNewLeader);
				m_pRouter->ChangeLeaderRes(uidParty, uidNewLeader);
			}


			// 해당 서버에 있는 모든 파티원 오프라인
			ZParty::member_iterator it = pParty->BeginMember();
			while(it != pParty->EndMember())
			{
				MUID uidMember = it->first;
				ZPartyMember* pPartyMember = it->second;

				if (nGameServerID == pPartyMember->GetGameServerID())
				{
					pPartyMember->SetMoveServer(false);
					pPartyMember->SetOffline(true);
					pPartyMember->ClearFieldGroup();
					pPartyMember->ClearQuest();

					m_pRouter->AddOfflineMember(uidParty, uidMember);
					m_pRouter->EraseQuestRunnerSync(uidParty, uidMember);
					
					beOffline = true;
				}

				it++;
			}


			// 파티정보 동기화
			if (beOffline)
			{
				RouteRemoveMemberSync(pParty);
			}
		}
	}
}

void ZPartyLogic::UpdateParty(MUID uidParty, const ZPartyMember* pMember)
{
	_ASSERT(pMember != NULL);

	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)						return;


	// 파티원 확인
	MUID uidMember = pMember->GetUID();
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)				return;


	// 파티원 정보 갱신
	pPartyMember->Update(pMember);


	// 파티 정보 동기화 - 첫 싱크는 모든 멤버 정보가 업데이트되기 전엔 보내지 않는다.
	if (pParty->IsSyncReady())
	{
		m_pRouter->PartySync(pParty);
	}
}

void ZPartyLogic::UpdateMember(MUID uidParty, const ZPartyMember* pMember)
{
	_ASSERT(pMember != NULL);

	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)						return;


	// 파티원 확인
	MUID uidMember = pMember->GetUID();
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)				return;


	// 파티원 정보 갱신
	pPartyMember->Update(pMember);


	// 파티원 정보 동기화
	m_pRouter->MemberSync(uidParty, pMember);
}

void ZPartyLogic::AddQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)						return;

	
	// 파티원 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)				return;


	// 퀘스트 추가
	for each(int nQuestID in vecQuestID)
	{
		pPartyMember->AddQuest(nQuestID);
	}	


	// 퀘스트 동기화
	vector<TD_PARTY_QUEST> vecPartyQuest;
	MakePartyQuest(pParty, vecPartyQuest);
	m_pRouter->QuestSync(uidParty, vecPartyQuest);
}

void ZPartyLogic::AddField(MUID uidParty, MUID uidMember, MUID uidFieldGroup)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)		return;


	// 파티원 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)	return;


	// 필드 확인
	ZDynamicFieldMaster* pFieldGroup = gmgr.pFieldManager->FindDynamicFieldMaster(uidFieldGroup);
	if (pFieldGroup == NULL)	return;


	// 필드 추가
	int nFieldGroupID = pFieldGroup->GetInfo()->nID;
	pPartyMember->SetFieldGroup(nFieldGroupID, uidFieldGroup);


	// 필드 동기화
	vector<TD_PARTY_FIELD> vecPartyField;
	MakePartyField(pParty, vecPartyField);	
	m_pRouter->FieldSync(uidParty, vecPartyField);
}

void ZPartyLogic::UpdateQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)						return;


	// 멤버확인
	if (!pParty->IsExistMember(uidMember))		return;


	// 퀘스트 러너 동기화
	m_pRouter->QuestRunnerSync(uidParty, uidMember, uidField, vecQuestID);
}

void ZPartyLogic::RemoveQuest(MUID uidParty, MUID uidMember, const vector<int>& vecQuestID)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)						return;


	// 파티원 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)				return;
	

	// 퀘스트 제거
	for each(int nQuestID in vecQuestID)
	{
		pPartyMember->RemoveQuest(nQuestID);
	}


	// 퀘스트 동기화
	vector<TD_PARTY_QUEST> vecPartyQuest;
	MakePartyQuest(pParty, vecPartyQuest);
	m_pRouter->QuestSync(uidParty, vecPartyQuest);
}

void ZPartyLogic::RemoveField(MUID uidParty, MUID uidMember, int nFieldGroupID)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)				return;

	
	// 파티원 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)		return;
	
	
	// 필드 제거
	pPartyMember->ClearFieldGroup();


	// 필드 동기화
	vector<TD_PARTY_FIELD> vecPartyField;
	MakePartyField(pParty, vecPartyField);	
	m_pRouter->FieldSync(uidParty, vecPartyField);
}

void ZPartyLogic::RemoveFieldSelf(MUID uidParty, int nFieldGroupID)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)		return;

	
	// 필드 제거
	pParty->RemoveFieldSelf(nFieldGroupID);


	// 필드 동기화
	vector<TD_PARTY_FIELD> vecPartyField;
	MakePartyField(pParty, vecPartyField);	
	m_pRouter->FieldSync(uidParty, vecPartyField);
}

void ZPartyLogic::EraseQuestRunner(MUID uidParty, MUID uidMember)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)						return;


	// 파티원 확인
	if (!pParty->IsExistMember(uidMember))		return;


	// 퀘스트 러너 동기화
	m_pRouter->EraseQuestRunnerSync(uidParty, uidMember);
}

void ZPartyLogic::DoOffline(MUID uidParty, MUID uidMember)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)				return;


	// 파티원 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)		return;


	// 파티장의 경우 파티장 위임
	if (uidMember == pParty->GetLeader())
	{
		MUID uidNewLeader = pParty->FindCandidateForLeader();
		if (uidNewLeader.IsValid())
		{
			pParty->ChangeLeader(uidNewLeader);
			m_pRouter->ChangeLeaderRes(uidParty, uidNewLeader);
		}
	}

	// 오프라인 파티원 추가
	pPartyMember->SetOffline(true);
	pPartyMember->ClearFieldGroup();
	pPartyMember->ClearQuest();

	m_pRouter->AddOfflineMember(uidParty, uidMember);
	

	// 온라인 파티원이 없으면 파티 해제
	if (pParty->IsEmpty())
	{
		// 고정파티는 전원 오프라인되어서 삭제되지 않는다.
		gmgr.pPartyManager->RemoveParty(uidParty);
		m_pRouter->PartyRemove(uidParty);
		return;
	}


	// 파티 동기화
	m_pRouter->EraseQuestRunnerSync(uidParty, uidMember);
	RouteRemoveMemberSync(pParty);


	// 고정파티 지스타 경우 그냥 파티에서 제거	
	if (ZConfig::IsExpoMode())
	{
		wstring strName = pPartyMember->GetName();
		int nFixedPartyID = m_pFixedPartyLogic->GetFixedPartyID(strName);

		if (nFixedPartyID != INVALID_ID)
		{
			// 멤버 제거
			pParty->RemoveMember(uidMember);
			m_pRouter->RemoveMember(uidParty, uidMember);

			// 파티 동기화
			m_pRouter->EraseQuestRunnerSync(uidParty, uidMember);
			RouteRemoveMemberSync(pParty);
		}
	}
}

bool ZPartyLogic::DoOnline(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	// 파티 찾기
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)				return false;


	// 파티 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidOffline);
	if (pPartyMember == NULL)		return false;


	// 파티원 확인
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidMember);
	if (pPlayer == NULL)			return false;


	// 서버이동 상태 확인
	if (pPartyMember->IsMoveServer())
	{
		// 정보교체
		int nGameServerID = pPlayer->GetGameServerID();
		pPartyMember->SetGameServerID(nGameServerID);

		pPartyMember->SetMoveServer(false);
		pParty->ChangeMemberUID(uidOffline, uidMember);

		pPlayer->SetPartyUID(uidParty);
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);


		// 서버이동 완료 상태 동기화
		m_pRouter->MoveServerSync(uidParty, uidMember, uidOffline);
	}
	else
	{
		// 오프라인 파티원정보 온라인 정보로 교체
		pPartyMember->SetOffline(false);

		int nGameServerID = pPlayer->GetGameServerID();
		pPartyMember->SetGameServerID(nGameServerID);

		pParty->ChangeMemberUID(uidOffline, uidMember);

		pPlayer->SetPartyUID(uidParty);
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
		

		// 파티원 온라인 동기화
		m_pRouter->RemoveOfflineMember(uidParty, uidMember, uidOffline);
	}

	return true;
}

void ZPartyLogic::MoveServer(MUID uidParty, MUID uidMember)
{
	// 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)				return;


	// 파티원 확인
	ZPartyMember* pPartyMember = pParty->GetMember(uidMember);
	if (pPartyMember == NULL)		return;


	// 서버이동 추가
	pPartyMember->SetMoveServer(true);
}

void ZPartyLogic::MakePartyField(const ZParty* pParty, vector<TD_PARTY_FIELD>& vecPartyField)
{
	_ASSERT(pParty != NULL);

	ZParty::member_iterator it = pParty->BeginMember();
	while(it != pParty->EndMember())
	{
		ZPartyMember* pPartyMember = it->second;

		if (!pPartyMember->IsEmptyFieldGroup())
		{
			TD_PARTY_FIELD tdPartyField;
			tdPartyField.nFieldGroupID = pPartyMember->GetFieldGroupID();
			tdPartyField.uidFieldGroup = pPartyMember->GetFieldGroupUID();

			vecPartyField.push_back(tdPartyField);
		}

		it++;
	}
}

void ZPartyLogic::MakePartyQuest(const ZParty* pParty, vector<TD_PARTY_QUEST>& vecPartyQuest)
{
	_ASSERT(pParty != NULL);
	
	map<int, int> mapQuestID;	// nQuestID, nQuestCount;
	
	// 멤버 퀘스트 수집
	ZParty::member_iterator it = pParty->BeginMember();
	while(it != pParty->EndMember())
	{
		ZPartyMember* pPartyMember = it->second;
		MemberQuestCounting(pPartyMember, mapQuestID);

		it++;
	}

	// 전송 객체로 패킹
	for each(map<int, int>::value_type quest in mapQuestID)
	{
		TD_PARTY_QUEST tdPartyQuest;
		tdPartyQuest.nQuestID = quest.first;
		tdPartyQuest.nQuestCount = quest.second;
		
		vecPartyQuest.push_back(tdPartyQuest);
	}
}

void ZPartyLogic::MemberQuestCounting(const ZPartyMember* pPartyMember, map<int, int>& mapQuestID)
{
	_ASSERT(pPartyMember != NULL);

	ZPartyMember::quest_iterator it = pPartyMember->BeginQuest();
	while(it != pPartyMember->EndQuest())
	{
		int nQuestID = *it;
		mapQuestID[nQuestID]++;

		it++;
	}
}

void ZPartyLogic::RouteRemoveMemberSync(const ZParty* pParty)
{
	_ASSERT(pParty != NULL);
	
	MUID uidParty = pParty->GetUID();

	vector<TD_PARTY_FIELD> vecPartyField;
	MakePartyField(pParty, vecPartyField);	
	m_pRouter->FieldSync(uidParty, vecPartyField);

	vector<TD_PARTY_QUEST> vecPartyQuest;
	MakePartyQuest(pParty, vecPartyQuest);
	m_pRouter->QuestSync(uidParty, vecPartyQuest);

	m_pRouter->PartySync(pParty);
}

void ZPartyLogic::ChangeNameReq( MUID uidSender, MUID uidParty, MUID uidLeader, wstring strName )
{
	// 파티 찾기
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}

	// 파티장 권한 확인
	if (uidLeader != pParty->GetLeader())
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}
	
	// 이름 변경
	pParty->SetName(strName);

	// 통보
	m_pRouter->ChangeNameRes(uidParty, strName);
}

void ZPartyLogic::ChangeLeaderReq( MUID uidSender, MUID uidParty, MUID uidLeader, MUID uidNewLeader )
{
	// 파티 찾기
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}

	// 파티장 권한 확인
	if (uidLeader != pParty->GetLeader())
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	// 다음 파티장 대상 찾기
	ZPartyMember* pNewLeader = pParty->GetMember(uidNewLeader);
	if (pNewLeader == NULL || pNewLeader->IsOffline())
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}

	// 파티장 변경
	pParty->ChangeLeader(uidNewLeader);

	// 통보
	m_pRouter->ChangeLeaderRes(uidParty, uidNewLeader);
}

void ZPartyLogic::ChangeLootingRuleReq( MUID uidSender, MUID uidParty, MUID uidLeader, LOOTING_RULE_DATA rule )
{
	// 파티 찾기
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}

	// 파티장 권한 확인
	if (uidLeader != pParty->GetLeader())
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	// 루팅룰 변경
	pParty->SetLootingRule(rule);

	// 루팅룰 변경 통보
	m_pRouter->ChangeLootingRuleRes(uidParty, rule);
}

void ZPartyLogic::ChangeQuestIDReq( MUID uidSender, MUID uidParty, MUID uidLeader, int nQuestID )
{
	// 파티 찾기
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);
	if (pParty == NULL)
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}

	// 파티장 권한 확인
	if (uidLeader != pParty->GetLeader())
	{
		m_pRouter->Fail(uidSender, uidLeader, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	// 루팅룰 변경
	pParty->SetAutoPartyQuestID(nQuestID);

	// 루팅룰 변경 통보
	m_pRouter->ChangeQuestIDRes(uidParty, nQuestID);
}

bool ZPartyLogic::FixedPartyLogOn(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	// 로그온 유저 확인
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidMember);

	if (pPlayer == NULL)
	{
		return false;
	}


	// 고정 파티 멤버의 경우, 현재 파티에서 자신의 기록이 남아있는지 확인한다.
	wstring strName = pPlayer->GetPlayerName();
	int nFixedPartyID = m_pFixedPartyLogic->GetFixedPartyID(strName);

	if (nFixedPartyID != INVALID_ID)
	{
		// 이전 파티에 등록된 UID를 찾아낸다. - 지스타의 경우, 이전파티 정보를 기록하지 않아 찾아야한다.
		ZParty* pParty = m_pFixedPartyLogic->FindParty(nFixedPartyID);

		if (pParty != NULL)
		{
			ZPartyMember* pPartyMember = pParty->FindMember(strName);

			if (pPartyMember != NULL)
			{
				uidParty = pParty->GetUID();
				uidOffline = pPartyMember->GetUID();
			}
		}
	}


	// Online처리 가능하면 Online처리한다.
	if (DoOnline(uidParty, uidMember, uidOffline))
	{
		return true;
	}


	// 등록된 고정파티 멤버인지 확인한다.
	nFixedPartyID = m_pFixedPartyLogic->GetFixedPartyID(strName);

	if (nFixedPartyID == INVALID_ID)
	{
		return false;
	}


	// 파티를 찾는다.
	ZParty* pParty = m_pFixedPartyLogic->FindParty(nFixedPartyID);

	if (pParty == NULL)
	{
		m_pFixedPartyLogic->AddWaiting(nFixedPartyID, uidMember);
		m_pFixedPartyLogic->CreateFixedParty(nFixedPartyID);
	}
	else
	{
		MUID uidParty = pParty->GetUID();

		// 자리가 없으면, 오프라인 멤버를 한명 자른다.
		if (MAX_PARTY_MEMBER_COUNT <= pParty->GetCount())
		{
			// 파티 풀이면 더이상 받을 수 없음.
			if (0 == pParty->GetOfflineMemberCount())
			{
				return false;
			}

			// 오프라인 멤버를 한명 자른다.
			MUID uidOfflineMember = pParty->GetFirstOfflineMember();

			// 멤버 제거
			pParty->RemoveMember(uidOfflineMember);
			m_pRouter->RemoveMember(uidParty, uidOfflineMember);

			// 파티 동기화
			m_pRouter->EraseQuestRunnerSync(uidParty, uidOfflineMember);
			RouteRemoveMemberSync(pParty);
		}


		// 멤버 추가
		int nGameServerID = pPlayer->GetGameServerID();
		CID nCID = pPlayer->GetCID();		

		pParty->AddMember(uidMember, strName, nGameServerID, nCID);
		pPlayer->SetPartyUID(uidParty);


		// 멤버 추가 동기화
		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
		m_pRouter->AddMember(uidParty, uidMember, strName, nCID);
	}

	return true;
}

void ZPartyLogic::CreateAutoPartyReq(int nQuestID, const vector<MUID>& vecMemberUID)
{	
	// 유효성 검사
	vector<MUID> vecValidMemberUID;

	for each(const MUID& uidMember in vecMemberUID)
	{
		// 접속 플레이어 검사
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidMember);
		
		if (pPlayer == NULL)
		{
			continue;
		}

		// 파티가 없는 플레이어 검사
		MUID uidCheckParty = pPlayer->GetPartyUID();

		if (uidCheckParty.IsValid())
		{
			if (NULL != gmgr.pPartyManager->Find(uidCheckParty))
			{
				continue;
			}
		}

		// 추가
		vecValidMemberUID.push_back(uidMember);
	}

	// 유효한 발기인이 2명이 못되면, 포기
	if (vecValidMemberUID.size() < 2)
	{
		return;
	}

	
	// 파티 생성
	ZParty* pParty = NULL;
	MUID uidParty;
	
	for each(const MUID& uidMember in vecValidMemberUID)
	{
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidMember);
				
		if (pPlayer == NULL)
		{
			return;
		}

		int nGameServerID = pPlayer->GetGameServerID();

		if (pParty == NULL)
		{
			pParty = gmgr.pPartyManager->AddParty(uidMember, pPlayer->GetPlayerName(), nGameServerID, pPlayer->GetCID());
			pParty->SetAutoPartyQuestID(nQuestID);
			uidParty = pParty->GetUID();
		}
		else
		{
			pParty->AddMember(uidMember, pPlayer->GetPlayerName(), nGameServerID, pPlayer->GetCID());
		}
				
		pPlayer->SetPartyUID(uidParty);

		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
	}


	// 파티 정보 동기화	
	for each(const MUID& uidMember in vecValidMemberUID)
	{		
		ZPartyMember* pMember = pParty->GetMember(uidMember);
		
		if (pMember->GetUID() == pParty->GetLeader())
		{			
			m_pRouter->PartyAdd(uidParty, pMember->GetUID(), pMember->GetName(), pMember->GetCID());		
		}
		else
		{
			m_pRouter->AddMember(uidParty, pMember->GetUID(), pMember->GetName(), pMember->GetCID());
		}
	}
}

void ZPartyLogic::JoinAotuPartyReq(MUID uidParty, MUID uidPlayer)
{
	// 참가 희망자 확인
	ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidPlayer);

	if (pPlayer == NULL)
	{
		return;
	}


	// 기존 가입된 파티가 없어야한다.
	MUID uidJoinedParty = pPlayer->GetPartyUID();

	if (gmgr.pPartyManager->IsPartyMember(uidJoinedParty, uidPlayer))
	{
		return;
	}


	// 참가할 파티 확인
	ZParty* pParty = gmgr.pPartyManager->Find(uidParty);

	if (pParty == NULL)
	{
		return;
	}


	// 이미 풀파티면 참가할 수 없다.
	if (pParty->IsFull())
	{
		return;
	}
	

	// 자동파티 참가
	wstring strName = pPlayer->GetPlayerName();
	CID nCID = pPlayer->GetCID(); 
	int nMemberServerID = pPlayer->GetGameServerID();

	pParty->AddMember(uidPlayer, strName, nMemberServerID, nCID);
	pPlayer->SetPartyUID(uidParty);
	
	ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);


	// 파티 정보 동기화		
	m_pRouter->AddMember(uidParty, uidPlayer, strName, nCID);	
}
