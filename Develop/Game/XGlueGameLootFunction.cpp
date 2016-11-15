#include "stdafx.h"
#include "XGlueGameLootFunction.h"
#include "XItemManager.h"
#include "XEventID.h"
#include "XMyPlayer.h"

void XGlueGameLootFunction::LootItem( const char* szItemID, const char* szItemCount, bool bPass /*= false*/ )
{
	XItemData* pItemData = info.item->GetItemData(atoi(szItemID));
	if (pItemData == NULL) return;

	if ((bPass == false) &&
		(pItemData->m_bQuestRelated == false) &&	//퀘스트 아이템이 아니고
		(pItemData->m_bTradable == false))		// 획귀
	{
		global.script->GetGlueGameEvent().OnLootItemBind(MLocale::ConvUTF16ToAnsi(pItemData->GetName()).c_str(), szItemID, szItemCount);		
		return;
	}

	MUID nDropItemUID;
	// TODO : DropItemUID	

	XEvent evt;
	evt.m_nID = XEVTL_LOOT_GETITEM;
	evt.m_pData = &nDropItemUID;
	gg.omgr->PostEvent(gvar.MyInfo.MyUID, evt);
}

void XGlueGameLootFunction::LootEndRequest()
{
	// 루팅 종료 메세지 보내기
	gvar.Game.pMyPlayer->GetModuleMyControl()->OnLootEndReady();
}
