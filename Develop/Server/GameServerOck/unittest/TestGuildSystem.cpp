#include "stdafx.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GGuildMgr.h"
#include "GGuildSystem.h"
#include "GGuild.h"
#include "GSharedField.h"
#include "GPlayerBattleArena.h"
#include "GFieldAttrGrid.h"
#include "GGuildSystemForMasterServer.h"
#include "AStlUtil.h"
#include "CSNameStringChecker.h"
#include "GItemMover.h"
#include "GItemHolder.h"

SUITE(GuildSystem)
{
	struct FGuildSystem : public FBaseGame
	{
		FGuildSystem()
		{
			m_pGuildStorageNPC = test.npc.SpawnNPC(m_pField);
			m_pGuildStorageIE = test.interaction.GiveNewIElementInfoForClick(m_pGuildStorageNPC->GetNPCInfo(), IT_GUILD_STORAGE);

			m_pGuest = test.player.NewPlayer(m_pField);
			m_pGuestLink = test.network.NewLink(m_pGuest);
			m_pLink->ResetCommands();
			m_pNetAgent->ResetSendCommands();

			m_pGuestLink->AddIgnoreID(MC_GUILD_JOIN_SECTOR);
			m_pGuestLink->AddIgnoreID(MC_GUILD_LEAVE_SECTOR);
			m_pGuestLink->AddIgnoreID(MC_CHAR_UPDATE_MONEY);
			m_pLink->AddIgnoreID(MC_GUILD_JOIN_SECTOR);
			m_pLink->AddIgnoreID(MC_GUILD_LEAVE_SECTOR);
			m_pLink->AddIgnoreID(MC_CHAR_UPDATE_MONEY);
		}

		~FGuildSystem()
		{
			gmgr.pGuildMgr->Clear();
		}

		GEntityPlayer*	m_pGuest;
		GEntityNPC*		m_pGuildStorageNPC;
		GNPCInteractionElementInfo*	m_pGuildStorageIE;
		MockLink*		m_pGuestLink;
		GUTHelper		m_Helper;
	};

	namespace Serialize
	{
		TEST_FIXTURE(FGuildSystem, AlreadySerialized)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gsys.pGuildSystem->Serialize(m_pPlayer);

			CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());

			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_INFO, m_pLink->GetCommand(0).GetID());
		}

		TEST_FIXTURE(FGuildSystem, FirstSerialize)
		{
			m_pPlayer->GetPlayerInfo()->nGID = SUnitTestUtil::NewGID();

			// CheckMasterServerReq		
			gsys.pGuildSystem->Serialize(m_pPlayer);
			ASSERT_EQUAL(MMC_GUILD_MEMBER_ONLINEINFO_REQ, m_pNetAgent->GetSendCommandID(0));

			// CheckMasterServerRes
			gsys.pGuildSystem->ForMasterServer().MemberOnlineInfo(m_pPlayer->GetUID(), vector<TD_GUILD_ONLINE_MEMBER_INFO>());
			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_INFO, m_pLink->GetCommand(0).GetID());
		}
	}
	
	namespace MakeTD_Guild
	{
		// 길드에 가입하지 않은 상태
		TEST_FIXTURE(FGuildSystem, NoJoinGuild)
		{
			vector<TD_GUILD> vecTDGuild;
			vector<TD_GUILD_MEMBER> vecTDGuildMember;
			gsys.pGuildSystem->MakeTD_GUILDAndTD_GUILD_MEMBER(0, vecTDGuild, vecTDGuildMember);

			CHECK_EQUAL(true, vecTDGuild.empty());
			CHECK_EQUAL(true, vecTDGuildMember.empty());
		}

		// 길드에 가입한 상태
		TEST_FIXTURE(FGuildSystem, JoinGuild)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			vector<TD_GUILD> vecTDGuild;
			vector<TD_GUILD_MEMBER> vecTDGuildMember;
			gsys.pGuildSystem->MakeTD_GUILDAndTD_GUILD_MEMBER(pGuild->GetID(), vecTDGuild, vecTDGuildMember);

			CHECK_EQUAL(1, vecTDGuild.size());
			CHECK_EQUAL(0, wcscmp(pGuild->GetName(), vecTDGuild[0].szName));
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), vecTDGuild[0].szMasterName));

			CHECK_EQUAL(1, vecTDGuildMember.size());
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), vecTDGuildMember[0].szName));
			CHECK(GMG_MASTER == vecTDGuildMember[0].nGrade);
			CHECK_EQUAL(true, vecTDGuildMember[0].bOnline);
			CHECK_EQUAL(m_pPlayer->GetField()->GetID(), vecTDGuildMember[0].nFieldID);
			CHECK_EQUAL(GetChannelID(m_pPlayer->GetField()), vecTDGuildMember[0].nChannelID);
		}
	}

	namespace CreateGuild
	{
		// 이미 길드에 가입해 있는 경우
		TEST_FIXTURE(FGuildSystem, AlreadyJoin)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			size_t nBeforeGuildSize = gmgr.pGuildMgr->GetSize();

			CHECK_EQUAL(false, gsys.pGuildSystem->Create(m_pPlayer, SUnitTestUtil::NewGuildName().c_str()));
			CHECK_EQUAL(nBeforeGuildSize, gmgr.pGuildMgr->GetSize());
		}

		// 길드명이 존재하는 경우
		TEST_FIXTURE(FGuildSystem, ExistName)
		{
			GEntityPlayer* pOther = GUTHelper::NewEntityPlayer(m_pField);
			GGuild* pGuild = m_Helper.NewGuild(pOther, SUnitTestUtil::NewGuildName().c_str());
			
			size_t nBeforeGuildSize = gmgr.pGuildMgr->GetSize();

			CHECK_EQUAL(false, gsys.pGuildSystem->Create(m_pPlayer, pGuild->GetName()));
			CHECK_EQUAL(nBeforeGuildSize, gmgr.pGuildMgr->GetSize());
		}

		// 길드명이 너무 짧은 경우
		TEST_FIXTURE(FGuildSystem, TooShortName)
		{
			wstring strGuildName;
			strGuildName.resize(CSNameStringChecker::GUILD_MIN_ENGLISH -1, 'a');

			size_t nBeforeGuildSize = gmgr.pGuildMgr->GetSize();

			CHECK_EQUAL(false, gsys.pGuildSystem->Create(m_pPlayer, strGuildName.c_str()));
			CHECK_EQUAL(nBeforeGuildSize, gmgr.pGuildMgr->GetSize());
		}

		// 길드명이 너무 긴 경우
		TEST_FIXTURE(FGuildSystem, TooLongName)
		{
			wstring strGuildName;
			strGuildName.resize(CSNameStringChecker::GUILD_MAX_ENGLISH +1, 'a');

			size_t nBeforeGuildSize = gmgr.pGuildMgr->GetSize();

			CHECK_EQUAL(false, gsys.pGuildSystem->Create(m_pPlayer, strGuildName.c_str()));
			CHECK_EQUAL(nBeforeGuildSize, gmgr.pGuildMgr->GetSize());
		}

		// 길드 생성
		TEST_FIXTURE(FGuildSystem, Create)
		{
			wstring strGuildName = L"TestGuild";

			size_t nBeforeGuildSize = gmgr.pGuildMgr->GetSize();

			m_pPlayer->GetPlayerInfo()->SetMoney(GConst::GUILD_CREATE_COST);
			int nBeforeMoney = m_pPlayer->GetMoney();
		
			CHECK_EQUAL(true, gsys.pGuildSystem->Create(m_pPlayer, strGuildName.c_str()));
			CHECK_EQUAL(nBeforeGuildSize+1, gmgr.pGuildMgr->GetSize());

			GGuild* pGuild = gmgr.pGuildMgr->Get(m_pPlayer->GetGID());
			CHECK(NULL != pGuild);
			CHECK_EQUAL(0, wcscmp(strGuildName.c_str(), pGuild->GetName()));
			CHECK_EQUAL(true, pGuild->IsGuildMaster(m_pPlayer->GetCID()));
			CHECK_EQUAL(m_pPlayer->GetGID(), pGuild->GetID());

			GGuildMember* pMember = pGuild->GetMember(m_pPlayer->GetCID());
			CHECK(NULL != pMember);
			CHECK_EQUAL(m_pPlayer->GetCID(), pMember->GetCID());
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetPlayerInfo()->szName, pMember->GetName()));
			CHECK(GMG_MASTER == pMember->GetGrade());
			CHECK_EQUAL(m_pPlayer->GetMoney(), nBeforeMoney - GConst::GUILD_CREATE_COST);

			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_CREATE, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUIID(), m_pLink->GetParam<unsigned short>(0, 0));
			CHECK_EQUAL(0, wcscmp(strGuildName.c_str(), m_pLink->GetParam<const wchar_t*>(0, 1)));
		}
	}

	namespace Invite_Req
	{
		// 초대하는자가 길드에 들지 않은 경우
		TEST_FIXTURE(FGuildSystem, InvitorHaveNoGuild)
		{
			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
		}

		// 초대하는자가 길드 마스터가 아닌 경우
		TEST_FIXTURE(FGuildSystem, InvitorIsNotGuildMaster)
		{
			GEntityPlayer* pGuildMaster = GUTHelper::NewEntityPlayer(m_pField);
			GGuild* pGuild = m_Helper.NewGuild(pGuildMaster, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pPlayer->GetCID(), m_pPlayer->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
		}

		// 초대받는자가 길드에 이미 가입한 경우
		TEST_FIXTURE(FGuildSystem, GuestAlreadyJoinGuild)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
		}

		// 초대받는자가 결투 중인 경우
		TEST_FIXTURE(FGuildSystem, GuestDueling)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			GEntityPlayer* pDuelPlayer = m_Helper.NewEntityPlayer(m_pField);
			GUTHelper_Duel::NewMockDuel_Fighting(m_pGuest, pDuelPlayer);

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
		}

		// 초대받는자가 전장에 있는 경우
		TEST_FIXTURE(FGuildSystem, GuestInBattleArena)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			m_pGuest->GetPlayerBattleArena().SetBattleArenaUID(SUnitTestUtil::NewUID());

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
		}

		// 초대받는자가 PVP지역 있는 경우
		TEST_FIXTURE(FGuildSystem, GuestInPvpArea)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			PVP_AREA_INFO infoPVP;
			infoPVP.rcArea = MRect(-500, -500, 500, 500);
			GFieldInfo* pInfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
			pInfo->m_PvPAreas.push_back(infoPVP);
			pInfo->GetAttrGrid()->Init();
			pInfo->Cooking();
			m_pGuest->SetPos(vec3(0, 0, 0));

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));

			m_pField->GetInfo()->GetAttrGrid()->Fini();
		}

		// 이미 최대 길드 인원인 경우
		TEST_FIXTURE(FGuildSystem, MaxMemberCount)
		{


			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			
			for (int i=0; i<MAX_GUILD_MEMBER_SIZE-1; ++i)
			{
				GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
				pGuild->InsertMember(m_pPlayer->GetAID(), pPlayer->GetCID(), pPlayer->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);
			}
			
			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
		}

		// 길드 초대
		TEST_FIXTURE(FGuildSystem, Invite_Req)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			CHECK_EQUAL(true, gsys.pGuildSystem->Invite_Req(m_pPlayer, m_pGuest));
			CHECK_EQUAL(m_pPlayer->GetCID(), gmgr.pGuildMgr->GetInvitorCID(m_pGuest->GetCID()));

			CHECK(m_pGuestLink->GetCommandCount() == 1);
			CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_INVITE_NOTIFY);
			CHECK_EQUAL(m_pPlayer->GetUID(), m_pGuestLink->GetParam<MUID>(0, 0));
			CHECK_EQUAL(0, wcscmp(pGuild->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 1)));
		}
	}

	namespace Invite_Res
	{
		// 초대한 길드가 없을 경우
		TEST_FIXTURE(FGuildSystem, NoExistGuild)
		{
			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pGuest, true));
		}

		// 초대받은 적이 없는 경우
		TEST_FIXTURE(FGuildSystem, NotInvited)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pGuest, true));
		}

		// 이미 길드에 가입한 경우
		TEST_FIXTURE(FGuildSystem, GuestAlreadyJoinGuild)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pGuest, true));
		}

		// 이미 최대 길드 인원인 경우
		TEST_FIXTURE(FGuildSystem, MaxMemberCount)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());

			for (int i=0; i<MAX_GUILD_MEMBER_SIZE-1; ++i)
			{
				GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
				pGuild->InsertMember(m_pPlayer->GetAID(), pPlayer->GetCID(), pPlayer->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);
			}

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pPlayer, true));
		}

		// 초대받는자가 결투 중인 경우
		TEST_FIXTURE(FGuildSystem, GuestDueling)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());

			GEntityPlayer* pDuelPlayer = m_Helper.NewEntityPlayer(m_pField);
			GUTHelper_Duel::NewMockDuel_Fighting(m_pGuest, pDuelPlayer);

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pGuest, true));
		}

		// 초대받는자가 전장에 있는 경우
		TEST_FIXTURE(FGuildSystem, GuestInBattleArena)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());

			m_pGuest->GetPlayerBattleArena().SetBattleArenaUID(SUnitTestUtil::NewUID());

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pGuest, true));
		}

		// 초대받는자가 PVP지역 있는 경우
		TEST_FIXTURE(FGuildSystem, GuestInPvpArea)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());

			PVP_AREA_INFO infoPVP;
			infoPVP.rcArea = MRect(-500, -500, 500, 500);
			GFieldInfo* pInfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
			pInfo->m_PvPAreas.push_back(infoPVP);
			pInfo->GetAttrGrid()->Init();
			pInfo->Cooking();
			m_pGuest->SetPos(vec3(0, 0, 0));

			CHECK_EQUAL(false, gsys.pGuildSystem->Invite_Res(m_pGuest, true));

			m_pField->GetInfo()->GetAttrGrid()->Fini();
		}

		// 초대를 거절한 경우
		TEST_FIXTURE(FGuildSystem, Reject)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());

			CHECK_EQUAL(true, gsys.pGuildSystem->Invite_Res(m_pGuest, false));

			CHECK(m_pLink->GetCommandCount() == 1);
			CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_INVITE_REJECT);
			CHECK_EQUAL(m_pLink->GetParam<MUID>(0, 0), m_pGuest->GetUID());
			CHECK(m_pGuestLink->GetCommandCount() == 0);
		}		

		// 초대를 승낙한 경우
		TEST_FIXTURE(FGuildSystem, Accept)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			gmgr.pGuildMgr->Invite(m_pGuest->GetCID(), m_pPlayer->GetCID());
			
			// CheckMasterServerReq
			CHECK_EQUAL(true, gsys.pGuildSystem->Invite_Res(m_pGuest, true));
			ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_JOIN);
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), m_pGuest->GetCID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 1), m_pGuest->GetAID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), pGuild->GetID());
			CHECK_EQUAL(0, wcscmp(m_pNetAgent->GetSendParam<const wchar_t*>(0, 3), m_pGuest->GetName()));
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 4), m_pGuest->GetLevel());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 5), m_pGuest->GetFieldID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 6), GetChannelID(m_pGuest->GetField()));

			// CheckMasterServerRes
			gsys.pGuildSystem->ForMasterServer().JoinSync(m_pGuest->GetCID(), (AID)m_pGuest->GetAID(), pGuild->GetID(), m_pGuest->GetName(), m_pGuest->GetLevel(), m_pGuest->GetFieldID(), GetChannelID(m_pGuest->GetField()));
			CHECK_EQUAL(true, pGuild->IsExist(m_pGuest->GetCID()));
			CHECK_EQUAL(m_pGuest->GetGID(), pGuild->GetID());

			CHECK(m_pGuestLink->GetCommandCount() == 1);
			CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_JOIN_ME);
			CHECK(m_pLink->GetCommandCount() == 1);
			CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_JOIN_OTHER);
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetPlayerInfo()->szName, m_pLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(m_pGuest->GetLevel(), m_pLink->GetParam<int>(0, 1));
			CHECK_EQUAL(m_pGuest->GetField()->GetID(), m_pLink->GetParam<int>(0, 2));
			CHECK_EQUAL(GetChannelID(m_pGuest->GetField()), m_pLink->GetParam<int>(0, 3));
		}
	}

	namespace Leave
	{
		// 길드에 가입하지 않은 경우
		TEST_FIXTURE(FGuildSystem, NoJoinGuild)
		{
			CHECK_EQUAL(false, gsys.pGuildSystem->Leave(m_pPlayer));
		}

		// 길마는 길드 탈퇴 할수 없다.
		TEST_FIXTURE(FGuildSystem, GuildMasterCannotLeave)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());			
			CHECK_EQUAL(false, gsys.pGuildSystem->Leave(m_pPlayer));
		}

		// 길드 탈퇴
		TEST_FIXTURE(FGuildSystem, Leave)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			// CheckMasterServerReq
			CHECK_EQUAL(true, gsys.pGuildSystem->Leave(m_pGuest));
			ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_LEAVE);
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), m_pGuest->GetCID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 1), pGuild->GetID());

			// CheckMasterServerRes
			gsys.pGuildSystem->ForMasterServer().LeaveSync(m_pGuest->GetCID(), pGuild->GetID());
			CHECK_EQUAL(false, pGuild->IsExist(m_pGuest->GetCID()));
			CHECK_EQUAL(0, m_pGuest->GetGID());

			CHECK_EQUAL(1, m_pGuestLink->GetCommandCount());
			CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_LEAVE);
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_LEAVE);
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));
		}
	}

	namespace Kick
	{
		// 길마가 아닌 경우
		TEST_FIXTURE(FGuildSystem, NoGuildMaster)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Kick(m_pGuest, m_pPlayer->GetName()));
		}

		// 길마를 강퇴하는 경우
		TEST_FIXTURE(FGuildSystem, GuildMasterCannotKick)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Kick(m_pPlayer, m_pPlayer->GetName()));
		}

		// 길드 강퇴
		TEST_FIXTURE(FGuildSystem, Kick)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			// CheckMasterServerReq
			CHECK_EQUAL(true, gsys.pGuildSystem->Kick(m_pPlayer, m_pGuest->GetName()));
			ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_KICK);
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), m_pGuest->GetCID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 1), pGuild->GetID());

			// CheckMasterServerRes
			gsys.pGuildSystem->ForMasterServer().KickSync(m_pGuest->GetCID(), pGuild->GetID());
			CHECK_EQUAL(false, pGuild->IsExist(m_pGuest->GetCID()));
			CHECK_EQUAL(0, m_pGuest->GetGID());

			CHECK_EQUAL(1, m_pGuestLink->GetCommandCount());
			CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_KICK);
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_KICK);
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));
		}
	}

	namespace Destroy
	{
		// 길드에 가입하지 않은 경우
		TEST_FIXTURE(FGuildSystem, NoJoinGuild)
		{
			CHECK_EQUAL(false, gsys.pGuildSystem->Destroy(m_pPlayer));
		}

		// 길마가 아닌 경우
		TEST_FIXTURE(FGuildSystem, NoGuildMaster)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Destroy(m_pGuest));
		}

		// 길드원이 남아 있는 경우
		TEST_FIXTURE(FGuildSystem, RemainedMember)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->Destroy(m_pPlayer));
		}

		// 길드 해체
		TEST_FIXTURE(FGuildSystem, Destroy)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			size_t nBeforeGuildSize = gmgr.pGuildMgr->GetSize();
			int nGID = pGuild->GetID();
			const wchar_t* szName = pGuild->GetName();

			// CheckMasterServerReq
			CHECK_EQUAL(true, gsys.pGuildSystem->Destroy(m_pPlayer));
			ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_DESTROY);
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), pGuild->GetID());

			// CheckMasterServerRes
			gsys.pGuildSystem->ForMasterServer().DestroySync(pGuild->GetID());
			CHECK_EQUAL(false, gmgr.pGuildMgr->IsExist(nGID));
			CHECK_EQUAL(false, gmgr.pGuildMgr->IsExist(szName));
			CHECK_EQUAL(nBeforeGuildSize-1, gmgr.pGuildMgr->GetSize());
			CHECK_EQUAL(0, m_pPlayer->GetGID());			

			CHECK(m_pLink->GetCommandCount() == 1);
			CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_DESTROY);
		}
	}

	// 길드원 접속
	TEST_FIXTURE(FGuildSystem, OnLine)
	{
		GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
		pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

		// CheckMasterServerReq
		gsys.pGuildSystem->OnLine(m_pPlayer);
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_ONLINE);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), m_pPlayer->GetCID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 1), pGuild->GetID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), m_pPlayer->GetFieldID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 3), GetChannelID(m_pPlayer->GetField()));

		// CheckMasterServerRes
		gsys.pGuildSystem->ForMasterServer().OnLineSync(m_pPlayer->GetCID(), pGuild->GetID(), m_pPlayer->GetFieldID(), GetChannelID(m_pPlayer->GetField()));
		CHECK_EQUAL(true, pGuild->IsOnlineMember(m_pPlayer->GetCID()));

		CHECK(m_pLink->GetCommandCount() == 1);
		CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_ONLINE);
		CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));
		CHECK_EQUAL(m_pPlayer->GetField()->GetID(), m_pLink->GetParam<int>(0, 1));
		CHECK_EQUAL(GetChannelID(m_pPlayer->GetField()), m_pLink->GetParam<int>(0, 2));		
		CHECK(m_pGuestLink->GetCommandCount() == 1);
		CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_ONLINE);
		CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
		CHECK_EQUAL(m_pPlayer->GetField()->GetID(), m_pGuestLink->GetParam<int>(0, 1));
		CHECK_EQUAL(GetChannelID(m_pPlayer->GetField()), m_pGuestLink->GetParam<int>(0, 2));
	}

	// 길드원 종료
	TEST_FIXTURE(FGuildSystem, OffLine)
	{
		GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
		pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

		// CheckMasterServerReq
		gsys.pGuildSystem->OffLine(m_pPlayer);
		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_OFFLINE);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), m_pPlayer->GetCID());
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 1), pGuild->GetID());

		// CheckMasterServerRes
		gsys.pGuildSystem->ForMasterServer().OffLineSync(m_pPlayer->GetCID(), pGuild->GetID());
		CHECK_EQUAL(false, pGuild->IsOnlineMember(m_pPlayer->GetCID()));

		CHECK(m_pLink->GetCommandCount() == 1);
		CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_OFFLINE);
		CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));
		CHECK(m_pGuestLink->GetCommandCount() == 1);
		CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_OFFLINE);
		CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
	}	

	namespace ChangeMaster
	{
		// 요청하는 사람이 길마가 아닌 경우
		TEST_FIXTURE(FGuildSystem, OldMasterIsNotMaster)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			CHECK_EQUAL(false, gsys.pGuildSystem->ChangeMaster(m_pGuest, m_pPlayer->GetName()));
		}

		// 새로 길마가 될 사람이 길드원이 아닌 경우 
		TEST_FIXTURE(FGuildSystem, NewMasterIsNotMember)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());

			CHECK_EQUAL(false, gsys.pGuildSystem->ChangeMaster(m_pPlayer, m_pGuest->GetName()));
		}

		// 길마 변경
		TEST_FIXTURE(FGuildSystem, ChangeMaster)
		{
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
			pGuild->InsertMember(m_pPlayer->GetAID(), m_pGuest->GetCID(), m_pGuest->GetPlayerInfo()->szName, m_pPlayer->GetPlayerInfo()->nLevel, GMG_NORMAL);

			// CheckMasterServerReq
			CHECK_EQUAL(true, gsys.pGuildSystem->ChangeMaster(m_pPlayer, m_pGuest->GetName()));
			ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GUILD_CHANGE_MASTER);
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), m_pPlayer->GetCID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 1), m_pGuest->GetCID());
			CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 2), pGuild->GetID());

			// CheckMasterServerRes
			gsys.pGuildSystem->ForMasterServer().ChangeMasterSync(m_pPlayer->GetCID(), m_pGuest->GetCID(), pGuild->GetID());
			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_CHANGE_MASTER);
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));			
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetName(), m_pLink->GetParam<const wchar_t*>(0, 1)));
			CHECK_EQUAL(1, m_pGuestLink->GetCommandCount());
			CHECK(m_pGuestLink->GetCommand(0).GetID() == MC_GUILD_CHANGE_MASTER);
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));			
			CHECK_EQUAL(0, wcscmp(m_pGuest->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 1)));			
		}
	}

	TEST_FIXTURE(FGuildSystem, ShowStorage)
	{
		GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str());
		pGuild->SetStorageMoney(3000);
		GItem* pItem = test.item.GiveNewItemToGuildStorage(m_pPlayer);

		gsys.pGuildSystem->ShowStorage(m_pPlayer);

		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK(m_pLink->GetCommand(0).GetID() == MC_GUILD_SHOW_STORAGE_UI);
		CHECK_EQUAL(pGuild->GetStorageMoney(), m_pLink->GetParam<int>(0, 0));
		vector<TD_ITEM> vecTDItem;
		ASSERT_EQUAL(true, m_pLink->GetCommand(0).GetBlob(vecTDItem, 1));
		CHECK(pItem->ToTD_ITEM() == vecTDItem.front());
	}

	namespace DepositStorageMoney
	{		
		TEST_FIXTURE(FGuildSystem, NotEnoughMoney)
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
			test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);
			m_pPlayer->GetPlayerInfo()->SetMoney(5000);
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), 1000, m_pGuest);			

			CHECK_EQUAL(false, gsys.pGuildSystem->DepositStorageMoney(m_pPlayer, 5001));
		}

		TEST_FIXTURE(FGuildSystem, OverDepositMoney)
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
			test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);
			m_pPlayer->GetPlayerInfo()->SetMoney(MAX_MONEY);
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), 1000, m_pGuest);
			
			CHECK_EQUAL(false, gsys.pGuildSystem->DepositStorageMoney(m_pPlayer, MAX_MONEY));
		}

		TEST_FIXTURE(FGuildSystem, DepositMoney)
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
			test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);

			int nBeforeMoney = 5000;
			int nBeforeGuildStorageMoney = 1000;
			int nDepositMoney = 5000;			

			m_pPlayer->GetPlayerInfo()->SetMoney(nBeforeMoney);
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), nBeforeGuildStorageMoney, m_pGuest);

			// CheckLocal
			CHECK_EQUAL(true, gsys.pGuildSystem->DepositStorageMoney(m_pPlayer, nDepositMoney));
			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_DEPOSIT_STORAGEMONEY, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(nDepositMoney, m_pLink->GetParam<int>(0, 1));
			CHECK_EQUAL(nBeforeGuildStorageMoney + nDepositMoney, m_pLink->GetParam<int>(0, 2));
			ASSERT_EQUAL(MMC_GUILD_DEPOSIT_STORAGEMONEY, m_pNetAgent->GetSendCommandID(0));
			CHECK_EQUAL(m_pPlayer->GetCID(), m_pNetAgent->GetSendParam<int>(0, 0));
			CHECK_EQUAL(pGuild->GetID(), m_pNetAgent->GetSendParam<int>(0, 1));
			CHECK_EQUAL(nDepositMoney, m_pNetAgent->GetSendParam<int>(0, 2));
			CHECK_EQUAL(nBeforeGuildStorageMoney + nDepositMoney, m_pNetAgent->GetSendParam<int>(0, 3));
			CHECK_EQUAL(nBeforeGuildStorageMoney + nDepositMoney, pGuild->GetStorageMoney());
			CHECK_EQUAL(nBeforeMoney - nDepositMoney, m_pPlayer->GetMoney());

			// CheckSync
			gsys.pGuildSystem->ForMasterServer().DepositStorageMoneySync(m_pPlayer->GetCID(), pGuild->GetID(), nDepositMoney, nBeforeGuildStorageMoney + nDepositMoney);
			CHECK_EQUAL(2, m_pGuestLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_DEPOSIT_STORAGEMONEY, m_pGuestLink->GetCommand(0).GetID());
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(nDepositMoney, m_pGuestLink->GetParam<int>(0, 1));
			CHECK_EQUAL(nBeforeGuildStorageMoney + nDepositMoney, m_pGuestLink->GetParam<int>(0, 2));			
		}
	}

	namespace WithdrawStorageMoney
	{
		TEST_FIXTURE(FGuildSystem, NotEnoughStorageMoney)
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
			test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);
			m_pPlayer->GetPlayerInfo()->SetMoney(5000);
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), 1000, m_pGuest);

			CHECK_EQUAL(false, gsys.pGuildSystem->WithdrawStorageMoney(m_pPlayer, 1001));
		}

		TEST_FIXTURE(FGuildSystem, OverWithdrawMoney)
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
			test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);
			m_pPlayer->GetPlayerInfo()->SetMoney(5000);
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), MAX_MONEY, m_pGuest);
			
			CHECK_EQUAL(false, gsys.pGuildSystem->WithdrawStorageMoney(m_pPlayer, MAX_MONEY));
		}

		TEST_FIXTURE(FGuildSystem, WithdrawMoney)
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
			test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);

			int nBeforeMoney = 5000;
			int nBeforeGuildStorageMoney = 1000;
			int nWithdrawMoney = 1000;

			m_pPlayer->GetPlayerInfo()->SetMoney(nBeforeMoney);
			GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), nBeforeGuildStorageMoney, m_pGuest);

			// CheckLocal
			CHECK_EQUAL(true, gsys.pGuildSystem->WithdrawStorageMoney(m_pPlayer, nWithdrawMoney));
			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_WITHDRAW_STORAGEMONEY, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(nWithdrawMoney, m_pLink->GetParam<int>(0, 1));
			CHECK_EQUAL(nBeforeGuildStorageMoney - nWithdrawMoney, m_pLink->GetParam<int>(0, 2));			
			ASSERT_EQUAL(MMC_GUILD_WITHDRAW_STORAGEMONEY, m_pNetAgent->GetSendCommandID(0));
			CHECK_EQUAL(m_pPlayer->GetCID(), m_pNetAgent->GetSendParam<int>(0, 0));
			CHECK_EQUAL(pGuild->GetID(), m_pNetAgent->GetSendParam<int>(0, 1));
			CHECK_EQUAL(nWithdrawMoney, m_pNetAgent->GetSendParam<int>(0, 2));
			CHECK_EQUAL(nBeforeGuildStorageMoney - nWithdrawMoney, m_pNetAgent->GetSendParam<int>(0, 3));
			CHECK_EQUAL(nBeforeGuildStorageMoney - nWithdrawMoney, pGuild->GetStorageMoney());
			CHECK_EQUAL(nBeforeMoney + nWithdrawMoney, m_pPlayer->GetMoney());

			// CheckSync
			gsys.pGuildSystem->ForMasterServer().WithdrawStorageMoneySync(m_pPlayer->GetCID(), pGuild->GetID(), nWithdrawMoney, nBeforeGuildStorageMoney - nWithdrawMoney);
			CHECK_EQUAL(2, m_pGuestLink->GetCommandCount());
			CHECK_EQUAL(MC_GUILD_WITHDRAW_STORAGEMONEY, m_pGuestLink->GetCommand(0).GetID());
			CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
			CHECK_EQUAL(nWithdrawMoney, m_pGuestLink->GetParam<int>(0, 1));
			CHECK_EQUAL(nBeforeGuildStorageMoney - nWithdrawMoney, m_pGuestLink->GetParam<int>(0, 2));			
		}
	}

	TEST_FIXTURE(FGuildSystem, MoveStorageItem)
	{
		test.interaction.MakeInteractionState(m_pPlayer, m_pGuildStorageNPC, m_pGuildStorageIE);
		test.interaction.MakeInteractionState(m_pGuest, m_pGuildStorageNPC, m_pGuildStorageIE);

		GGuild* pGuild = m_Helper.NewGuild(m_pPlayer, SUnitTestUtil::NewGuildName().c_str(), 0, m_pGuest);
		GItem* pItem = test.item.GiveNewItem(m_pPlayer);		

		SH_ITEM_SLOT_TYPE nFromSlotType = pItem->m_nSlotType;
		int nFromSlotID = pItem->m_nSlotID;
		SH_ITEM_SLOT_TYPE nToSlotType = SLOTTYPE_GUILDSTORAGE;
		int nToSlotID = 0;
		int nAmount = pItem->GetAmount();

		// CheckLocal
		CHECK_EQUAL(true, gsys.pItemSystem->GetMover().Move(m_pPlayer, nFromSlotType, nFromSlotID, pItem->GetID(), nToSlotType, nToSlotID, 0, nAmount));
		// TestItemMover에서 길드보관함까지 이동 로직을 체크하고 있어서 여기선 별도의 체크를 하지 않는다.
		ASSERT_EQUAL(MMC_GUILD_MOVE_STORAGEITEM, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetCID(), m_pNetAgent->GetSendParam<int>(0, 0));
		CHECK_EQUAL(pGuild->GetID(), m_pNetAgent->GetSendParam<int>(0, 1));
		{
			vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE> vecTDItem;
			m_pNetAgent->GetSendCommand(0).GetBlob(vecTDItem, 2);
			CHECK_EQUAL(vecTDItem.front().nItemID, pItem->GetID());
		}

		// CheckSync
		gsys.pGuildSystem->ForMasterServer().MoveStorageItemSync(m_pPlayer->GetCID(), pGuild->GetID(), vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>(1, pItem->ToTD_PLAYER_GAME_DATA_ITEM_INSTANCE()));
		CHECK_EQUAL(2, m_pGuestLink->GetCommandCount());
		CHECK_EQUAL(MC_GUILD_MOVE_STORAGEITEM, m_pGuestLink->GetCommand(0).GetID());
		CHECK_EQUAL(0, wcscmp(m_pPlayer->GetName(), m_pGuestLink->GetParam<const wchar_t*>(0, 0)));
		{
			vector<TD_ITEM> vecTDItem;
			m_pGuestLink->GetCommand(0).GetBlob(vecTDItem, 1);
			CHECK_EQUAL(vecTDItem.front().m_nItemID, pItem->GetID());
		}		
	}
}
