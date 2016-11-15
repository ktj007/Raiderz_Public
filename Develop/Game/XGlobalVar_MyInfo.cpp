#include "stdafx.h"
#include "XGlobalVar_MyInfo.h"
#include "XNPCInfo.h"
#include "XNonPlayer.h"
#include "XMyPlayer.h"
#include "XItemManager.h"
#include "XRecipeInfoMgr.h"

GlobalVar_MyInfo::GlobalVar_MyInfo()
{
	MyUID.SetZero();
	memset(&ChrStatus, 0, sizeof(CHR_STATUS));
	m_fExpPercent = 0;
	nLevel = 1;
	nRecievedQuest = 0;
	nCompletedQuest = 0;
	nRecievedRecipe = 0;
	nCompletedRecipe = 0;
	nKilledEnemy = 0;
	nKilledEpicEnemy = 0;
	bGod = false;

	nFocusType = TFT_NONE;
	nBubblePercent = 0;

	nDieTime = 0;
	fAutoRebirthRemainCoolTime = 0.f;
	m_nUIID = UIID_INVALID;

	CraftableRecipeInfo.Clear();

	nSoulbindingFieldID = 0;
	nCheckPointFieldID = 0;
}

GlobalVar_MyInfo::~GlobalVar_MyInfo()
{
	Clear();
}

void GlobalVar_MyInfo::Clear()
{
	this->FactionInfo.Clear();
	this->Party.Clear();
	this->Guild.Clear();
}

void GlobalVar_MyInfo::SubChrStatus( XTalentInfo* pTalentInfo )
{
	//SetHP(ChrStatus.nHP - pTalentInfo->m_nHPCost);
	SetEN(ChrStatus.nEN - pTalentInfo->m_nENCost);
	SetSTA(ChrStatus.nSTA - pTalentInfo->m_nSTACost);
}

void GlobalVar_MyInfo::SetFrom_TD_MYINFO(
	const TD_MYINFO& TDMyInfo, 
	const vector<TD_ITEM>& vecTDInventory, 
	const vector<TD_ITEM>& vecTDEquipment,
	const vector<int>& vecTalentID, 
	const vector<TD_PLAYERQUEST>& vecTDPlayerQuest, 
	const TD_PALETTE& TDPalette,
	const vector<TD_FACTION>& vecTDFaction, 
	const vector<int>& vecRecipeID)
{
	// 위치 변경	_by tripleJ 110506
	//this->Assign(TDMyInfo);

	// 인벤토리 아이템
	this->Inventory.Clear();
	for each (TD_ITEM tdItem in vecTDInventory)
	{
		XItem* pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);
		if (pNewItem == NULL) 
		{
			mlog3("No Item ID = %d\n", tdItem.m_nItemID);
			_ASSERT(0);
			continue;
		}

		pNewItem->ApplyTDItem(tdItem);
		this->Inventory.PushItem(tdItem.m_nSlotID, pNewItem);
	}

	// Modifier 초기화
	gvar.MyInfo.ChrStatus.ActorModifier.Clear();
	gvar.MyInfo.ActorModifier_ExceptTalent.Clear();
	gvar.MyInfo.PlayerModifier.Clear();
	gvar.MyInfo.PlayerModifier_ExceptTalent.Clear();

	// 배운 탤런트들
	Talent.InitTalent();
	for each (int nTalentID in vecTalentID)
	{
		XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
		_ASSERTE(NULL != pTalentInfo && "pTalentInfo is NULL");
		if (pTalentInfo)
		{
			this->Talent.Train(pTalentInfo, false);
		}
	}

	// 장비 아이템
	EquipmentSlot.Clear();
	for each (TD_ITEM tdItem in vecTDEquipment)
	{
		XItem* pNewItem = info.item->MakeNewItem(tdItem.m_nItemID);
		if (pNewItem == NULL) 
		{
			mlog3("No Item ID = %d\n", tdItem.m_nItemID);
			_ASSERT(0);
			continue;
		}

		pNewItem->ApplyTDItem(tdItem);
		EquipmentSlot.EquipItem(pNewItem, SH_ITEM_SLOT(pNewItem->m_nSlotID));

		if(XGetMyPlayer() != NULL)
			XGetMyPlayer()->EnchantBuffAllGainForEquip(pNewItem, SH_ITEM_SLOT(pNewItem->m_nSlotID));
	}

	this->EquipmentSlot.SwitchingWeaponSet(TDMyInfo.nWeaponSet);
	
	// 위치 변경	_by tripleJ 110506
	this->Assign(TDMyInfo);

	// 소셜 탤런트들
	vector<int> vecSocialID;
	info.talent->GetSocialTalentID(vecSocialID);
	for each (int nSocialTalentID in vecSocialID)
	{
		this->Talent.SetSocialTalentID(nSocialTalentID);
	}

	// 퀘스트
	this->Quests.Clear();
	for each (TD_PLAYERQUEST tdPlayerQuest in vecTDPlayerQuest)
	{
		XPlayerQuest* pPlayerQuest = this->Quests.Insert(tdPlayerQuest.nQuestID);
		if (NULL == pPlayerQuest) continue;

		pPlayerQuest->m_nState = tdPlayerQuest.nState;
		pPlayerQuest->m_nAcceptTime = tdPlayerQuest.nAcceptTime;
		pPlayerQuest->m_nLimitTime = tdPlayerQuest.nLimitTime;
		pPlayerQuest->bChallengerQuest = tdPlayerQuest.bChallengerQuest;
		pPlayerQuest->m_pQuestInfo->vRewardPos = tdPlayerQuest.vRewardPos;
		for (int j = 0; j < MAX_PLAYERQOBJECTIVE; j++)
		{
			XPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(tdPlayerQuest.playerQObjectives[j].nQObjectiveID);
			if (NULL == pPlayerQObjective) continue;

			pPlayerQObjective->bComplete = tdPlayerQuest.playerQObjectives[j].bComplete;
			pPlayerQObjective->nProgress = tdPlayerQuest.playerQObjectives[j].nProgress;
		}		
	}

	// 레시피 아이디
	this->RecipeInfo.Clear();
	for each (int nRecipeID in vecRecipeID)
	{
		XRecipeInfo* pRecipeInfo = info.recipe->Get(nRecipeID);
		if (pRecipeInfo == NULL) continue;

		gvar.MyInfo.RecipeInfo.push_back(nRecipeID);
	}

	//팔레트 리스트
	this->PaletteList.Clear();
	this->PaletteList.SetCurPaletteListIndex(TDPalette.nSelectedNum);
	this->PaletteList.BindingByPaletteNum(TDPalette.nPrimaryNum, TDPalette.nSecondaryNum);
	for(int i = 0; i< (PALETTENUM_MAX*PALETTESLOT_MAX); i++)
	{
		TD_PALETTE_ITEM td = TDPalette.paletteItems[i];
		if (td.nType == PIT_ITEM)
		{
			XPaletteItem item;
			item.itemID = td.nItemIDorTalentID;
			this->PaletteList.SetItemAtIndex(i, item);
		}
		else if (td.nType == PIT_TALENT)
		{
			XPaletteItem item;
			item.talentID = td.nItemIDorTalentID;
			this->PaletteList.SetItemAtIndex(i, item);
		}
		else
		{
			XPaletteItem item;
			this->PaletteList.SetItemAtIndex(i, item);
		}
	}
	//mlog0("Recved Inventory Item qty = %d\n", gvar.MyInfo.Inventory.GetSize());

	// 팩션
	for each (TD_FACTION tdFaction in vecTDFaction)
	{
		FactionInfo.SetQuantity(tdFaction.nID, tdFaction.nQuantity);
	}	

	nRecievedQuest = 0;
	nCompletedQuest = 0;
	nRecievedRecipe = 0;
	nCompletedRecipe = 0;
	nKilledEnemy = 0;
	nKilledEpicEnemy = 0;
}

TD_MYINFO GlobalVar_MyInfo::AsTransDataMyInfo()
{
	TD_MYINFO ret_td_myinfo;

	// 기본 정보
	wcsncpy_s(ret_td_myinfo.szName, this->ChrInfo.szName, _TRUNCATE);
	wcsncpy_s(ret_td_myinfo.szSurname, this->ChrInfo.szSurname, _TRUNCATE);

	if (this->ChrInfo.nSex == SEX_MALE) ret_td_myinfo.nSex = SEX_MALE;
	else ret_td_myinfo.nSex = SEX_FEMALE;
	ret_td_myinfo.nFeatureHair = this->ChrInfo.nFeatureHair;
	ret_td_myinfo.nFeatureFace = this->ChrInfo.nFeatureFace;
	ret_td_myinfo.nLevel = this->ChrInfo.nLevel;
	ret_td_myinfo.nMoney = this->ChrInfo.GetMoney();
	ret_td_myinfo.nMaxHP = this->ChrInfo.nMaxHP;
	ret_td_myinfo.nMaxEN = this->ChrInfo.nMaxEN;
	ret_td_myinfo.nMaxSTA = this->ChrInfo.nMaxSTA;
	ret_td_myinfo.nXP = this->ChrInfo.nXP;
	ret_td_myinfo.nSTR = this->ChrInfo.nSTR;
	ret_td_myinfo.nDEX = this->ChrInfo.nDEX;
	ret_td_myinfo.nINT = this->ChrInfo.nINT;
	ret_td_myinfo.nCHA = this->ChrInfo.nCHA;
	ret_td_myinfo.nCON = this->ChrInfo.nCON;
	ret_td_myinfo.nPlayerGrade = static_cast<int8>(this->ChrInfo.nPlayerGrade);

	// 동적 정보
	ret_td_myinfo.nCurHP = this->ChrStatus.nHP;
	ret_td_myinfo.nCurEN = this->ChrStatus.nEN;
	ret_td_myinfo.nCurSTA = this->ChrStatus.nSTA;

	ret_td_myinfo.nLevel = this->nLevel;
	ret_td_myinfo.fExpPercent = this->GetExpPercent();
	ret_td_myinfo.nRemainTP = this->Talent.GetTP();

	return ret_td_myinfo;
}

void GlobalVar_MyInfo::Assign( const TD_MYINFO& td_myinfo )
{
	// 기본 정보
	wcscpy_s(this->ChrInfo.szName, td_myinfo.szName);
	wcscpy_s(this->ChrInfo.szSurname, td_myinfo.szSurname);

	if (td_myinfo.nSex == SEX_MALE) this->ChrInfo.nSex = SEX_MALE;
	else this->ChrInfo.nSex = SEX_FEMALE;
	this->ChrInfo.nFeatureHair = td_myinfo.nFeatureHair;
	this->ChrInfo.nFeatureFace = td_myinfo.nFeatureFace;
	this->ChrInfo.nFeatureHairColor = td_myinfo.nFeatureHairColor;
	this->ChrInfo.nFeatureSkinColor = td_myinfo.nFeatureSkinColor;

	this->ChrInfo.nMakeUp = td_myinfo.nMakeUp;
	this->ChrInfo.nTattooType = td_myinfo.nTattooType;
	this->ChrInfo.nTattooPosX = td_myinfo.nTattooPosX;
	this->ChrInfo.nTattooPosY = td_myinfo.nTattooPosY;
	this->ChrInfo.nTattooScale = td_myinfo.nTattooScale;

	this->ChrInfo.nLevel = td_myinfo.nLevel;
	this->ChrInfo.SetMoney(td_myinfo.nMoney);
	this->ChrInfo.nMaxHP = td_myinfo.nMaxHP;
	this->ChrInfo.nMaxEN = td_myinfo.nMaxEN;
	this->ChrInfo.nMaxSTA = td_myinfo.nMaxSTA;
	this->ChrInfo.nXP = td_myinfo.nXP;
	this->ChrInfo.nSTR = td_myinfo.nSTR;
	this->ChrInfo.nDEX = td_myinfo.nDEX;
	this->ChrInfo.nINT = td_myinfo.nINT;
	this->ChrInfo.nCHA = td_myinfo.nCHA;
	this->ChrInfo.nCON = td_myinfo.nCON;
	this->ChrInfo.nPlayerGrade = static_cast<GPlayerGrade>(td_myinfo.nPlayerGrade);		

	// 동적 정보
	this->ChrStatus.nHP = td_myinfo.nCurHP;
	this->ChrStatus.nEN = td_myinfo.nCurEN;
	this->ChrStatus.nSTA = td_myinfo.nCurSTA;

	this->nLevel = td_myinfo.nLevel;
	this->SetExpPercent(td_myinfo.fExpPercent);
	this->Talent.SetTP(td_myinfo.nRemainTP);
	this->nFatigueType = static_cast<FATIGUE_TYPE>(td_myinfo.nFatigueType);	

	this->nSoulbindingFieldID = td_myinfo.nSoulbindingFieldID;
	this->nCheckPointFieldID = td_myinfo.nCheckPointFieldID;

	this->Mailbox.SetSummaryInfo(td_myinfo.bExistUnreadMail, td_myinfo.bIsFullMailbox);
}

void GlobalVar_MyInfo::ResetByMoveGameServer()
{
	this->Mailbox.Clear();
}
