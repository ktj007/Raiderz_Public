#include "StdAfx.h"
#include "XCmdHandler_Party.h"
#include "XObjectManager.h"
#include "XMyPlayer.h"
#include "XStrings.h"
#include "CSDef_Loot.h"
#include "CCommandResultTable.h"
#include "XInteractionInfo.h"
#include "XPost_Action.h"
#include "XGameState.h"
#include "XSensorManager.h"
#include "XPost_Comm.h"


XCmdHandler_Party::XCmdHandler_Party(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_PARTY_INVITE,						OnInviteParty);
	SetCmdHandler(MC_PARTY_INVITE_QUESTION,				OnInviteQuestion);
	SetCmdHandler(MC_PARTY_INVITE_FOR_ME_QUESTION,		OnInviteForMeQuestion);
	SetCmdHandler(MC_PARTY_REFRESH_INFO_ALL,			OnRefreshInfoAll);
	SetCmdHandler(MC_PARTY_REFRESH_INFO,				OnRefreshInfo);
	SetCmdHandler(MC_PARTY_NOTIFY_JOIN,					OnNotifyJoin);
	SetCmdHandler(MC_PARTY_NOTIFY_LEAVE,				OnNotifyLeave);
	//SetCmdHandler(MC_PARTY_CHANGE_NAME,				OnChangeName);
	SetCmdHandler(MC_PARTY_CHANGE_LEADER,				OnChangeLeader);
	SetCmdHandler(MC_PARTY_CHANGE_LOOTING_RULE,			OnChangeLootingRule);
	//SetCmdHandler(MC_PARTY_CHANGE_QUESTID,			OnChangeQuestID);	
	SetCmdHandler(MC_PARTY_CHANGE_LEADER_NOTIFY,		OnChangeLeaderNotify);	

	//SetCmdHandler(MC_AUTOPARTY_CHECK_SENSOR,			OnCheckSensor);
	SetCmdHandler(MC_AUTOPARTY_CHANGE_STATE,			OnChangeState);
}

MCommandResult XCmdHandler_Party::OnInviteParty(MCommand* pCommand, MCommandHandler* pHandler)
{
	CCommandResultTable nRet;
	if (pCommand->GetParameter(&nRet,	0, MPT_INT)==false) return CR_ERROR;
	
	if (CR_SUCCESS == nRet)
	{
		global.ui->OnSystemMsg( XGetStr( L"PARTY_ACCEPTINVITATION"));
	}
	else
	{	
		global.ui->OnSystemMsg(XCmdResultStr(nRet));
	}		

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnInviteQuestion(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidInvitor;
	wstring strInvitorName;
	bool bCombat;

	if (pCommand->GetParameter(&uidInvitor,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(strInvitorName,	1, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&bCombat,		2, MPT_BOOL)==false) return CR_ERROR;


	if (XGETCFG_GAME_REJECT_PARTY)
	{
		global.ui->OnSystemMsg( XGetStr( L"PARTY_DECLINEINVITATIONBYOPTION"), FSParam(strInvitorName));
		
		XPostPartyRequestInviteQuestionRespond(false);
	}
	else if (bCombat)
	{
		global.ui->OnSystemMsg( XGetStr( L"PARTY_DECLINEINVITATIONBYDUEL"), FSParam(strInvitorName));

		XPostPartyRequestInviteQuestionRespond(false);
	}
	else 
	{
		wstring strResult = FormatString(XGetStr(L"SMSG_PARTY_INVITED"), FSParam(strInvitorName));
		global.ui->OnPresentationLower(strResult, PRESENTATION_ICON_INFORMATION, true);

		gvar.Game.pMyPlayer->SafeStop();

		XGetInteractionInfo().SetRequestPlayerUID(uidInvitor);
		gvar.Game.bPartyReq	= true;

		if(global.script)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "PARTY", "INVITED");
		}
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnInviteForMeQuestion(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidInvitor;
	wstring strInvitorName;
	bool bCombat;
	
	if (pCommand->GetParameter(&uidInvitor,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(strInvitorName,	1, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&bCombat,		2, MPT_BOOL)==false) return CR_ERROR;


	if (XGETCFG_GAME_REJECT_PARTY)
	{
		global.ui->OnSystemMsg( XGetStr( L"PARTY_DECLINEREQUESTBYOPTION"), FSParam(strInvitorName));

		XPostPartyRequestInviteForMeQuestionRespond(false);
	}
	else if (bCombat)
	{
		global.ui->OnSystemMsg( XGetStr( L"PARTY_DECLINEREQUESTBYDUEL"), FSParam(strInvitorName));

		XPostPartyRequestInviteForMeQuestionRespond(false);
	}
	else
	{
		global.ui->OnSystemMsg( XGetStr( L"PARTY_REQUESTJOIN"), FSParam(strInvitorName));
		global.ui->OnGuideMsg(GUIDE_MSG_LUA_RECV_INVITE_PARTY);

		if(global.script)
			global.script->GetGlueGameEvent().OnPartyDemandRequireWindowShow();
	}


	return CR_TRUE;
}


MCommandResult XCmdHandler_Party::OnRefreshInfoAll(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 확인
	TD_PARTY* pPartyInfo;
	int nCount;
	vector<TD_PARTY_MEMBER> vecMemberInfo;
	vector<vector<int> >	vecMemberBuffID;	

	if (!pCommand->GetBlob(pPartyInfo, nCount, 0))		return CR_ERROR;	
	if (!pCommand->GetBlob(vecMemberInfo, 1))			return CR_ERROR;			
	
	// 멤버 수 만큼 버프 정보가 들어와야한다.
	for(uint32 i = 0; i < vecMemberInfo.size(); i++)
	{	
		vector<int> vecBuffID;		
		if (!pCommand->GetBlob(vecBuffID, 2 + i))		return CR_ERROR;
		
		vecMemberBuffID.push_back(vecBuffID);
	}


	// 로그온 멤버 확인
	for each(const TD_PARTY_MEMBER& partyMember in vecMemberInfo)
	{	
		if (gvar.MyInfo.Party.IsPartyMemberReconnect(partyMember))
		{
			wstring strMsg = CSFormatString::Format( XGetStr( L"PARTY_MEMBERONLINE"), FSParam(partyMember.szName));
			global.script->GetGlueGameEvent().OnMessageBox(MLocale::ConvUTF16ToAnsi(strMsg));
		}
	}


	// 파티정보 새로고침
	gvar.MyInfo.Party.Init(*pPartyInfo);

	for(uint32 i = 0; i < vecMemberInfo.size(); i++)
	{
		TD_PARTY_MEMBER& partyMember = vecMemberInfo[i];		
		gvar.MyInfo.Party.AddMember(partyMember);

		for each(int nBuffID in vecMemberBuffID[i])
		{
			MUID uidMember = partyMember.m_uidPlayer;
			gvar.MyInfo.Party.SetPartyMemberBuffID(uidMember, nBuffID);
		}
	}

	global.script->GetGlueGameEvent().OnPartyRefresh();


	// 자동파티 설정 새로고침
	int nQuestID = pPartyInfo->m_partySetting.m_nAutoPartyQuestID;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo)
	{
		if ( pQuestInfo->nAutoPartyMemberLimit <= gvar.MyInfo.Party.GetMemberCounter())
			gvar.MyInfo.Party.SetPartyState(nQuestID, AUTOPARTY_PAUSE);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnRefreshInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	MCommandParameter* pParam;
	pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;
	TD_PARTY_MEMBER* pPartyMembers = (TD_PARTY_MEMBER*)pParam->GetPointer();
	if (pParam->GetBlobCount() != 1)	return CR_ERROR;

	gvar.MyInfo.Party.ChangeStatus(pPartyMembers[0]);

	pParam = pCommand->GetParameter(1);
	int* pBuffList = (int*)pParam->GetPointer();
	for (int i=0; i<pParam->GetBlobCount(); i++)
	{
		//mlog("buffID %d = %d\n", i, pBuffList[i]);
		gvar.MyInfo.Party.SetPartyMemberBuffID(pPartyMembers[0].m_uidPlayer
			, pBuffList[i]);
	}

	//if (global.ui)	global.ui->PartyUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnNotifyJoin(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	wstring strName;
	if (pCommand->GetParameter(&uidPlayer,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(strName,			1, MPT_WSTR)==false) return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr( L"PARTY_ENTEREDMEMBER"), FSParam(strName));

	if(global.script)
	{
		wstring strMsg;		

		if (uidPlayer == XGetMyUID())
		{
			strMsg = CSFormatString::Format( XGetStr( L"PARTY_ENTERED"));

			if ( global.sound)
				global.sound->Play("party");
		}
		else
			strMsg = CSFormatString::Format( XGetStr( L"PARTY_ENTEREDMEMBER"), FSParam(strName));

		global.script->GetGlueGameEvent().OnMessageBox(MLocale::ConvUTF16ToAnsi(strMsg));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnNotifyLeave(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,		0, MPT_UID)==false) return CR_ERROR;

	if (uidPlayer == XGetMyUID())
	{
		gvar.MyInfo.Party.Clear();
		global.ui->OnSystemMsg( XGetStr( L"PARTY_LEFT"));

		global.script->GetGlueGameEvent().OnPartyRefresh();
	}
	else
	{
		XPartyMember Member;
		if (gvar.MyInfo.Party.Find(uidPlayer, Member) == false)
			return CR_TRUE;

		wstring strMsg = CSFormatString::Format( XGetStr( L"PARTY_LEFTMEMBER"), FSParam(Member.m_strName));
		global.ui->OnSystemMsg(strMsg.c_str());

		if(global.script)
		{
			global.script->GetGlueGameEvent().OnMessageBox(MLocale::ConvUTF16ToAnsi(strMsg));
		}
	}

	if (gvar.MyInfo.Party.nQuestIDWithCurrentSensor == 0) 
	{
		gvar.MyInfo.Party.SetPartyState(0, AUTOPARTY_STANDBY);
	}
	else
	{
		gvar.MyInfo.Party.SetPartyState(0, AUTOPARTY_PAUSE);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnChangeLeader(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidNewLeader;
	if (!pCommand->GetParameter(&uidNewLeader, 0, MPT_UID))	return CR_ERROR;

	gvar.MyInfo.Party.ChangeLeader(uidNewLeader);
	global.script->GetGlueGameEvent().OnPartyRefresh();

	XPartyMember Member;
	if (gvar.MyInfo.Party.Find(uidNewLeader, Member) == false)
		return CR_TRUE;

	global.ui->OnSystemMsg( XGetStr( L"PARTY_CHANGEDLEADER"), FSParam(Member.m_strName));

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnChangeLootingRule(MCommand* pCommand, MCommandHandler* pHandler)
{
	int8 nCommon;
	int8 nRare;
	int8 nRareFor;

	if (!pCommand->GetParameter(&nCommon, 0, MPT_CHAR))		return CR_ERROR;
	if (!pCommand->GetParameter(&nRare, 1, MPT_CHAR))		return CR_ERROR;
	if (!pCommand->GetParameter(&nRareFor, 2, MPT_CHAR))	return CR_ERROR;


	LOOTING_RULE_DATA rule;
	rule.m_nLRC = (LOOTING_RULE_COMMON)nCommon;
	rule.m_nLRR = (LOOTING_RULE_RARE)nRare;
	rule.m_nLRRF = (LOOTING_RULE_RARE_FOR)nRareFor;

	LOOTING_RULE_DATA oldrule = gvar.MyInfo.Party.GetPartySetting()->m_lootingRuleData;

	const wchar_t* rulecommon[4] = {L"", XGetStr( L"LOOTINGRULE_FREEFORALL"), XGetStr( L"LOOTINGRULE_ROUNDROBIN"), XGetStr( L"LOOTINGRULE_DESIGNATELOOTER") };
	const wchar_t* rulerare[4] = {L"", XGetStr( L"LOOTINGRULE_FREEFORALL"), XGetStr( L"LOOTINGRULE_ROLLDICE"), XGetStr( L"LOOTINGRULE_DESIGNATELOOTER")};
	const wchar_t* rulerarefor[4] = {L"", XGetStr( L"RAREFOR_RAREORBETTER"), XGetStr( L"RAREFOR_LEGENDARYORBETTER"), XGetStr( L"RAREFOR_EPIC")};

	const wchar_t* pName = gvar.MyInfo.Party.GetPartyMemberName(gvar.MyInfo.Party.GetLeader());

	if (rule.m_nLRC != oldrule.m_nLRC)
		global.ui->OnSystemMsg( XGetStr( L"PARTY_GENERALLOOTINGRULE"), FSParam(pName, rulecommon[nCommon]));
	if (rule.m_nLRR != oldrule.m_nLRR)
		global.ui->OnSystemMsg( XGetStr( L"PARTY_RARELOOTINGRULE"), FSParam(pName, rulerare[nRare]));
	if (rule.m_nLRRF != oldrule.m_nLRRF)
		global.ui->OnSystemMsg( XGetStr( L"PARTY_RARELOOTINGRULEITEM"), FSParam(pName, rulerarefor[nRareFor]));

	gvar.MyInfo.Party.ChangeLootingRule(rule);
	global.script->GetGlueGameEvent().OnPartyRefresh();

	return CR_TRUE;
}

// 주변 파티원이 아닌 플레이어들에게 모두 알려준다.
MCommandResult XCmdHandler_Party::OnChangeLeaderNotify(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidOldLeader;
	MUID uidNewLeader;
	if (!pCommand->GetParameter(&uidOldLeader, 0, MPT_UID))	return CR_ERROR;
	if (!pCommand->GetParameter(&uidNewLeader, 1, MPT_UID))	return CR_ERROR;

	XNetPlayer* pOldLeader = gg.omgr->FindNetPlayer(uidOldLeader);
	if(pOldLeader)	pOldLeader->SetPartyLeader(false);

	XNetPlayer* pNewLeader = gg.omgr->FindNetPlayer(uidNewLeader);
	if(pNewLeader)	pNewLeader->SetPartyLeader(true);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Party::OnChangeState(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nQuestID, nAutoPartyState;
	if (!pCommand->GetParameter(&nQuestID, 0, MPT_INT))	return CR_ERROR;
	if (!pCommand->GetParameter(&nAutoPartyState, 1, MPT_INT))	return CR_ERROR;

	gvar.MyInfo.Party.ChangeAutoPartyState(nQuestID, nAutoPartyState);

	return CR_TRUE;
}

