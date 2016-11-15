#include "StdAfx.h"
#include "XCmdHandler_Guild.h"
#include "XGuild.h"
#include "XGuildMember.h"
#include "XMyPlayer.h"
#include "XPost_Guild.h"
#include "XInteractionInfo.h"
#include "XItemManager.h"
#include "XItemHelper.h"
#include "XStrings.h"

XCmdHandler_Guild::XCmdHandler_Guild(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_GUILD_SHOW_CREATE_UI,	OnShowCreateUI);
	SetCmdHandler(MC_GUILD_SHOW_DESTROY_UI,	OnShowDestroyUI);
	SetCmdHandler(MC_GUILD_CREATE,			OnCreate);
	SetCmdHandler(MC_GUILD_INVITE_NOTIFY,	OnInvite_Notify);
	SetCmdHandler(MC_GUILD_INVITE_REJECT,	OnInvite_Reject);
	SetCmdHandler(MC_GUILD_JOIN_ME,			OnJoinMe);
	SetCmdHandler(MC_GUILD_JOIN_OTHER,		OnJoinOther);
	SetCmdHandler(MC_GUILD_JOIN_SECTOR,		OnGuildJoinSector);
	SetCmdHandler(MC_GUILD_LEAVE,			OnLeave);
	SetCmdHandler(MC_GUILD_LEAVE_SECTOR,	OnGuildLeaveSector);
	SetCmdHandler(MC_GUILD_KICK,			OnKick);
	SetCmdHandler(MC_GUILD_DESTROY,			OnDestroy);
	SetCmdHandler(MC_GUILD_ONLINE,			OnOnLine);
	SetCmdHandler(MC_GUILD_OFFLINE,			OnOffLine);
	SetCmdHandler(MC_GUILD_MOVEFIELD,		OnMoveField);
	SetCmdHandler(MC_GUILD_CHANGE_MASTER,	OnChangeMaster);
	SetCmdHandler(MC_GUILD_INFO,			OnGuildInfo);

	SetCmdHandler(MC_GUILD_SHOW_STORAGE_UI,			OnShowStorageUI);
	SetCmdHandler(MC_GUILD_DEPOSIT_STORAGEMONEY,	OnDepositStorageMoney);
	SetCmdHandler(MC_GUILD_WITHDRAW_STORAGEMONEY,	OnWithdrawStorageMoney);
	SetCmdHandler(MC_GUILD_MOVE_STORAGEITEM,		OnMoveStorageItem);
}

MCommandResult XCmdHandler_Guild::OnShowCreateUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.script->GetGlueGameEvent().OnGuildCreate();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnShowDestroyUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.script->GetGlueGameEvent().OnGuildDestroy();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnCreate(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nUIID;
	wstring strGuildName;
	if (pCommand->GetParameter(&nUIID,			0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strGuildName,	1, MPT_WSTR)==false) return CR_ERROR;

	if (nUIID == XGetMyUIID())
	{
		gvar.MyInfo.Guild.m_strName = strGuildName;
		gvar.MyInfo.Guild.m_strMasterName = gvar.MyInfo.ChrInfo.szName;
		gvar.MyInfo.Guild.InsertMember(gvar.MyInfo.ChrInfo.szName, gvar.MyInfo.ChrInfo.nLevel, GMG_MASTER, true, gvar.World.GetFieldID(), gvar.World.nChannelID);	
		XGuildMember* pGuildMember = gvar.MyInfo.Guild.GetMember(gvar.MyInfo.ChrInfo.szName);
		if (NULL == pGuildMember) return CR_FALSE;
		pGuildMember->m_nGrade = GMG_MASTER;

		global.ui->OnPresentationLower( XGetStr( L"GUILD_CREATED"), PRESENTATION_ICON_EXCLAMATION, true);

		XGetMyPlayer()->SetCharacterPane_Player();

		global.script->GetGlueGameEvent().OnGuildRefresh();

		gvar.Game.pMyPlayer->GetModuleMyControl()->OnEffectCreateGuild();
	}
	else
	{
		XPlayer* pPlayer = gg.omgr->FindPlayer_UIID(nUIID);
		if(pPlayer)
		{
			XNetPlayer* pNetPlayer = pPlayer->AsNetPlayer();
			pNetPlayer->GetModuleActorControl()->OnEffectCreateGuild();
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnInvite_Notify(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidInvitor;
	wstring strGuildName;
	if (pCommand->GetParameter(&uidInvitor,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(strGuildName,	1, MPT_WSTR)==false) return CR_ERROR;


	if ( XGETCFG_GAME_REJECT_GUILD)
	{
		global.ui->OnSystemMsg( XGetStr( L"GUILD_CANCELINVITED"));
		XPostGuild_Invite_Res( false);
	}
	else
	{
		XGetInteractionInfo().SetRequestPlayerUID(uidInvitor);

		global.ui->OnSystemMsg( XGetStr( L"GUILD_INVITED"));
		global.ui->OnGuideMsg(GUIDE_MSG_LUA_RECV_INVITE_GUILD);

		gvar.Game.pMyPlayer->SafeStop();

		if (global.script)
			global.script->GetGlueGameEvent().OnGameEvent( "GUILD", "INVITED");
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnInvite_Reject(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidRejector;
	if (pCommand->GetParameter(&uidRejector,	0, MPT_UID)==false) return CR_ERROR;

	XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer(uidRejector);
	if(pNetPlayer)
	{
		wstring strName;
		strName = pNetPlayer->GetName();
		global.ui->OnSystemMsg( XGetStr( L"GUILD_INVITED_REJECT"), FSParam(strName));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnJoinMe(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_GUILD> vecTDGuild;
	vector<TD_GUILD_MEMBER> vecTDGuildMember;
	if (pCommand->GetBlob(vecTDGuild,		0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDGuildMember,	1)==false) return CR_ERROR;

	if (1 != vecTDGuild.size()) return CR_ERROR;

	gvar.MyInfo.Guild.m_strName = vecTDGuild[0].szName;
	gvar.MyInfo.Guild.m_strMasterName = vecTDGuild[0].szMasterName;
	for each (TD_GUILD_MEMBER tdGuildMember in vecTDGuildMember)
	{
		gvar.MyInfo.Guild.InsertMember(tdGuildMember.szName, tdGuildMember.nLevel, tdGuildMember.nGrade, tdGuildMember.bOnline, gvar.World.GetFieldID(), gvar.World.nChannelID);
	}

	global.ui->OnSystemMsg( XGetStr( L"GUILD_ENTERED"));
	XGetMyPlayer()->SetCharacterPane_Player();

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnJoinOther(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	int nLevel;
	int nFieldID;
	int nChannelID;
	if (pCommand->GetParameter(strName,			0, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nLevel,			1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFieldID,		2, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nChannelID,		3, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Guild.InsertMember(strName.c_str(), nLevel, GMG_NORMAL, true, nFieldID, nChannelID);

	global.ui->OnSystemMsg( XGetStr( L"GUILD_ENTEREDMEMBER"), FSParam(strName));
	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnLeave(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	if (pCommand->GetParameter(strName,		0, MPT_WSTR)==false) return CR_ERROR;

	if (strName == XGetMyName())
	{
		gvar.MyInfo.Guild.Clear();
		global.ui->OnSystemMsg( XGetStr( L"GUILD_LEFT"));

		XGetMyPlayer()->SetCharacterPane_Player();
	}
	else
	{
		gvar.MyInfo.Guild.DeleteMember(strName.c_str());
		global.ui->OnSystemMsg( XGetStr( L"GUILD_LEFTMEMBER"), FSParam(strName));

		XPlayer* pPlayer = gg.omgr->FindPlayer(strName.c_str());
		if(pPlayer)
		{
			XNetPlayer* pNetPlayer = pPlayer->AsNetPlayer();
			pNetPlayer->ChangeGuildName(L"");
			pNetPlayer->SetCharacterPane_Player();
		}
	}

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnKick(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	if (pCommand->GetParameter(strName,		0, MPT_WSTR)==false) return CR_ERROR;

	if (strName == gvar.MyInfo.ChrInfo.szName)
	{
		global.ui->OnSystemMsg( XGetStr( L"GUILD_BANISHED"), FSParam(gvar.MyInfo.Guild.m_strName));
		gvar.MyInfo.Guild.Clear();		

		XGetMyPlayer()->SetCharacterPane_Player();
	}
	else
	{
		gvar.MyInfo.Guild.DeleteMember(strName.c_str());
		global.ui->OnSystemMsg( XGetStr( L"GUILD_BANISHEDMEMBER"), FSParam(strName));

		XPlayer* pPlayer = gg.omgr->FindPlayer(strName.c_str());
		if(pPlayer)
		{
			XNetPlayer* pNetPlayer = pPlayer->AsNetPlayer();
			pNetPlayer->ChangeGuildName(L"");
			pNetPlayer->SetCharacterPane_Player();
		}
	}

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnDestroy(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.MyInfo.Guild.Clear();

	global.ui->OnPresentationLower( XGetStr( L"GUILD_DISBANDED"), PRESENTATION_ICON_EXCLAMATION, true);

	XGetMyPlayer()->SetCharacterPane_Player();

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnOnLine(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	int nFieldID;
	int nChannelID;
	if (pCommand->GetParameter(strName,		0, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFieldID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nChannelID,	2, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Guild.OnLine(strName.c_str(), nFieldID, nChannelID);

	if (0 != wcscmp(strName.c_str(), gvar.MyInfo.ChrInfo.szName))
	{
		wstring strMsg = CSFormatString::Format( XGetStr( L"GUILD_ONLINEMEMBER"), strName.c_str());
		global.ui->OnSystemMsg(strMsg.c_str());

		global.script->GetGlueGameEvent().OnMessageBox(MLocale::ConvUTF16ToAnsi(strMsg));

	}	

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnOffLine(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	if (pCommand->GetParameter(strName,		0, MPT_WSTR)==false) return CR_ERROR;

	gvar.MyInfo.Guild.OffLine(strName.c_str());

	if (0 != wcscmp(strName.c_str(), gvar.MyInfo.ChrInfo.szName))
	{
		global.ui->OnSystemMsg( XGetStr( L"GUILD_OFFLINEMEMBER"), FSParam(strName));
	}

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnMoveField(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	int nFieldID;
	int nChannelID;
	if (pCommand->GetParameter(strName,		0, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nFieldID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nChannelID,	2, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Guild.MoveField(strName.c_str(), nFieldID, nChannelID);
	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnChangeMaster(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strOldMasterName;
	wstring strNewMasterName;
	if (pCommand->GetParameter(strOldMasterName,		0, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(strNewMasterName,		1, MPT_WSTR)==false) return CR_ERROR;

	gvar.MyInfo.Guild.ChangeGuildMaster(strOldMasterName.c_str(), strNewMasterName.c_str());

	global.ui->OnSystemMsg( XGetStr( L"GUILD_CHANGEDGUILDMASTER"), FSParam(strOldMasterName, strNewMasterName));

	global.script->GetGlueGameEvent().OnGuildRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnGuildInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_GUILD> vecTDGuild;
	vector<TD_GUILD_MEMBER> vecTDGuildMember;

	if (pCommand->GetBlob(vecTDGuild, 0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDGuildMember, 1)==false) return CR_ERROR;

	gvar.MyInfo.Guild.Clear();
	if (false == vecTDGuild.empty())
	{
		gvar.MyInfo.Guild.m_strMasterName = vecTDGuild[0].szMasterName;
		gvar.MyInfo.Guild.m_strName = vecTDGuild[0].szName;

		for each (TD_GUILD_MEMBER tdGuildMember in vecTDGuildMember)
		{
			gvar.MyInfo.Guild.InsertMember(tdGuildMember.szName, tdGuildMember.nLevel, tdGuildMember.nGrade, tdGuildMember.bOnline, tdGuildMember.nFieldID, tdGuildMember.nChannelID);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnGuildJoinSector(MCommand* pCommand, MCommandHandler* pHandler)
{								  
	UIID nUIID;
	wstring strGuildName;
	if (pCommand->GetParameter(&nUIID,			0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strGuildName,	1, MPT_WSTR)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer_UIID(nUIID);
	if(pPlayer)
	{
		XNetPlayer* pNetPlayer = pPlayer->AsNetPlayer();
		pNetPlayer->ChangeGuildName(strGuildName);
		pNetPlayer->SetCharacterPane_Player();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnGuildLeaveSector(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nUIID;
	if (pCommand->GetParameter(&nUIID,		0, MPT_USHORT)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer_UIID(nUIID);
	if(pPlayer)
	{
		XNetPlayer* pNetPlayer = pPlayer->AsNetPlayer();
		pNetPlayer->ChangeGuildName(L"");
		pNetPlayer->SetCharacterPane_Player();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnShowStorageUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nStorageMoney;
	vector<TD_ITEM>	vecTDItem;
	if (pCommand->GetParameter(&nStorageMoney, 0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDItem, 1)==false) return CR_ERROR;

	gvar.MyInfo.Guild.SetStorageMoney(nStorageMoney);

	for each (const TD_ITEM& tdItem in vecTDItem)
	{
		XItem* pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);
		if (pNewItem == NULL) 
		{
			mlog3("No Item ID = %d\n", tdItem.m_nItemID);
			_ASSERT(0);
			continue;
		}

		pNewItem->ApplyTDItem(tdItem);
		gvar.MyInfo.GuildStorage.PushItem(pNewItem->m_nSlotID, pNewItem);
	}	

	global.script->GetGlueGameEvent().OnGuildStorageShow();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnDepositStorageMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	int nDepositMoney;
	int nStorageMoney;

	if (pCommand->GetParameter(strName,				0, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nDepositMoney,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nStorageMoney,		2, MPT_INT)==false) return CR_ERROR;

	if (0 == wcscmp(strName.c_str(), XGetMyName()))
	{
		gvar.MyInfo.ChrInfo.SetMoney(gvar.MyInfo.ChrInfo.GetMoney()-nDepositMoney);
	}	

	gvar.MyInfo.Guild.SetStorageMoney(nStorageMoney);

	global.script->GetGlueGameEvent().OnGuildStorageRefresh();
	global.ui->InventoryMoneyRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnWithdrawStorageMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	int nWithdrawMoney;
	int nStorageMoney;

	if (pCommand->GetParameter(strName,				0, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nWithdrawMoney,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nStorageMoney,		2, MPT_INT)==false) return CR_ERROR;

	if (0 == wcscmp(strName.c_str(), XGetMyName()))
	{
		gvar.MyInfo.ChrInfo.SetMoney(gvar.MyInfo.ChrInfo.GetMoney()+nWithdrawMoney);
	}	

	gvar.MyInfo.Guild.SetStorageMoney(nStorageMoney);

	global.script->GetGlueGameEvent().OnGuildStorageRefresh();
	global.ui->InventoryMoneyRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Guild::OnMoveStorageItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strName;
	vector<TD_ITEM> vecTDItem;
	if (!pCommand->GetParameter(strName,		0, MPT_WSTR)) return CR_FALSE;
	if (!pCommand->GetBlob(vecTDItem,			1)) return CR_FALSE;

	int nIndex = 0;
	for each (const TD_ITEM& tdItem in vecTDItem)
	{
		XItem* pOldItem = XItemHelper::PopItem(SLOTTYPE_GUILDSTORAGE, tdItem.m_nSlotID);
		XItem* pNewItem = NULL;

		if (0 != tdItem.m_nItemID)
		{
			pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);		
			pNewItem->ApplyTDItem(tdItem);
			XItemHelper::PushItem(SLOTTYPE_GUILDSTORAGE, tdItem.m_nSlotID, pNewItem);
		}

		// 길드보관함에서 아이템이동
		if (vecTDItem.size() >= 2)
		{
			if (nIndex > 0) continue;

			// 빈슬롯으로 이동
			if (pNewItem == NULL)
			{
				global.ui->OnSystemMsg(L"{0}님이 {1} {2}개를 이동했습니다.",
					FSParam(strName,pOldItem->m_pItemData->GetName(),pOldItem->m_nAmount));
			}
			//아이템 스왑
			else
			{
				global.ui->OnSystemMsg(L"{0}님이 {1} {2}개를 {3} {4}개와 이동했습니다.",
					FSParam(strName,pOldItem->m_pItemData->GetName(),pOldItem->m_nAmount
					,pNewItem->m_pItemData->GetName(),pNewItem->m_nAmount));
			}

			nIndex++;
			continue;
		}
		// 길드보관함에서 인벤토리로
		if (NULL == pNewItem && NULL != pOldItem)
		{
			global.ui->OnSystemMsg(L"{0}님이 {1} {2}개를 가져갔습니다.",
				FSParam(strName,pOldItem->m_pItemData->GetName(),pOldItem->m_nAmount));
		}
		// 인벤토리에서 길드보관함으로
		else if(NULL != pNewItem && NULL == pOldItem)
		{
			global.ui->OnSystemMsg(L"{0}님이 {1} {2}개를 넣었습니다.",
				FSParam(strName,pNewItem->m_pItemData->GetName(),pNewItem->m_nAmount));
		}
		// 인벤토리과 길드보관함 스왑
		else if(NULL != pNewItem && NULL != pOldItem)
		{
			global.ui->OnSystemMsg(L"{0}님이 {1} {2}개를 가져갔습니다.",
				FSParam(strName,pOldItem->m_pItemData->GetName(),pOldItem->m_nAmount));
			global.ui->OnSystemMsg(L"{0}님이 {1} {2}개를 넣었습니다.",
				FSParam(strName,pNewItem->m_pItemData->GetName(),pNewItem->m_nAmount));
		}
	}

	global.script->GetGlueGameEvent().OnGuildStorageRefresh();

	return CR_TRUE;
}