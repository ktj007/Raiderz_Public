#include "StdAfx.h"
#include "XCmdHandler_Loot.h"
#include "XModuleActorControl.h"
#include "XMyPlayer.h"
#include "XModuleMyControl.h"
#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XEventID.h"
#include "XItemManager.h"
#include "XModuleBParts.h"

XCmdHandler_Loot::XCmdHandler_Loot(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_LOOT_BEGIN,				OnBegin);
	SetCmdHandler(MC_LOOT_SEE_ITEM,				OnSeeItem);
	SetCmdHandler(MC_LOOT_MASTERLOOT_UI,		OnMasterLootUI);
	SetCmdHandler(MC_LOOT_MASTERLOOT_NOTIFY,	OnMasterLootNotify);
	SetCmdHandler(MC_LOOT_ROLLRESULT,			OnRollResult);
	SetCmdHandler(MC_LOOT_REFRESH_ITEM,			OnRefreshItem);
	SetCmdHandler(MC_LOOT_OTHER_GAIN_ITEM,		OnOtherGainItem);
	SetCmdHandler(MC_LOOT_GETTABLE_ITEM_ADD,	OnGettableItemAdd);
	SetCmdHandler(MC_LOOT_END,					OnEnd);
	
	SetCmdHandler(MC_LOOT_INSERT_LOOTABLE_NPC,	OnInsertLootableNPC);
	SetCmdHandler(MC_LOOT_REMOVE_LOOTABLE_NPC,	OnRemoveLootableNPC);	
}

MCommandResult XCmdHandler_Loot::OnBegin(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidLooter;
	MUID uidNPC;
	float fLootTime;

	if (pCommand->GetParameter(&uidLooter,	0, MPT_UID)==false)		return CR_ERROR;
	if (pCommand->GetParameter(&uidNPC,		1, MPT_UID)==false)		return CR_ERROR;
	if (pCommand->GetParameter(&fLootTime,	2, MPT_FLOAT)==false)	return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidLooter);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->DoActionLootingItem(uidNPC, fLootTime);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Loot::OnSeeItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_ITEM_DROP> vecTDDropItem;

	if (pCommand->GetBlob(vecTDDropItem,	0)==false)	return CR_ERROR;

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if(pModuleMyControl == NULL) return CR_ERROR;

	gvar.MyInfo.LootingItem.Clear();

	for each( TD_ITEM_DROP tdDropItem in vecTDDropItem)
	{
		gvar.MyInfo.LootingItem.InsertItem( tdDropItem);
	}

	pModuleMyControl->OnLootItemStart();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Loot::OnMasterLootUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	int									nGameServerID;
	MUID								nFieldUID;
	MUID								nNPCUID;
	vector<TD_LOOT_MASTERLOOT_UI>		vecTDLootMasterLootUI;

	if (pCommand->GetParameter(&nGameServerID,			0, MPT_INT)==false)		return CR_ERROR;
	if (pCommand->GetParameter(&nFieldUID,				1, MPT_UID)==false)		return CR_ERROR;
	if (pCommand->GetParameter(&nNPCUID,				2, MPT_UID)==false)		return CR_ERROR;
	if (pCommand->GetBlob(vecTDLootMasterLootUI,		3)==false)	return CR_ERROR;


	if ( vecTDLootMasterLootUI.size() == 0)		return CR_TRUE;


//	char szBuff[64];	
	for each (TD_LOOT_MASTERLOOT_UI tdLootAssign in vecTDLootMasterLootUI)
	{
		XItemData* pItemData = info.item->GetItemData(tdLootAssign.m_nItemID);
		if (pItemData == NULL) continue;

		gvar.MyInfo.AssignLootingItem.InsertItem( nNPCUID, tdLootAssign);
	}

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ASSIGNLOOTING", "REFRESH", (int)gvar.MyInfo.AssignLootingItem.GetSize());

	return CR_TRUE;	
}

MCommandResult XCmdHandler_Loot::OnMasterLootNotify(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_LOOT_MASTERLOOT_NOTIFY> vecTDLootMasterLootNotify;

	if (pCommand->GetBlob(vecTDLootMasterLootNotify,	0)==false)	return CR_ERROR;

	for each (TD_LOOT_MASTERLOOT_NOTIFY tdLootAssignNotify in vecTDLootMasterLootNotify)
	{
		XItemData* pItemData = info.item->GetItemData(tdLootAssignNotify.m_nItemID);
		if (pItemData == NULL) continue;

		wstring strMsg = CSFormatString::Format( XGetStr( L"LOOTING_LOOTERPROGRESS"), FSParam(pItemData->GetName()));		
		global.script->GetGlueGameEvent().OnMessageBox(MLocale::ConvUTF16ToAnsi(strMsg));
	}

	return CR_TRUE;	
}

MCommandResult XCmdHandler_Loot::OnRollResult(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;
	vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;

	if (pCommand->GetBlob(vecTDRollItem,		0)==false)	return CR_ERROR;
	if (pCommand->GetBlob(vecTDRollResult,		1)==false)	return CR_ERROR;

	int nTDRollItemSize = vecTDRollItem.size();
	for (int i=0; i<nTDRollItemSize; i++)
	{
		const TD_LOOT_ROLL_ITEM& tdRollItem = vecTDRollItem[i];

		XItemData* pItemData = info.item->GetItemData(tdRollItem.m_nItemID);
		if (pItemData == NULL) return CR_TRUE;

		global.ui->OnSystemMsg( XGetStr( L"LOOTING_RESULTROLLDICE"), FSParam(pItemData->GetName(), tdRollItem.m_nItemAmount));

		for (vector<TD_LOOT_ROLL_RESULT>::iterator itor=vecTDRollResult.begin(); itor!=vecTDRollResult.end(); itor++)
		{
			TD_LOOT_ROLL_RESULT tdRollResult = (*itor);

			if (tdRollItem.m_nIndex == tdRollResult.m_nIndex)
			{
				wstring strName;
				XNetPlayer* pPlayer = gg.omgr->FindNetPlayer(tdRollResult.m_nPlayerUID);
				if (NULL != pPlayer)
				{
					strName = pPlayer->GetName();
				}
				else
				{
					strName = gvar.MyInfo.Party.GetPartyMemberName(tdRollResult.m_nPlayerUID);
				}

				if (!strName.empty())
					global.ui->OnSystemMsg( XGetStr( L"LOOTING_RESULTPOINTROLLDICE"), FSParam(strName, tdRollResult.m_nRollResult));
			}
			else if(tdRollItem.m_nIndex < tdRollResult.m_nIndex)
			{
				break;
			}
		}
	}


	return CR_TRUE;	
}

//루팅창 아이템리스트
MCommandResult XCmdHandler_Loot::OnRefreshItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_ITEM_DROP> vecTDDropItem;

	if (pCommand->GetBlob(vecTDDropItem,	0)==false)	return CR_ERROR;

	gvar.MyInfo.LootingItem.Clear();
	for each(TD_ITEM_DROP tdDropItem in vecTDDropItem)
	{
		gvar.MyInfo.LootingItem.InsertItem( tdDropItem);
	}

	//루팅창 정보리프레쉬
	if  (global.script)		global.script->GetGlueGameEvent().OnGameEvent( "LOOTING", "REFRESH");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Loot::OnOtherGainItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID nGainnerUID;
	vector<TD_LOOT_OTHERGAIN_ITEM> vecTDOtherGainItem;

	if (pCommand->GetParameter(&nGainnerUID,	0, MPT_UID)==false)	return CR_ERROR;
	if (pCommand->GetBlob(vecTDOtherGainItem,	1)==false)	return CR_ERROR;

	if (false == gvar.MyInfo.Party.Find(nGainnerUID)) return CR_ERROR;

	const wchar_t* szGainnerName = gvar.MyInfo.Party.GetPartyMemberName(nGainnerUID);

	for each (TD_LOOT_OTHERGAIN_ITEM tdOtherGainItem in vecTDOtherGainItem)
	{			
		XItemData* pItemData = info.item->GetItemData(tdOtherGainItem.m_nItemID);
		if (NULL == pItemData) continue;

		global.ui->OnSystemMsg( XGetStr( L"LOOTING_GAINITEM"), FSParam(szGainnerName, pItemData->GetName(), tdOtherGainItem.m_nAmount));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Loot::OnGettableItemAdd(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_LOOT_GETTABLE_ITEM_ADD> vecTDGettableItemAdd;

	if (pCommand->GetBlob(vecTDGettableItemAdd,	0)==false)	return CR_ERROR;

	for each (TD_LOOT_GETTABLE_ITEM_ADD tdGettableItemAdd in vecTDGettableItemAdd)
	{			
		XItemData* pItemData = info.item->GetItemData(tdGettableItemAdd.m_nItemID);
		if (NULL == pItemData) continue;
		
		global.ui->OnSystemMsg( XGetStr( L"LOOTING_NOTGAINITEM"), FSParam(pItemData->GetName(), tdGettableItemAdd.m_nItemAmount));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Loot::OnEnd(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,			0, MPT_UID)==false)	return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidPlayer);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnLootEnd();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Loot::OnInsertLootableNPC(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidNPC;
	if (pCommand->GetParameter(&uidNPC,	0, MPT_UID)==false)	return CR_ERROR;

	XActor* pNPC = gg.omgr->FindActor(uidNPC);
	if (pNPC)
	{
		pNPC->GetModuleActorControl()->OnSetLootable(true);
	}
	else
	{
		// 혹시 BPartNPC입니까?

		if(gg.omgr)
		{
			XObjectMap * pObjectMap = gg.omgr->GetObjectMap(XOBJ_NPC);
			if(pObjectMap)
			{
				for(XObjectMap::iterator it = pObjectMap->begin(); it != pObjectMap->end(); ++it)
				{
					XModuleBParts* pModuleBPart = it->second->GetModuleBParts();
					if(pModuleBPart)
					{
						pModuleBPart->SetLootableBPartNPC(uidNPC, true);
					}
				}
			}
		}
	}
	
	return CR_TRUE;	
}

MCommandResult XCmdHandler_Loot::OnRemoveLootableNPC(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidNPC;
	if (pCommand->GetParameter(&uidNPC,	0, MPT_UID)==false)	return CR_ERROR;

	XActor* pNPC = gg.omgr->FindActor(uidNPC);
	if (pNPC)
	{
		pNPC->GetModuleActorControl()->OnSetLootable(false);
	}

	return CR_TRUE;
}