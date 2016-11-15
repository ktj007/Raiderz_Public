#include "stdafx.h"
#include "XGlueGameQuestFunction.h"
#include "XFieldInfo.h"
#include "XPost_Quest.h"
#include "XItemManager.h"
#include "XStrings.h"
#include "XMyPlayer.h"
#include "XInteractionInfo.h"

int XGlueGameQuestFunction::GetQuestLevel(int nQuestID)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	return pQuestInfo->nLevel;
}

int XGlueGameQuestFunction::GetQuestLimitPersonnel( int nQuestID )
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	return pQuestInfo->nLimitPersonnel;
}

//const char* XGlueGameQuestFunction::GetQuestName( int nQuestID )
//{
//
//	if (nQuestID == 0) return "";
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return "";
//
//	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pQuestInfo->GetTitle());
//
//	return gvar.Temp.szTemp.c_str();
//}

//const char* XGlueGameQuestFunction::GetQuestDesc( int nQuestID )
//{
//
//	if (nQuestID == 0) return "";
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return "";
//	
//	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pQuestInfo->strDesc.c_str());
//	
//	return gvar.Temp.szTemp.c_str();
//}

//bool XGlueGameQuestFunction::IsQuestManualShare(int nQuestID)
//{
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return false;
//
//	return QSHAT_MANUAL == pQuestInfo->nShareType;
//}

//int XGlueGameQuestFunction::GetCountQuestObjectives( int nQuestID )
//{
//	if (nQuestID == 0) return 0;
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return 0;
//
//	return pQuestInfo->GetQObejctiveInfoCount();
//}

//int XGlueGameQuestFunction::GetQuestObjectiveAchive(int nQuestID, unsigned int nCount)
//{
//	if (nQuestID == 0) return 0;
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return 0;
//
//	if (nCount <= 0 || nCount > static_cast<unsigned int>(pQuestInfo->GetQObejctiveInfoCount()))
//		return 0;
//
//	XQObjectiveInfo* pQObjectiveInfo = pQuestInfo->Get(nCount);
//	if (pQObjectiveInfo == NULL)
//	{
//		mlog("뭔가가 이상해요!!");
//		return 0;
//	}
//
//	return pQObjectiveInfo->nParam2;
//}

int XGlueGameQuestFunction::GetQuestObjectiveID(int nQuestID, unsigned int nCount)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	if (nCount <= 0 || nCount > static_cast<unsigned int>(pQuestInfo->GetQObejctiveInfoCount()))
		return 0;

	XQObjectiveInfo* pQObjectiveInfo = pQuestInfo->Get(nCount);

	if (pQObjectiveInfo == NULL)
	{
		mlog("뭔가가 이상해요!!");
		return 0;
	}

	return pQObjectiveInfo->nID;
}

//const char* XGlueGameQuestFunction::GetQuestObjectiveDesc(int nQuestID, unsigned int nCount)
//{
//
//	if (nQuestID == 0) return 0;
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return 0;
//
//	if (nCount <= 0 || nCount > static_cast<unsigned int>(pQuestInfo->GetQObejctiveInfoCount()))
//		return 0;
//
//	XQObjectiveInfo* pQObjectiveInfo = pQuestInfo->Get(nCount);
//	if (pQObjectiveInfo == NULL)
//	{
//		mlog("뭔가가 이상해요!!");
//		return NULL;
//	}
//
//	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(pQObjectiveInfo->strDesc.c_str());
//
//	return gvar.Temp.szTemp.c_str();
//}

const char* XGlueGameQuestFunction::GetZoneID(int nFieldID)
{
	if (info.field)
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(info.field->GetFieldName(nFieldID));

		return gvar.Temp.szTemp.c_str();
	}

	return "";
}

const char* XGlueGameQuestFunction::GetQuestObjectiveZoneID(int nQuestID, unsigned int nCount)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	if (nCount <= 0 || nCount > static_cast<unsigned int>(pQuestInfo->GetQObejctiveInfoCount()))
		return 0;

	XQObjectiveInfo* pQObjectiveInfo = pQuestInfo->Get(nCount);
	if (pQObjectiveInfo == NULL)
	{
		mlog("뭔가가 이상해요!!");
		return NULL;
	}

	return GetZoneID(pQObjectiveInfo->nFieldID);
}

void XGlueGameQuestFunction::GiveUpQuest( int nQuestID )
{
	if (nQuestID == 0) return;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return;

	XPostQuest_RequestCancel(nQuestID);
}

//void XGlueGameQuestFunction::ShareQuest( int nQuestID )
//{
//	if (nQuestID == 0) return;
//	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
//	if (pQuestInfo == NULL) return;
//
//	XPostQuest_RequestShare(nQuestID);
//}

int XGlueGameQuestFunction::GetRewardCount(int nQuestID)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	SIZE_T size = pQuestInfo->vecReward.size();
	return size;
}

int XGlueGameQuestFunction::GetSelectableRewardCount(int nQuestID)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	SIZE_T size = pQuestInfo->vecSelectableReward.size();
	return size;
}

const char* XGlueGameQuestFunction::GetRewardTypeNParam(int nQuestID, int index)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	int size = pQuestInfo->vecReward.size();
	if (index >= size || index < 0) return 0;

	return GetTypeNParam(pQuestInfo->vecReward, nQuestID, index);
}

const char* XGlueGameQuestFunction::GetSelectableRewardTypeNParam(int nQuestID, int index)
{
	if (nQuestID == 0) return 0;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return 0;

	int size = pQuestInfo->vecSelectableReward.size();
	if (index >= size || index < 0) return 0;

	return GetTypeNParam(pQuestInfo->vecSelectableReward, nQuestID, index);
}

const char* XGlueGameQuestFunction::GetTypeNParam(VEC_QUEST_REWARD& vecReward, int nQuestID, int index)
{
	char text[256] ="";
	gvar.Temp.szTemp = "";

	QUEST_REWARD_TYPE qrt = vecReward[index].nType;
	switch (qrt)
	{
	case QRT_MONEY:
		{
			gvar.Temp.szTemp = "SILVER";
			sprintf_s(text, 256, ",%d", vecReward[index].nParam1);
			gvar.Temp.szTemp += text;
		}
		break;
	case QRT_XP:
		{
			gvar.Temp.szTemp = "XP";
			sprintf_s(text, 256, ",%d", vecReward[index].nParam1);
			gvar.Temp.szTemp += text;
		}
		break;
	case QRT_TITLE:
		{
			gvar.Temp.szTemp = "TITLE";
			sprintf_s(text, 256, ",%d", vecReward[index].nParam1);
			gvar.Temp.szTemp += text;
		}
		break;
	case QRT_ITEM:
		{
			gvar.Temp.szTemp = "ITEM";
			sprintf_s(text, 256, ",%d,%d", vecReward[index].nParam1, vecReward[index].nParam2);
			gvar.Temp.szTemp += text;
		}
		break;
	case QRT_FACTION:
		{
			gvar.Temp.szTemp = "FACTION";
			sprintf_s(text, 256, ",%d,%d", vecReward[index].nParam1, vecReward[index].nParam2);
			gvar.Temp.szTemp += text;
		}
		break;
	}

	return gvar.Temp.szTemp.c_str();
}

void XGlueGameQuestFunction::QuestAccept(int nQuestID)
{
	if (nQuestID == 0) return;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return;

	XPostQuest_RequestGive(nQuestID);
}

void XGlueGameQuestFunction::QuestReject()
{
	//global.ui_noti->SetNPCIConType(XGetInteractionInfo().InteractionTargetUID, NIT_QUEST_BEGIN);

	//if(gg.omgr)
	//{
	//	XNonPlayer* pPlayer = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
	//	if (pPlayer)
	//	{
	//		pPlayer->GetModuleEffect()->ChangeNPCIConState(NPC_ICON_STATE_QUEST_BEGIN);
	//	}
	//}
}

void XGlueGameQuestFunction::QuestReward(int nQuestID, int nSelectedReward)
{
	if (nQuestID == 0) return;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return;

	XPostQuest_RequestReward(XGetInteractionInfo().InteractionTargetUID, nQuestID, nSelectedReward);
}

const char* XGlueGameQuestFunction::QuestAbandonDestroyItem(int nQuestID )
{
	if (nQuestID == 0) return NULL;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo == NULL) return NULL;

	wchar_t szChat[512] = {0,};
	gvar.Temp.szTemp = "";

	for( int i=0; i < (int)pQuestInfo->vecDestroyItemID.size(); i++)
	{
		XItemData* pItemData = info.item->GetItemData(pQuestInfo->vecDestroyItemID[i]);
		if (NULL == pItemData) continue;

		swprintf_s(szChat, L"%s", pItemData->GetName());

		if( i != 0) gvar.Temp.szTemp += ",";

		gvar.Temp.szTemp += MLocale::ConvUTF16ToAnsi(szChat);
	}

	return gvar.Temp.szTemp.c_str();
}

void XGlueGameQuestFunction::QuestObjectivePos(int nQuestID, int nObjectiveID)
{
	if (nQuestID == 0) return;
	//XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	//if (pQuestInfo == NULL) return;

	//if (nObjectiveID <= 0 || nObjectiveID > (int)pQuestInfo->GetQObejctiveInfoCount())
	//	return;
	//XQObjectiveInfo* pQObjectiveInfo = pQuestInfo->Get(nObjectiveID);
	//if (pQObjectiveInfo == NULL)
	//{
	//	mlog("뭔가가 이상해요!!");
	//	return;
	//}

	//if(gvar.World.GetFieldID() == pQObjectiveInfo->nFieldID)
	//{
	//	if(global.script)
	//	{
	//		global.script->GetGlueGameEvent().OnMiniMapMarkPosition(pQObjectiveInfo->GetPos().x, pQObjectiveInfo->GetPos().y);

	//		const char* pStr = XStringResMgr::GetInstance()->GetString("MiniMapNewPos");
	//		global.ui->OnSystemMsg(pStr);
	//	}
	//}
}

void XGlueGameQuestFunction::QuestInteractionCancelReq()
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return;
	if( ACTION_STATE_QUEST_INTERACTION!= pModuleMyControl->GetCurrentActionStateID()) return;

	XPostQuest_RequestInteractCancel();
}