#include "stdafx.h"
#include "GPartySystem.h"
#include "GParty.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPartyManager.h"
#include "GPlayerObjectManager.h"
#include "GServer.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GFieldMgr.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicFieldPolicy.h"
#include "GBattleArenaMgr.h"
#include "GMasterServerFacade.h"
#include "GProxyParty.h"
#include "GProxyFieldManager.h"
#include "GProxyDynamicFieldMaster.h"
#include "GClientPartyRouter.h"
#include "GServerPartyRouter.h"
#include "GPlayerPVPArea.h"
#include "GPlayerBattleArena.h"
#include "GPlayerDoing.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBTaskDataParty.h"
#include "GDBManager.h"

GPartySystem::GPartySystem()
: m_pPartyManager(NULL)
{
	m_pPartyManager = NewPartyManager();
	m_pClientRouter = NewClientRouter();
}

GPartySystem::~GPartySystem()
{
	SAFE_DELETE(m_pPartyManager);
	SAFE_DELETE(m_pClientRouter);
}

void GPartySystem::UpdateMember(GEntityPlayer* pMember)
{
	_ASSERT(pMember != NULL);

	// 파티 확인
	if (!pMember->HasParty())		return;

	MUID uidParty = pMember->GetPartyUID();	
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)	
	{
		pMember->SetParty(MUID::Invalid());
		return;
	}


	// 파티원 정보 동기화
	MUID uidMember = pMember->GetUID();
	TD_PARTY_MEMBER tdPartyMember;
	vector<int> vecBuff;	
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();

	pServerRouter->MakePartyMemberInfo(pParty, uidMember, &tdPartyMember, &vecBuff);
	pServerRouter->UpdateMember(uidParty, tdPartyMember, vecBuff);
}

void GPartySystem::RouteToAllMembers(MUID uidParty, MCommand* pCommand, const MUID& uidExceptMember)
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		
	{
		SAFE_DELETE(pCommand);
		return;
	}

	m_pClientRouter->RouteToAllMembers(pParty, pCommand, uidExceptMember);
}

bool GPartySystem::IsPartyMember(GEntityPlayer* pPlayer) const
{
	_ASSERT(pPlayer != NULL);

	if (!pPlayer->HasParty())	return false;

	MUID uidParty = pPlayer->GetPartyUID();
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)			return false;

	return pParty->IsExistMember(pPlayer->GetUID());
}

bool GPartySystem::IsSamePartyMember(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2) const
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	if (!pPlayer1->HasParty())	
		return false;

	if (!pPlayer2->HasParty())	
		return false;

	MUID uidParty = pPlayer1->GetPartyUID();
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)			return false;

	return pParty->IsExistMember(pPlayer2->GetUID());
}

bool GPartySystem::IsPartyLeader( GEntityPlayer* pPlayer ) const
{
	_ASSERT(pPlayer != NULL);

	if (!pPlayer->HasParty())	return false;

	MUID uidParty = pPlayer->GetPartyUID();
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)			return false;

	return pParty->IsLeader(pPlayer->GetUID());
}

int GPartySystem::CalculateOnlineMemberCount(MUID uidParty) const
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return 0;

	int nCount = 0;
	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;
		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
		if (pMember == NULL)	continue;

		nCount++;
	}

	return nCount;
}

void GPartySystem::AddQuest(MUID uidParty, MUID uidMember, int nQuestID) const
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;

	// 퀘스트 추가
	vector<int> vecQuestID;
	vecQuestID.push_back(nQuestID);

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->AddQuest(uidParty, uidMember, vecQuestID);
}

void GPartySystem::RemoveQuest(MUID uidParty, MUID uidMember, int nQuestID) const
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 퀘스트 제거
	vector<int> vecQuestID;
	vecQuestID.push_back(nQuestID);

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->RemoveQuest(uidParty, uidMember, vecQuestID);
}

MUID GPartySystem::FindField(MUID uidParty, int nFieldGroupID) const
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return MUID::Invalid();

	return pParty->FindField(nFieldGroupID);
}

void GPartySystem::RemoveFieldSelf(MUID uidParty, int nFieldGroupID) const
{
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->RemoveFieldSelf(uidParty, nFieldGroupID);
}

GPartyManager* GPartySystem::NewPartyManager(void)
{
	return new GPartyManager();
}

GClientPartyRouter* GPartySystem::NewClientRouter(void)
{
	return new GClientPartyRouter();
}

GPartyManager* GPartySystem::GetPartyManager(void) const
{
	return m_pPartyManager;
}

GParty* GPartySystem::FindParty(MUID uidParty) const
{
	return m_pPartyManager->FindParty(uidParty);
}

void GPartySystem::CreateAutoPartyReq(QuestID nQuestID, const vector<MUID>& uidPromoters)
{
	// 발기인이 있어야한다.
	if (uidPromoters.empty())
	{
		return;
	}

	// 발기인 수는 파티원 인원 제한보다 적어야한다.
	if (MAX_PARTY_MEMBER_COUNT < uidPromoters.size())
	{
		return;
	}

	// 발기인은 파티가 없어야한다.
	// TODO: EntityPlayer로 있는 유저인지 검사한다. [11/3/2010 praptor]
	// TODO: 파티가 있는지 검사 [11/3/2010 praptor]
	

	// 마스터 서버에 통보
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->CreateAutoPartyReq(nQuestID, uidPromoters);
}

void GPartySystem::JoinAutoPartyReq(MUID uidParty, GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);
	

	// 이미 파티가 있으면 요청할 수 없다.
	if (pPlayer->HasParty())
	{
		// TODO: 실패 통보해야하는지 확인 [12/2/2010 praptor]
		return;
	}


	// 자동파티 가입	
	MUID uidPlayer = pPlayer->GetUID();
	
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();	
	pServerRouter->JoinAutoPartyReq(uidParty, uidPlayer);
}

void GPartySystem::InviteReq(MUID uidTargetPlayer, MUID uidRequestPlayer)
{
	// 요청자 확인
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;
	

	// 대상 UID 검사
	if (uidTargetPlayer.IsInvalid())
	{
		mlog3("%s - invalid uid. uid(%I64d)\n", __FUNCTION__, uidTargetPlayer.Value);
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}


	// 자기 자신을 초대하는지 검사
	if (uidTargetPlayer == uidRequestPlayer)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_NOT_INVITE_SELF);
		return;
	}


	// 파티 확인
	if (pRequestPlayer->HasParty())
	{
		// 일반 파티원은 권한없음
		if (!pRequestPlayer->IsPartyLeader())
		{
			m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_NOT_PARTY_LEADER);
			return;
		}

		// 파티 꽉참
		MUID uidParty = pRequestPlayer->GetPartyUID();
		if (IsPartyFull(uidParty))
		{
			m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_FULL_PARTY);
			return;
		}
	}


	// 초대 요청
	pRequestPlayer->GetDoing().EndDoing();

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->InviteReq(uidTargetPlayer, uidRequestPlayer);
}

void GPartySystem::InviteRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	// 요청자 확인
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;

	
	// 응답 통보
	if (nResult != CR_SUCCESS)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, nResult);
		return;
	}
}

void GPartySystem::AcceptReq(MUID uidTargetPlayer, MUID uidRequestPlayer, wstring strRequestPlayerName)
{	
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();

	// 대상 확인
	GEntityPlayer* pTargetPlayer = gmgr.pPlayerObjectManager->GetEntity(uidTargetPlayer);
	if (pTargetPlayer == NULL)
	{		
		pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}


	// 동적필드 입장 시, 파티 초대 안됨
	if (pTargetPlayer->IsStayDynamicField())
	{
		pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_NOT_INVITE_TO_PLAYER_IN_TRIALFIELD);
		return;
	}


	// 파티 확인
	if (pTargetPlayer->HasParty())
	{
		pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
		return;
	}


	// 인터렉션 상태 확인
	if (pTargetPlayer->GetDoing().IsBusy())
	{
		pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_OPPONENT_IS_BUSY);
		return;
	}


	// 수락 요청
	pTargetPlayer->SetPartyInvitorUID(uidRequestPlayer);

	bool isCombatInvitee = pTargetPlayer->IsNowCombat();	
	m_pClientRouter->InviteQuestion(pTargetPlayer, uidRequestPlayer, strRequestPlayerName.c_str(), isCombatInvitee);
}

void GPartySystem::AcceptRes(MUID uidTargetPlayer, PARTY_RESPOND nRespond)
{
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();


	// 대상 확인
	GEntityPlayer* pTargetPlayer = gmgr.pPlayerObjectManager->GetEntity(uidTargetPlayer);
	if (pTargetPlayer == NULL)				return;


	// 요청자 확인
	MUID uidRequestPlayer = pTargetPlayer->GetPartyInvitorUID();	
	if (uidRequestPlayer.IsInvalid())
	{
		m_pClientRouter->AcceptCancel(pTargetPlayer, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}


	// 파티 확인
	if (pTargetPlayer->HasParty())
	{
		pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
		m_pClientRouter->AcceptCancel(pTargetPlayer, CR_FAIL_PARTY_TARGET_ALEADY_HAS_PARTY);
		return;
	}


	// 수락 거절
	if (nRespond != PRS_OK)
	{		
		pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_FAIL_PARTY_REFUSE_INVITATION);
		return;
	}


	// 수락 응답
	pTargetPlayer->GetDoing().EndDoing();
	pTargetPlayer->SetPartyInvitorUID(MUID::Invalid());

	pServerRouter->AcceptRes(uidRequestPlayer, uidTargetPlayer, CR_SUCCESS);


	// 새 파티리더가 됨을 주변 유저에게 알림
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (!pLeader)	
		return;
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LEADER_NOTIFY,
		2,
		NEW_UID(MUID::Invalid()),
		NEW_UID(pLeader->GetUID())
		);
	pLeader->RouteToThisCell(pNewCmd);
}

void GPartySystem::AcceptCancel(MUID uidTargetPlayer, CCommandResultTable nResult)
{
	// 대상 확인
	GEntityPlayer* pTargetPlayer = gmgr.pPlayerObjectManager->GetEntity(uidTargetPlayer);
	if (pTargetPlayer == NULL)				return;


	// 수락 응답 취소
	m_pClientRouter->AcceptCancel(pTargetPlayer, nResult);
}

void GPartySystem::LeaveReq(MUID uidLeavePlayer)
{
	// 요청자 확인
	GEntityPlayer* pLeavePlayer = gmgr.pPlayerObjectManager->GetEntity(uidLeavePlayer);
	if (pLeavePlayer == NULL)		return;


	// 파티 확인
	if (!pLeavePlayer->HasParty())
	{
		m_pClientRouter->InviteRespond(pLeavePlayer, CR_FAIL_PARTY_NOT_IN_PARTY);
		return;
	}

	// 탈퇴 요청
	MUID uidParty = pLeavePlayer->GetPartyUID();

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->LeaveReq(uidParty, uidLeavePlayer);
}

void GPartySystem::LeaveRes(MUID uidLeavePlayer, CCommandResultTable nResult)
{
	// 요청자 확인
	GEntityPlayer* pLeavePlayer = gmgr.pPlayerObjectManager->GetEntity(uidLeavePlayer);
	if (pLeavePlayer == NULL)		return;

	
	// 결과 확인
	if (nResult != CR_SUCCESS)
	{
		m_pClientRouter->InviteRespond(pLeavePlayer, nResult);
		return;
	}

	pLeavePlayer->OnLeaveFromParty();
}

void GPartySystem::KickReq(MUID uidRequestPlayer, MUID uidTargetPlayer)
{
	// 요청자 확인
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;


	// 파티장 확인
	if (!pRequestPlayer->IsPartyLeader())
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	
	// 자신은 강퇴 안됨
	if (uidRequestPlayer == uidTargetPlayer)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_KICK_SELF);
		return;
	}


	// 파티객체 확인
	MUID uidParty = pRequestPlayer->GetPartyUID();	
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;		
	}

	
	// 파티원 확인
	if (!pParty->IsExistMember(uidTargetPlayer))
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_TARGET_NOT_IN_PARTY);
		return;
	}


	// 강퇴 요청
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->KickReq(uidParty, uidRequestPlayer, uidTargetPlayer);
}

void GPartySystem::KickRes(MUID uidRequestPlayer, MUID uidTargetPlayer, CCommandResultTable nResult)
{
	// 요청자 확인
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;


	// 결과 확인
	if (nResult != CR_SUCCESS)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, nResult);
		return;
	}

	GEntityPlayer* pLeavePlayer = gmgr.pPlayerObjectManager->GetEntity(uidTargetPlayer);
	if (pLeavePlayer == NULL)		return;
	pLeavePlayer->OnLeaveFromParty();
}

void GPartySystem::DoOnline(MUID uidMember)
{	
	// 접속자 확인
	GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
	if (pMember == NULL)		return;


	// 파티원인지 확인
	if (!pMember->HasParty())	return;

	
	// 기존 파티정보 초기화 - 서버 응답 성공 시, 다시 설정한다.
	MUID uidParty = pMember->GetPartyUID();	
	pMember->SetParty(MUID::Invalid());


	// 파티 확인
	const GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);
	if (pProxyParty == NULL)
	{
		return;
	}


	// 파티원 정보 확인
	wstring strMemberName = pMember->GetName();
	MUID uidOffline = pProxyParty->Find(strMemberName);
	if (uidOffline.IsInvalid())
	{
		return;
	}


	// 온라인 상태 전달
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->DoOnline(uidParty, uidMember, uidOffline);
}

void GPartySystem::DoOffline(GEntityPlayer* pMember)
{
	_ASSERT(pMember != NULL);

	// 파티 확인
	if (!pMember->HasParty())	return;


	// 파티 객체 확인
	MUID uidParty = pMember->GetPartyUID();
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)			return;


	// 파티원 확인
	MUID uidMember = pMember->GetUID();
	if (!pParty->IsExistMember(uidMember))	return;

	
	// 서버이동 확인
	if (pMember->IsMoveServer())
	{
		// 서버이동상태 전달
		GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
		pServerRouter->MoveServer(uidParty, uidMember);
	}
	else
	{
		// 오프라인상태 전달
		GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
		pServerRouter->DoOffline(uidParty, uidMember);
	}
}

void GPartySystem::JoinInviteReq(MUID uidParty, MUID uidRequestPlayer)
{
	// 요청자 확인
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;


	// 요청자 인터렉션 확인
	if (pRequestPlayer->GetDoing().IsBusy())
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_PLAYER_IS_BUSY);
		return;
	}


	// 요청자 전투 확인
	if (pRequestPlayer->HasDuel())
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_REQUESTER_IS_DUEL);
		return;
	}

	// 요청자 PVP영역 확인
	if (pRequestPlayer->GetPlayerPVPArea().IsLocatedPvPArea())
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_REQUESTER_IN_PVPAREA);
		return;
	}


	// 요청자 투기장 확인
	if (pRequestPlayer->GetPlayerBattleArena().IsEntered())
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_REQUESTER_IN_BATTLEARENA);
		return;
	}


	// 파티 확인	
	const GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);	
	if (pProxyParty == NULL)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return; 
	}


	// 파티 제한 확인
	if (pProxyParty->IsFull())
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, CR_FAIL_PARTY_FULL_PARTY);
		return;
	}


	// 가입 요청
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->JoinInviteReq(uidParty, uidRequestPlayer);
}

void GPartySystem::JoinInviteRes(MUID uidRequestPlayer, CCommandResultTable nResult)
{
	// 요청자 확인
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntity(uidRequestPlayer);
	if (pRequestPlayer == NULL)		return;

	
	// 응답 통보
	if (nResult != CR_SUCCESS)
	{
		m_pClientRouter->InviteRespond(pRequestPlayer, nResult);
		return;
	}
}

void GPartySystem::JoinAcceptReq(MUID uidParty, MUID uidLeader, MUID uidRequestPlayer, wstring strRequestPlayerName)
{
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();

	// 파티 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 파티장 맞는지 확인
	if (uidLeader != pParty->GetLeader())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 파티장 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 파티장 인터렉션 확인
	if (!pLeader->GetDoing().IsEndableBusy())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_PLAYER_IS_BUSY);
		return;
	}


	// 가입 요청
	pLeader->GetDoing().EndDoing();
	pLeader->SetPartyInviteeUID(uidRequestPlayer);

	bool isNowCombat = pLeader->IsNowCombat();
	m_pClientRouter->InviteForMeQuestion(pLeader, uidRequestPlayer, strRequestPlayerName.c_str(), isNowCombat);
}

void GPartySystem::JoinAcceptRes(MUID uidLeader, PARTY_RESPOND nRespond)
{	
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();

	// 파티장 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)	return;


	// 가입 요청자 확인
	MUID uidRequestPlayer = pLeader->GetPartyInviteeUID();
	pLeader->SetPartyInviteeUID(MUID::Invalid());

	if (uidRequestPlayer.IsInvalid())
	{
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_SYSTEM_INVALID_PC_UID);
		return;
	}


	// 파티 확인
	MUID uidParty = pLeader->GetPartyUID();
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_INVALID_PARTY);
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}


	// 파티 꽉참
	if (pParty->IsFull())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_FULL_PARTY);
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_PARTY_FULL_PARTY);
		return;
	}


	// 파티장 인터렉션 확인
	if (!pLeader->GetDoing().IsEndableBusy())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY);
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_PARTY_RESPOND_PLAYER_IS_BUSY);
		return;
	}

	
	// 파티장 듀얼 확인
	if (pLeader->HasDuel())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_RESPONDER_IS_DUEL);
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_PARTY_RESPONDER_IS_DUEL);
		return;
	}


	// 파티장 PVP 확인
	if (pLeader->GetPlayerPVPArea().IsLocatedPvPArea())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_RESPONDER_IN_PVPAREA);
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_PARTY_RESPONDER_IN_PVPAREA);
		return;
	}


	// 파티장 투기장 확인
	if (pLeader->GetPlayerBattleArena().IsEntered())
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_RESPONDER_IN_BATTLEARENA);
		m_pClientRouter->JoinAcceptCancel(pLeader, CR_FAIL_PARTY_RESPONDER_IN_BATTLEARENA);
		return;
	}


	// 수락 거절
	if (nRespond != PRS_OK)
	{
		pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_FAIL_PARTY_REFUSE_INVITATION_FOR_ME);
		return;
	}

	
	// 수락 응답
	pLeader->GetDoing().EndDoing();
	pServerRouter->JoinAcceptRes(uidParty, uidLeader, uidRequestPlayer, CR_SUCCESS);

	// 새 파티리더가 됨을 주변 유저에게 알림
	MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LEADER_NOTIFY,
		2,
		NEW_UID(MUID::Invalid()),
		NEW_UID(uidLeader)
		);
	pLeader->RouteToThisCell(pNewCmd);
}

void GPartySystem::JoinAcceptCancel(MUID uidLeader, CCommandResultTable nResult)
{
	// 파티장 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)	return;


	// 수락 취소
	m_pClientRouter->JoinAcceptCancel(pLeader, nResult);
}

void GPartySystem::AddParty(MUID uidParty, MUID uidLeader, QuestID nQuestID)
{
	// 파티장 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)		return;


	// 파티 생성
	GParty* pParty = RestoreParty(uidParty);
	VALID(pParty);
	pParty->SetAutoPartyQuestID(nQuestID);

	// 파티장 설정
	pLeader->SetParty(uidParty);
	pLeader->SetPartyLeader(true);
	
	// 파티원간 정보 동기화
	AddQuest(uidParty, pLeader);
	AddField(uidParty, pLeader);
	UpdateQuestRunner(uidParty, pLeader);

	UpdateParty(pParty, pLeader);

	gsys.pDBManager->PartyCreateLog(
		GDBT_PARTY(pParty->GetUID()
			, pLeader->GetAID()
			, pLeader->GetCID()
			, GDBT_PARTY::NORMAL));	
}

void GPartySystem::RemoveParty(MUID uidParty)
{
	// 투기장에 등록되있었던 경우라면
	if (gmgr.pBattleArenaMgr->IsEntryTeam(uidParty))
	{
		// 투기장 등록해제
		gmgr.pBattleArenaMgr->OnDeleteTeam(uidParty);
	}


	// 새 파티리더가 됨을 주변 유저에게 알림
	GParty* pParty = FindParty(uidParty);
	if (pParty)
	{
		GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(pParty->GetLeader());
		if (pLeader)
		{
			MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LEADER_NOTIFY,
				2,
				NEW_UID(pLeader->GetUID()),
				NEW_UID(MUID::Invalid())
				);
			pLeader->RouteToThisCell(pNewCmd);

			gsys.pDBManager->PartyDeleteLog(
				GDBT_PARTY(pParty->GetUID()
				, pLeader->GetAID()
				, pLeader->GetCID()
				, GDBT_PARTY::NORMAL));	
		}	
	}
	

	m_pPartyManager->DeleteParty(uidParty);
}

void GPartySystem::AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID)
{
	// 파티와 파티원 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);

	if (pParty == NULL && pMember == NULL)		return;


	// 파티 생성 및 추가
	if (pParty == NULL)
	{
		pParty = RestoreParty(uidParty);
	}
	else
	{
		pParty->AddMember(uidMember, strMemberName, nMemberCID);	
	}


	// 파티 설정
	if (pMember != NULL)
	{
		// 파티원 설정
		pMember->SetParty(uidParty);

		// 파티원간 정보 동기화		
		AddQuest(uidParty, pMember);
		AddField(uidParty, pMember);
		UpdateQuestRunner(uidParty, pMember);

		UpdateParty(pParty, pMember);

		gsys.pDBManager->PartyEnterLog(
			GDBT_PARTY(pParty->GetUID()
			, pMember->GetAID()
			, pMember->GetCID()
			, GDBT_PARTY::NORMAL));			
	}


	// 파티원간 정보 동기화
	m_pClientRouter->NotifyJoin(pParty, uidMember, strMemberName.c_str());
}

void GPartySystem::RemoveMember(MUID uidParty, MUID uidMember)
{
	// 파티 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 파티원 확인
	GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);

	if (pMember != NULL)
	{		
		pMember->SetParty(MUID::Invalid());
		pMember->SetPartyLeader(false);
	}


	// 파티원 제거
	m_pClientRouter->NotifyLeave(pParty, uidMember);
	pParty->RemoveMember(uidMember);


	// 로그
	if (pMember != NULL)
	{
		gsys.pDBManager->PartyLeaveLog(
			GDBT_PARTY(pParty->GetUID()
			, pMember->GetAID()
			, pMember->GetCID()
			, GDBT_PARTY::NORMAL));	
	}


	if (IsEmptyMemberInServer(pParty))
	{
		RemoveParty(uidParty);		
	}
}

void GPartySystem::AddOfflineMember(MUID uidParty, MUID uidMember)
{
	// 파티 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 오프라인 파티원 추가	
	pParty->AddOfflineMember(uidMember);

	if (IsEmptyMemberInServer(pParty))
	{
		m_pPartyManager->DeleteParty(uidParty);
	}
}

void GPartySystem::RemoveOfflineMember(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	// 파티와 파티원 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);

	if (pParty == NULL && pMember == NULL)		return;


	// 파티 생성 및 Offline 정보 변경
	if (pParty == NULL)
	{
		pParty = RestoreParty(uidParty);
	}
	else
	{
		pParty->RemoveOfflineMember(uidOffline);
		pParty->ChangeMemberUID(uidOffline, uidMember);		
	}


	// 파티원 정도 재동기화
	if (pMember != NULL)
	{
		pMember->SetParty(uidParty);

		AddQuest(uidParty, pMember);
		AddField(uidParty, pMember);
		UpdateQuestRunner(uidParty, pMember);

		UpdateParty(pParty, pMember);
	}
}

void GPartySystem::ChangePartyNameRes(MUID uidParty, wstring strName)
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)	return;

	pParty->SetName(strName);

	m_pClientRouter->ChangePartyNameRes(pParty, strName);
}

void GPartySystem::ChangePartyLeaderRes( MUID uidParty, MUID uidNewLeader )
{
	// 파티 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)			return;

	
	// 파티장 해제
	MUID uidOldLeader = pParty->GetLeader();
	GEntityPlayer* pOldLeader = gmgr.pPlayerObjectManager->GetEntity(uidOldLeader);
	if (pOldLeader != NULL)
	{
		pOldLeader->SetPartyLeader(false);
	}


	// 새 파티장 설정
	GEntityPlayer* pNewLeader = gmgr.pPlayerObjectManager->GetEntity(uidNewLeader);
	if (pNewLeader != NULL)
	{
		pNewLeader->SetPartyLeader(true);
	}


	// 파티장 변경
	pParty->SetLeader(uidNewLeader);


	// 변경 통보
	m_pClientRouter->ChangePartyLeaderRes(pParty, uidOldLeader, uidNewLeader);


	// 새 파티리더가 됨을 주변 유저에게 알림
	GEntityPlayer* pLeader = 
		gmgr.pPlayerObjectManager->GetEntity(uidNewLeader);
	if (pLeader)
	{
		MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LEADER_NOTIFY,
			2,
			NEW_UID(uidOldLeader),
			NEW_UID(uidNewLeader)
			);
		pLeader->RouteToThisCell(pNewCmd);
	}

	// 오래된 파티리더가 됨을 주변 유저에게 알림
	if (pOldLeader)
	{
		MCommand* pNewCmd = MakeNewCommand(MC_PARTY_CHANGE_LEADER_NOTIFY,
			2,
			NEW_UID(uidOldLeader),
			NEW_UID(uidNewLeader)
			);
		pOldLeader->RouteToThisCell(pNewCmd);
	}
}

void GPartySystem::ChangePartyLootingRuleRes(MUID uidParty, LOOTING_RULE_DATA rule)
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)	return;

	pParty->SetLootingRuleData(rule);

	m_pClientRouter->ChangePartyLootingRuleRes(pParty, rule);
}

void GPartySystem::ChangeQuestIDRes( MUID uidParty, int nQuestID )
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)	return;

	pParty->SetAutoPartyQuestID(nQuestID);

	m_pClientRouter->ChangePartyQuestIDRes(pParty, nQuestID);
}

void GPartySystem::SyncParty(const TD_PARTY& tdParty, MCommand* pPartySyncCommand)
{	
	GParty* pParty = m_pPartyManager->FindParty(tdParty.m_uidParty);
	if (pParty == NULL)		return;


	PARTY_SETTING partySetting;
	partySetting.m_uidLeader = tdParty.m_partySetting.m_uidLeader;
	wcsncpy_s(partySetting.m_szName, tdParty.m_partySetting.m_szName, _TRUNCATE);
	partySetting.m_nAutoPartyQuestID = tdParty.m_partySetting.m_nAutoPartyQuestID;
	
	partySetting.m_lootingRuleData.m_nLRC = (LOOTING_RULE_COMMON)tdParty.m_partySetting.m_nLRC;
	partySetting.m_lootingRuleData.m_nLRR = (LOOTING_RULE_RARE)tdParty.m_partySetting.m_nLRR;
	partySetting.m_lootingRuleData.m_nLRRF = (LOOTING_RULE_RARE_FOR)tdParty.m_partySetting.m_nLRRF;

	pParty->SetPartySetting(partySetting);
	m_pClientRouter->PartySync(pParty, pPartySyncCommand);
}

void GPartySystem::SyncMember(MUID uidParty, MCommand* pPartyMemberSyncCommand)
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	m_pClientRouter->MemberSync(pParty, pPartyMemberSyncCommand);
}

void GPartySystem::SyncQuest(MUID uidParty, const map<int, int>& mapQuests)
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 퀘스트 갱신
	pParty->ClearQuests();

	for each(const map<int, int>::value_type& pairQuest in mapQuests)
	{
		int nQuestID = pairQuest.first;
		int nQuestCount = pairQuest.second;
		
		pParty->AddQuest(nQuestID, nQuestCount);
	}
}

void GPartySystem::SyncField(MUID uidParty, const map<int, MUID>& mapFields)
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 필드 갱신
	pParty->ClearField();

	for each(const map<int, MUID>::value_type& pairQuest in mapFields)
	{
		int nFieldGroupID = pairQuest.first;
		MUID uidFieldGroup = pairQuest.second;

		pParty->CreateField(nFieldGroupID, uidFieldGroup);
	}
}

void GPartySystem::SyncQuestRunner(MUID uidParty, MUID uidMember, MUID uidField, const vector<int>& vecQuestID)
{
	// 파티 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 퀘스트러너 동기화
	const set<MUID>& setPolicy = pParty->GetFieldPolicy();
	for each(const MUID& uidFieldGroup in setPolicy)
	{
		GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
		if (pDynamicFieldMaster == NULL)		continue;

		GDynamicFieldPolicy* pPolicy = pDynamicFieldMaster->GetPolicy();
		_ASSERT(pPolicy != NULL);

		pPolicy->OnQuestSynch(uidMember, uidField, &vecQuestID);
	}
}

void GPartySystem::SyncEraseQuestRunner(MUID uidParty, MUID uidMember)
{
	// 파티 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return;


	// 퀘스트러너 제거
	const set<MUID>& setPolicy = pParty->GetFieldPolicy();
	for each(const MUID& uidFieldGroup in setPolicy)
	{
		GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
		if (pDynamicFieldMaster == NULL)		continue;

		GDynamicFieldPolicy* pPolicy = pDynamicFieldMaster->GetPolicy();
		_ASSERT(pPolicy != NULL);

		pPolicy->OnQuestEraseRunner(uidMember);
	}
}

void GPartySystem::MoveServerSync(MUID uidParty, MUID uidMember, MUID uidOffline)
{
	// 파티와 파티원 확인
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);

	if (pParty == NULL && pMember == NULL)		return;


	// 파티 생성 및 Offline 정보 변경
	if (pParty == NULL)
	{
		pParty = RestoreParty(uidParty);
	}
	else
	{
		pParty->ChangeMemberUID(uidOffline, uidMember);
	}


	// 파티설정 복원
	if (pMember != NULL)
	{
		pMember->SetParty(uidParty);

		if (uidMember == pParty->GetLeader())
		{
			pMember->SetPartyLeader(true);
		}
	}


	// 퀘스트러너 동기화
	const set<MUID>& setPolicy = pParty->GetFieldPolicy();
	for each(const MUID& uidFieldGroup in setPolicy)
	{
		GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup);
		if (pDynamicFieldMaster == NULL)		continue;

		GDynamicFieldPolicy* pPolicy = pDynamicFieldMaster->GetPolicy();
		_ASSERT(pPolicy != NULL);

		pPolicy->ChangePlayerUID(uidOffline, uidMember);
	}
}

bool GPartySystem::IsPartyFull(MUID uidParty) const
{
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)		return false;

	return pParty->IsFull();
}

GParty* GPartySystem::RestoreParty(MUID uidParty)
{
	// Proxy 확인
	const GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);
	_ASSERT(pProxyParty != NULL);


	// 파티 생성
	GParty* pParty = m_pPartyManager->NewParty(uidParty);
	_ASSERT(pParty != NULL);


	// 파티원 추가
	partymember_iterator it = pProxyParty->BeginMember();
	while(it != pProxyParty->EndMember())
	{
		MUID uidMember = it->first;
		wstring strMemberName = it->second.m_strName;
		int nMemberCID = it->second.m_nCID;

		pParty->AddMember(uidMember, strMemberName, nMemberCID);
		
		if (uidMember == pProxyParty->GetLeader())
		{
			pParty->SetLeader(uidMember);
		}

		it++;
	}

	m_pPartyManager->AddParty(pParty);


	return pParty;
}

void GPartySystem::AddQuest(MUID uidParty, GEntityPlayer* pMember) const
{
	MUID uidMember = pMember->GetUID();
	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);	

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->AddQuest(uidParty, uidMember, vecQuestID);
}

void GPartySystem::AddField(MUID uidParty, GEntityPlayer* pMember) const
{
	_ASSERT(pMember != NULL);

	// 소유 동적필드 확인
	MUID uidFieldGroup = pMember->GetPlayerField().GetDynamicFieldGroupUID();
	if (uidFieldGroup.IsInvalid())		return;


	// 파티필드에 추가
	MUID uidMember = pMember->GetUID();
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->AddField(uidParty, uidMember, uidFieldGroup);
}

void GPartySystem::UpdateQuestRunner(MUID uidParty, GEntityPlayer* pMember) const
{
	MUID uidMember = pMember->GetUID();
	MUID uidField = pMember->GetFieldUID();
	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->UpdateQuestRunner(uidParty, uidMember, uidField, vecQuestID);
}

void GPartySystem::RemoveQuest(MUID uidParty, GEntityPlayer* pMember) const
{
	MUID uidMember = pMember->GetUID();
	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->RemoveQuest(uidParty, uidMember, vecQuestID);
}

void GPartySystem::RemoveField(MUID uidParty, GEntityPlayer* pMember) const
{
	_ASSERT(pMember != NULL);

	// 소유 동적필드 확인
	MUID uidFieldGroup = pMember->GetPlayerField().GetDynamicFieldGroupUID();
	if (uidFieldGroup.IsInvalid())		return;


	// 동적 필드 생성 확인
	GProxyFieldManager* pFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
	_ASSERT(pFieldManager != NULL);
	GProxyDynamicFieldMaster* pFieldGroup = pFieldManager->FindFieldGroup(uidFieldGroup);
	if (pFieldGroup == NULL)			return;


	// 파티필드에 제거
	int nFieldGroupID = pFieldGroup->GetInfo()->nID;
	MUID uidMember = pMember->GetUID();
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->RemoveField(uidParty, uidMember, nFieldGroupID);
}

void GPartySystem::EraseQuestRunner(MUID uidParty, MUID uidMember) const
{
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->EraseQuestRunner(uidParty, uidMember);
}

void GPartySystem::UpdateParty(const GParty* pParty, GEntityPlayer* pMember) const
{
	_ASSERT(pParty != NULL);
	_ASSERT(pMember != NULL);

	// 서버 라우터 확보
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();

	
	// 파티원 정보 수집
	MUID uidMember = pMember->GetUID();
	TD_PARTY_MEMBER tdPartyMember;
	vector<int> vecBuff;	
	pServerRouter->MakePartyMemberInfo(pParty, uidMember, &tdPartyMember, &vecBuff);


	// 파티원 정보 갱신	
	MUID uidParty = pParty->GetUID();
	pServerRouter->UpdateParty(uidParty, tdPartyMember, vecBuff);
}

bool GPartySystem::IsEmptyMemberInServer(const GParty* pParty) const
{
	_ASSERT(pParty != NULL);

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;

		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
		if (pMember != NULL)		
		{
			return false;
		}
	}

	return true;
}

void GPartySystem::CreateSinglePartyReq(MUID uidRequestPlayer)
{
	if (IsRunForTest())
	{
		GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
		pServerRouter->CreateSinglePartyReq(uidRequestPlayer);
	}
}

void GPartySystem::ChangePartyNameReq(MUID uidLeader, wstring strName)
{
	// 파티장 권한 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)	return;

	if (!pLeader->IsPartyLeader())
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}
	
	// 이름 변경 요청
	MUID uidParty = pLeader->GetPartyUID();

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->ChangePartyNameReq(uidParty, uidLeader, strName);
}

void GPartySystem::ChangePartyLeaderReq(MUID uidLeader, MUID uidNewLeader)
{
	// 파티장 권한 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)	return;

	if (!pLeader->IsPartyLeader())
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	// 다음 파티장 확인
	MUID uidParty = pLeader->GetPartyUID();
	
	GParty* pParty = m_pPartyManager->FindParty(uidParty);
	if (pParty == NULL)
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_INVALID_PARTY);
		return;
	}

	if (!pParty->IsExistMember(uidNewLeader))
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_NOT_SAME_PARTY);
		return;
	}

	// 파티장 변경 요청
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->ChangePartyLeaderReq(uidParty, uidLeader, uidNewLeader);
}

void GPartySystem::ChangePartyLootingRuleReq(MUID uidLeader, LOOTING_RULE_DATA rule)
{
	// 파티장 권한 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)	return;

	if (!pLeader->IsPartyLeader())
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	//// 20037 luna1x 마스터룻 사용안함	
	if (LRC_MASTER_LOOT == rule.m_nLRC || LRR_MASTER_LOOT == rule.m_nLRR)
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_INVALID_SETTING);
		return;
	}
	
	// 루팅룰 변경 요청
	MUID uidParty = pLeader->GetPartyUID();
	
	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->ChangePartyLootingRuleReq(uidParty, uidLeader, rule);
}

void GPartySystem::ChangeQuestIDReq(MUID uidLeader, QuestID nQuestID)
{
	// 파티장 권한 확인
	GEntityPlayer* pLeader = gmgr.pPlayerObjectManager->GetEntity(uidLeader);
	if (pLeader == NULL)	return;

	if (!pLeader->IsPartyLeader())
	{
		pLeader->FailAndRouteSystemMsg(CR_FAIL_PARTY_NOT_PARTY_LEADER);
		return;
	}

	// 설정 변경
	MUID uidParty = pLeader->GetPartyUID();

	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();
	pServerRouter->ChangeQuestIDReq(uidParty, uidLeader, nQuestID);
}

vector<GEntityPlayer*> GPartySystem::CollectNeighborMember(GEntityPlayer* pPlayer, float fDistance) const
{	
	VALID_RET(pPlayer, vector<GEntityPlayer*>());

	vector<GEntityPlayer*> vecNeighborMember;
	vecNeighborMember.push_back(pPlayer);

	GParty* pParty = gsys.pPartySystem->FindParty(pPlayer->GetPartyUID());
	if (NULL == pParty) return vecNeighborMember;

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;
		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
		if (NULL == pMember) continue;
		if (pPlayer == pMember) continue;

		if (fDistance < pPlayer->DistanceTo(pMember)) continue;

		vecNeighborMember.push_back(pMember);
	}
	
	return vecNeighborMember;
}

vector<MUID> GPartySystem::CollectMemberUID( GEntityPlayer* pPlayer ) const
{
	VALID_RET(pPlayer, vector<MUID>());

	vector<MUID> vecMemberUID;
	vecMemberUID.push_back(pPlayer->GetUID());

	GParty* pParty = gsys.pPartySystem->FindParty(pPlayer->GetPartyUID());
	if (NULL == pParty) return vecMemberUID;

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;
		if (pPlayer->GetUID() == uidMember) continue;
				
		vecMemberUID.push_back(uidMember);
	}

	return vecMemberUID;
}

vector<int> GPartySystem::CollectNeighborMemberCID( GEntityPlayer* pPlayer, float fDistance ) const
{
	VALID_RET(pPlayer, vector<int>());

	vector<GEntityPlayer*> vecMember = CollectNeighborMember(pPlayer, fDistance);

	vector<int> vecMemberCID;
	for each (GEntityPlayer* pMember in vecMember)
	{
		vecMemberCID.push_back(pMember->GetCID());
	}

	return vecMemberCID;
}

void GPartySystem::FixedPartyLogOn(MUID uidMember, wstring strName) const
{
	MUID uidParty;
	MUID uidOffline;


	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);

	if (pPlayer == NULL)
	{
		return;
	}


	if (pPlayer->HasParty())
	{ 
		uidParty = pPlayer->GetPartyUID();
		pPlayer->SetParty(MUID::Invalid());

		const GProxyParty* pProxyParty = gsys.pMasterServerFacade->FindProxyParty(uidParty);

		if (pProxyParty != NULL)
		{
			wstring strMemberName = pPlayer->GetName();
			uidOffline = pProxyParty->Find(strMemberName);
		}
	}


	GServerPartyRouter* pServerRouter = gsys.pMasterServerFacade->GetPartyRouter();	
	pServerRouter->FixedPartyLogOn(uidParty, uidMember, uidOffline);
}
