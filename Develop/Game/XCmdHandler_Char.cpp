#include "StdAfx.h"
#include "XCmdHandler_Char.h"
#include "XMyPlayer.h"
#include "XGameState.h"
#include "XController.h"
#include "XBaseApplication.h"
#include "XModuleEffect.h"
#include "XNonPlayer.h"
#include "XInteractionInfo.h"
#include "XStrings.h"

XCmdHandler_Char::XCmdHandler_Char(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CHAR_MYINFO,				OnMyInfo);
	SetCmdHandler(MC_CHAR_MYTIMEINFO,			OnMyTimeInfo);
	SetCmdHandler(MC_CHAR_REBIRTH_MYPLAYER,		OnRebirthMyPlayer);
	SetCmdHandler(MC_CHAR_REBIRTH_NETPLAYER,	OnRebirthNetPlayer);
	SetCmdHandler(MC_CHAR_UPDATE_SIMPLE_STATUS,	OnUpdateSimpleStatus);
	SetCmdHandler(MC_CHAR_REWARD_EXP,			OnRewardExp);
//	SetCmdHandler(MC_SH_ITEM_XP,				OnRewardItemXP);
	SetCmdHandler(MC_CHAR_UPDATE_MONEY,			OnUpdateMoney);
	SetCmdHandler(MC_CHAR_UPDATE_TP,			OnUpdateTP);
	SetCmdHandler(MC_CHAR_LEVEL_UP,				OnLevelUp);
//	SetCmdHandler(MC_CHAR_SAVE_SOULBINDING,		OnSaveSoulBinding);
	SetCmdHandler(MC_CHAR_SAVE_CHECKPOINT,		OnSaveCheckPoint);
	SetCmdHandler(MC_CHAR_ENEMY_INFO,			OnEnemyInfo);
	SetCmdHandler(MC_CHAR_ENEMY_INFO_CLEAR,		OnEnemyInfoClear);
	SetCmdHandler(MC_CHAR_TIP,					OnTip);
	SetCmdHandler(MC_CHAR_ADD_ENEMYNPC,			OnAddEnemyNPC);
	SetCmdHandler(MC_CHAR_DEL_ENEMYNPC,			OnDelEnemyNPC);
	SetCmdHandler(MC_CHAR_TARGET_INFO,			OnTargetInfo);
}

MCommandResult XCmdHandler_Char::OnMyInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	TD_MYINFO tdMyInfo;
	vector<TD_ITEM>	vecTDInventory;
	vector<TD_ITEM> vecTDEquipment;
	vector<TD_ITEM_SLOT_INFO> vecTDReservedDummySlotInfo;
	vector<TD_TALENT> vecTalent;
	vector<TD_PLAYERQUEST> vecTDPlayerQuest;
	vector<TD_PALETTE> vecTDPalette;
	vector<TD_FACTION> vecTDFaction;
	vector<int> vecRecipeID;

	if (pCommand->GetSingleBlob(tdMyInfo, 0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDInventory, 1)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDEquipment, 2)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDReservedDummySlotInfo, 3)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTalent, 4)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDPlayerQuest, 5)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDPalette, 6)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDFaction, 7)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecRecipeID, 8)==false) return CR_ERROR;

	gvar.MyInfo.SetFrom_TD_MYINFO(tdMyInfo, 
		vecTDInventory, 
		vecTDEquipment, 
		vecTalent, 
		vecTDPlayerQuest, 
		vecTDPalette[0], 
		vecTDFaction, 
		vecRecipeID);

	if (global.ui)
	{
		global.ui->InventoryUIRefresh();
		//global.ui->TalentRefresh();
	}

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnMyTimeInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_BUFF_REMAIN_TIME>			vecTDBuffRemainTime;
	vector<TD_TALENT_REMAIN_COOLTIME>	vecTDTalentRemainCoolTime;
	float								fAutoRebirthRemainTime;

	if (pCommand->GetBlob(vecTDBuffRemainTime, 0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDTalentRemainCoolTime, 1)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fAutoRebirthRemainTime, 2, MPT_FLOAT)==false) return CR_ERROR;

	gvar.Game.pMyPlayer->SetMyTimeInfo(vecTDBuffRemainTime, vecTDTalentRemainCoolTime, fAutoRebirthRemainTime);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnRebirthMyPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_ERROR;

	int nFieldID;
	vec3 vPos, vDir;
	svec2 svDir;
	TD_UPDATE_SIMPLE_STATUS* pUpdateSimpleStatus = NULL;	

	if (pCommand->GetParameter(&nFieldID,		0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,			1, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,			2, MPT_SVEC2)==false) return CR_ERROR;

	vDir = svDir;

	MCommandParameter* pParam = pCommand->GetParameter(3);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
	pUpdateSimpleStatus = (TD_UPDATE_SIMPLE_STATUS*)pParam->GetPointer();

	if (NULL == pUpdateSimpleStatus) return CR_ERROR;
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;// gg.omgr->FindMyPlayer();
	if (NULL == pMyPlayer) return CR_ERROR;

	XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
	if(NULL == pModuleMyControl) return CR_ERROR;

	if (nFieldID == gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID)
	{
		if (gg.controller) 
		{
			pMyPlayer->SetPosition(vPos);
			pMyPlayer->SetDirection(vDir);

			gg.controller->SetEnabled(true);
			gg.controller->ClearKeyBuffer();
		}
	}
	else
	{
		gvar.World.SetNextMapInfo(nFieldID, vPos, vDir);
	}

	pModuleMyControl->OnRebirth();
	gvar.MyInfo.SetHP(pUpdateSimpleStatus->nHP);
	gvar.MyInfo.SetEN(pUpdateSimpleStatus->nEN);
	gvar.MyInfo.SetSTA(pUpdateSimpleStatus->nSTA);

	global.ui->OnSystemMsg( XGetStr( L"MSG_REBIRTH"));
	global.ui->InventoryUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnRebirthNetPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidRebirther;
	if (pCommand->GetParameter(&uidRebirther,		0, MPT_UID)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidRebirther);
	if (pActor == NULL) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnRebirth();
	}
	XModuleEffect* pModuleEffect = pActor->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_REBIRTH);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnUpdateSimpleStatus(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
	TD_UPDATE_SIMPLE_STATUS* pSimpleStatus = (TD_UPDATE_SIMPLE_STATUS*)pParam->GetPointer();

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if (pMyPlayer)
	{
		XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
		if(pModuleMyControl)
		{
			if (pModuleMyControl->IsReservedHitByTalent() == true)
			{
				return CR_ERROR;
			}

			// 잡기 상태라면... 셋팅하지 않는다. 잡기에서 알아서 빠진다. 끝나면 계속 진행
			if(pModuleMyControl->IsCurrentGrappled() == true)
				return CR_ERROR;
		}
	}



	gvar.MyInfo.SetHP(pSimpleStatus->nHP);
	gvar.MyInfo.SetEN(pSimpleStatus->nEN);
	gvar.MyInfo.SetSTA(pSimpleStatus->nSTA);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnRewardExp(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nExp;
	int nGainExp;
	float fExpPercent;

	if (pCommand->GetParameter(&nGainExp,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fExpPercent,2, MPT_FLOAT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nExp,		3, MPT_INT)==false) return CR_ERROR;


	if (nGainExp > 0)
	{
		if (FT_NORMAL > gvar.MyInfo.nFatigueType)
		{
			global.ui->OnSystemMsg(XGetStr(L"GAIN_EXP_WITH_FATIGUE"), FSParam(nGainExp));
		}
		else
		{
			global.ui->OnSystemMsg(XGetStr(L"GAIN_EXP"), FSParam(nGainExp));
		}
	}

	_ASSERT(fExpPercent < 100.f);
	if (fExpPercent >= 100.f)	return CR_FALSE;

	gvar.MyInfo.SetExpPercent(fExpPercent);
	gvar.MyInfo.ChrInfo.nXP = nExp;

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "EXPERIENCE");
	}

	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	if (pMyPlayer && pMyPlayer->GetModuleEffect())
	{
		XCaptionEffectEventData * pEventData = new XCaptionEffectEventData;
		pEventData->m_nParam1 = nGainExp;
		pMyPlayer->GetModuleEffect()->OnEffectEvent(XEFTEVT_EFFECT_CAPTION_EXP, pEventData);
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnRewardItemXP(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nExp;
	int nGainExp;
	int nNextAttuneXP;
	int nSlotID;

	if (pCommand->GetParameter(&nSlotID,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nGainExp,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nNextAttuneXP,2, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nExp,		3, MPT_INT)==false) return CR_ERROR;

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)nSlotID );
	if(!pItem) return CR_ERROR;
	if(pItem->m_nEXP == -1)
	{
		pItem->m_nEXP = nGainExp;
		pItem->m_nNextAttuneXP = nNextAttuneXP;
	}
	else
	{
		if(pItem->m_nEXP + nGainExp > nNextAttuneXP)
			pItem->m_nEXP = nNextAttuneXP;
		else
			pItem->m_nEXP = nExp;
		pItem->m_nNextAttuneXP = nNextAttuneXP;
	}
	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnUpdateMoney(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nSilver;
	if (pCommand->GetParameter(&nSilver,		1, MPT_INT)==false) return CR_ERROR;
	if (gvar.MyInfo.ChrInfo.GetMoney() == nSilver) return CR_TRUE;

	if (gvar.MyInfo.ChrInfo.GetMoney() < nSilver)
	{
		global.ui->OnSystemMsg(XGetStr(L"OBTAIN_SILVER"), FSParam(ConvertMoneyToString(nSilver - gvar.MyInfo.ChrInfo.GetMoney())));
	}
	else
	{
		global.ui->OnSystemMsg(XGetStr(L"SUBTRACT_SILVER"), FSParam(ConvertMoneyToString(gvar.MyInfo.ChrInfo.GetMoney() - nSilver)));
	}

	global.ui->SetSilver(nSilver);

	if(global.script)	global.script->GetGlueGameEvent().OnGameEvent( "SHOP", "REFRESH");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnUpdateTP(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nTP;
	if (pCommand->GetParameter(&nTP,		0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Talent.SetTP(nTP);

	if(global.script)	global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "TALENTPOINT", nTP);

	global.ui->TalentRefresh();

	return CR_TRUE;
}


MCommandResult XCmdHandler_Char::OnLevelUp(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidPlayer;
	uint8 nLevel;
	unsigned short max_hp, max_en, max_sta, remain_tp;

	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nLevel,		1, MPT_UCHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&max_hp,		2, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&max_en,		3, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&max_sta,	4, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&remain_tp,	5, MPT_USHORT)==false) return CR_ERROR;
	
	if (uidPlayer == XGetMyUID())
	{
		if (pCommand->GetParameter(&gvar.MyInfo.ChrInfo.nSTR,	8, MPT_USHORT)==false) return CR_ERROR;
		if (pCommand->GetParameter(&gvar.MyInfo.ChrInfo.nDEX,	9, MPT_USHORT)==false) return CR_ERROR;
		if (pCommand->GetParameter(&gvar.MyInfo.ChrInfo.nINT,	10, MPT_USHORT)==false) return CR_ERROR;
		if (pCommand->GetParameter(&gvar.MyInfo.ChrInfo.nCHA,	11, MPT_USHORT)==false) return CR_ERROR;
		if (pCommand->GetParameter(&gvar.MyInfo.ChrInfo.nCON,	12, MPT_USHORT)==false) return CR_ERROR;

		gvar.MyInfo.nLevel = nLevel;
		gvar.MyInfo.ChrInfo.nMaxHP = max_hp;
		gvar.MyInfo.SetHP(gvar.MyInfo.GetMaxHP());
		gvar.MyInfo.ChrInfo.nMaxEN = max_en;
		gvar.MyInfo.SetEN(gvar.MyInfo.GetMaxEN());
		gvar.MyInfo.ChrInfo.nMaxSTA = max_sta;
		gvar.MyInfo.SetSTA(gvar.MyInfo.GetMaxSTA());
		gvar.MyInfo.Talent.SetTP(remain_tp);

		global.ui->OnSystemMsg( XGetStr( L"MSG_LEVELUP"), FSParam(nLevel));
		global.ui->OnSystemMsg( XGetStr( L"TALENT_LEARNABLENEW"), FSParam( L"T"));
		global.ui->OnGuideMsg(GUIDE_MSG_LEVEL_UP);

		if(global.script)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "LEVEL", nLevel);
			global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "STATUS");
			global.script->GetGlueGameEvent().OnGameEvent( "SHOP", "REFRESH");
		}
	}

	XObject* pObject = gg.omgr->Find(uidPlayer);
	if (NULL == pObject) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pObject->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->OnEffectLevelUP();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnSaveSoulBinding(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nSoulBindingFieldID;
	if (pCommand->GetParameter(&nSoulBindingFieldID,	0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.nSoulbindingFieldID = nSoulBindingFieldID;

	global.ui->OnSystemMsg( XGetStr( L"MSG_SAVEDSOULPOSITION"));
	global.ui->OnSoulBindingEffect();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnSaveCheckPoint(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nCheckPointFieldID;
	if (pCommand->GetParameter(&nCheckPointFieldID,	0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.nCheckPointFieldID = nCheckPointFieldID;

	global.ui->OnSystemMsg( XGetStr( L"MSG_SAVEDCHECKPOINT"));

	return CR_TRUE;
}


MCommandResult XCmdHandler_Char::OnEnemyInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	gvar.Game.EnemyInfo.Clear();
	vector<TD_ENEMY_INFO>& enemylist = gvar.Game.EnemyInfo.GetEnemyList();
	if (pCommand->GetBlob(enemylist, 0)==false) return CR_ERROR;

	gvar.Game.EnemyInfo.NPCGradeSort();

	gvar.Game.EnemyInfo.CheckEnemyInfoList();

	gvar.Game.EnemyInfo.CheckDamageInfo();

	XGetInteractionInfo().stTargetActorInfo.AddEnemyList();

	if(global.script)
		global.script->GetGlueGameEvent().OnEnemyInfoRefresh();

	if( NULL != gvar.Game.EnemyInfo.GetEnemyInfoByUIID(XGetInteractionInfo().GetSearchUIID()))
	{
		if(global.script)	global.script->GetGlueGameEvent().OnTargetInfoRefresh();
	}

	return CR_TRUE;
}

//적이 죽거나 어그로가 풀리거나 전투가 종료될때
MCommandResult XCmdHandler_Char::OnEnemyInfoClear(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	gvar.Game.EnemyInfo.Clear();

	if(global.script)	global.script->GetGlueGameEvent().OnEnemyInfoRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnTip(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring	strKeyWithParam;
	if (pCommand->GetParameter(strKeyWithParam, 0, MPT_WSTR)==false)	return CR_ERROR;
			
	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strKeyWithParam);
	global.ui->OnPresentationUpper(strMessage);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnAddEnemyNPC(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID nNPCUID;
	if (pCommand->GetParameter(&nNPCUID,			0, MPT_UID)==false) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC(nNPCUID);
	if (pNPC)
	{
		pNPC->SetCombating(true);
		XNPCInfo* pNPCInfo = pNPC->GetInfo();
		if ( pNPCInfo != NULL)
		{
			// 전투 사운드
			if(pNPCInfo->SoundInfo.CombatBGM.empty() == false)
			{
				if ( pNPCInfo->SoundInfo.CombatBGM.empty() == false)
				{
					if ( global.sound)
						global.sound->Play( MLocale::ConvUTF16ToAnsi(pNPCInfo->SoundInfo.CombatBGM.c_str()).c_str(), "bgm_battle");
				}
			}

			// 어그로 획득 사운드
			if ( pNPC != NULL)
			{
				XNPCInfo* pNPCInfo = pNPC->GetInfo();
				if ( pNPCInfo->SoundInfo.Aggro.empty() != NULL)
				{
					if ( global.sound)
						global.sound->Play( MLocale::ConvUTF16ToAnsi(pNPCInfo->SoundInfo.Aggro.c_str()).c_str());
				}
			}
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Char::OnDelEnemyNPC(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID nNPCUID;
	if (pCommand->GetParameter(&nNPCUID,			0, MPT_UID)==false) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC(nNPCUID);
	if (pNPC)
	{
		pNPC->SetCombating(false);
		XNPCInfo* pNPCInfo = pNPC->GetInfo();
		if (pNPCInfo)
		{
			if(pNPCInfo->SoundInfo.CombatBGM.empty() == false)
			{
				if ( global.sound)
					global.sound->Stop( "bgm_battle");
			}
		}
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_Char::OnTargetInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_TARGET_INFO> vecTargetInfo;
	if (pCommand->GetBlob(vecTargetInfo,	0)==false) return CR_ERROR;

	_ASSERT(vecTargetInfo.size() == 1);
	if (vecTargetInfo.size() != 1) return CR_TRUE;

	XGetInteractionInfo().stTargetActorInfo.Add(vecTargetInfo[0].m_nUIID, vecTargetInfo[0]);

	if(  XGetInteractionInfo().GetSearchUIID() == vecTargetInfo[0].m_nUIID)
	{
		if(global.script)	global.script->GetGlueGameEvent().OnTargetInfoRefresh();
	}

	return CR_TRUE;
}
