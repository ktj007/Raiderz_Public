#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseScriptGlue.h"
#include "GPlayerQuest.h"
#include "GSoulBindingInfo.h"
#include "GConditionsInfo.h"
#include "GPlayerFieldDynamic.h"
#include "GDynamicFieldMaster.h"
#include "GInnRoomInfo.h"
#include "GFatigueSystem.h"
#include "GPlayerTalent.h"
#include "GStrings.h"
#include "GPlayerDoing.h"
#include "GSoulBindingInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GItemManager.h"
#include "GInnRoomInfoMgr.h"
#include "GPlayerFieldLazyGateway.h"
#include "GPlayerQuests.h"
#include "GPlayerFactions.h"
#include "GFactionInfo.h"
#include "GItemHolder.h"
#include "GQuestInfo.h"
#include "CSDef_Fatigue.h"
#include "CTransData.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptFunction_Player)
{
	struct Fixture : public FBaseScriptGlue
	{
		Fixture()
		{ 
			m_strFuncName = "Function";
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);

			GFactionInfo* pFactionInfo = test.faction.NewFactionInfo(100);
			m_nFactionID = pFactionInfo->m_nID;
		}

		virtual ~Fixture() 		{}

		GEntityPlayer*	m_pPlayer;		
		string			m_strFuncName;
		string			m_strScript;

		int				m_nFactionID;

		GTestMgrWrapper<GSoulBindingInfoMgr>	m_SouBindingInfoMgr;
		GTestMgrWrapper<GConditionsInfoMgr>		m_ConditionsInfoMgr;
		GTestMgrWrapper<GItemManager>			m_ItemMgr;
		GTestMgrWrapper<GQuestInfoMgr>			m_QuestInfoMgr;
		GTestMgrWrapper<GTalentInfoMgr>			m_TalentInfoMgr;
		GTestMgrWrapper<GInnRoomInfoMgr>		m_InnRoomInfoMgr;
		GTestMgrWrapper<GFactionInfoMgr>		m_FactionInfoMgr;
	};

	TEST_FIXTURE(Fixture, TSFP_UpdateQuestVar)
	{
		m_strScript = 
			"function %s(Player, QuestID, nVar)\n"
			"	return Player:UpdateQuestVar(QuestID, nVar)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		GPlayerQuest* pQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);
		int nVar = 15;

		bool bSuccess = false;
		WCALL3(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), pQuest->GetInfo()->nID, nVar, &bSuccess);
		CHECK_EQUAL(nVar, pQuest->GetQuestVar());
	}

	TEST_FIXTURE(Fixture, TSFP_GetQuestVar)
	{
		m_strScript = 
			"function %s(Player, QuestID)\n"
			"	return Player:GetQuestVar(QuestID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		GPlayerQuest* pQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);
		pQuest->UpdateQuestVar(15);

		int nVar = 0;
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), pQuest->GetInfo()->nID, &nVar);
		CHECK_EQUAL(pQuest->GetQuestVar(), nVar);		
	}

	TEST_FIXTURE(Fixture, TSFP_BindSoul)
	{
		m_strScript = 
			"function %s(Player, SoulBindID)\n"
			"	return Player:BindSoul(SoulBindID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		GSoulBindingInfo* pSoulBindingInfo = m_Helper.NewSoulBindingInfo(NULL);
		pSoulBindingInfo->m_pMarkerInfo = new MARKER_INFO();

		bool bSuccess = false;
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), pSoulBindingInfo->m_nID, &bSuccess);
		CHECK_EQUAL(pSoulBindingInfo->m_nID, m_pPlayer->GetPlayerInfo()->nSoulBindingID);
		CHECK_EQUAL(true, bSuccess);

		SAFE_DELETE(pSoulBindingInfo->m_pMarkerInfo);
	}
	
	TEST_FIXTURE(Fixture, TSFP_CheckItem)
	{	
		m_strScript = 
			"function %s(Player, ItemID)\n"
			"	return Player:GetItem(ItemID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		GItemData* pItemData = m_Helper.NewItemData();
		int nQuantity = 3;
		GItem* pItem = m_Helper.GiveNewItem(m_pPlayer, pItemData->m_nID,  nQuantity);

		int nQtyInScript = 0;
		WCALL3(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), pItemData->m_nID, nQuantity, &nQtyInScript);
		
		CHECK_EQUAL(nQuantity, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));
		CHECK_EQUAL(nQuantity, nQtyInScript);
	}

	TEST_FIXTURE(Fixture, TSFP_CheckCondition)
	{
		m_strScript = 
			"function %s(Player, ConditionID)\n"
			"	return Player:CheckCondition(ConditionID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }				

		GConditionsInfo* pConditionsInfo = m_Helper.NewConditionsInfoForLevel(5, 5);	// min/max Level=5, id=10

		bool bCheck = false;

		m_pPlayer->GetPlayerInfo()->nLevel = 1;
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), pConditionsInfo->m_nID, &bCheck);
		CHECK_EQUAL(false, bCheck);

		m_pPlayer->GetPlayerInfo()->nLevel = 5;
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), pConditionsInfo->m_nID, &bCheck);
		CHECK_EQUAL(true, bCheck);
	}

	TEST_FIXTURE(Fixture, TSFP_EnterToTrial)
	{		
		m_strScript = 
			"function %s(Player)\n"
			"	Player:GateToTrial(1000);\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }				

		FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
		info->nID = 1000;

		bool bStartField = true;
		for (int i=100; i<=102; i++)
		{
			GFieldInfo* pFieldInfo = new GFieldInfo;
			pFieldInfo->m_bDynamic = true;
			pFieldInfo->m_nFieldID = 1000+i;
			pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			info->vecFieldInfo.push_back(pFieldInfo);

			if (true == bStartField)
			{
				MARKER_INFO marker;
				marker.nID = 1;
				marker.nFieldID = pFieldInfo->m_nFieldID;
				marker.vPoint = vec3(0);
				pFieldInfo->InsertMarkerInfo(marker);

				info->pStartFieldInfo = pFieldInfo;
				info->pStartMarkerInfo = pFieldInfo->FindMarker(1);
				bStartField = false;
			}

			gmgr.pFieldInfoMgr->Insert(pFieldInfo);
		}
		gmgr.pFieldInfoMgr->InsertGroup(info);
		
		MARKER_INFO markerExit;
		markerExit.nFieldID = 100;
		markerExit.vPoint = vec3(0,0,0);
		m_pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerExit);

		CHECK_EQUAL(m_pPlayer->GetField(), m_pField);

		m_pPlayer->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
		gmgr.pFieldMgr->Update(0.1f);
		m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

		CHECK_EQUAL(m_pPlayer->GetField(), m_pField); // failed...

		WCALL1(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), WNULL); // open dynamic field group

		m_pPlayer->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
		gmgr.pFieldMgr->Update(0.1f);
		m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(m_pPlayer->GetPlayerField().GetDynamicFieldGroupUID());
		if (!pFieldGroup)	{ CHECK(FALSE && "enter to dynamic-group field failed."); return; }

		GDynamicField* pField1100 = pFieldGroup->Get(1100);
		CHECK_EQUAL(m_pPlayer->GetField(), pField1100);
	}

	TEST_FIXTURE(Fixture, TSFP_EnterToInnRoom)
	{		
		m_strScript = 
			"function %s(Player)\n"
			"	Player:GateToInnRoom();\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }	

		FIELD_GROUP_INFO* pFieldGroupInfo = GUTHelper_Field::NewFieldGroupInfo();
		GInnRoomInfo* pInnRoomInfo = m_Helper.NewInnRoomInfo();
		pInnRoomInfo->m_nType = IRT_NORMAL;
		pInnRoomInfo->m_nFieldGroupID = pFieldGroupInfo->nID;
		m_pPlayer->GetPlayerInfo()->nInnRoomID = pInnRoomInfo->m_nID;

		bool bSuccess = false;
		WCALL1(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), &bSuccess);

		CHECK_EQUAL(true, m_pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		m_pPlayer->GetPlayerField().Update();

		MUID uidReservedWarpField = m_pPlayer->GetPlayerField().GetFieldEntry().GetReservedWarpField();
		GField* pField = gmgr.pFieldMgr->GetField(uidReservedWarpField);
		CHECK_EQUAL(pFieldGroupInfo->pStartFieldInfo->m_nFieldID, pField->GetInfo()->m_nFieldID);
	}


	TEST_FIXTURE(Fixture, TSFP_SetFatigue)
	{
		m_strScript = 
			"function %s(Player, FatiguePercent)\n"
			"	return Player:SetFatigue(FatiguePercent)\n"
			"end\n";

		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), 50, WNULL);
		CHECK_EQUAL(BEST_FATIGUE * 50 / 100, m_pPlayer->GetPlayerInfo()->nFatigueQuantity);
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), 100, WNULL);
		CHECK_EQUAL(BEST_FATIGUE, m_pPlayer->GetPlayerInfo()->nFatigueQuantity);
	}

	TEST_FIXTURE(Fixture, TSFP_Tip)
	{
		m_strScript = 
			"function %s(Player)\n"
			"	return Player:Tip(\"Test\")\n"
			"end\n";

		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		MockLink* pLink = NewLink(m_pPlayer);

		WCALL1(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), WNULL);
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_TIP, pLink->GetCommand(0).GetID());
		CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 0));		
	}

	TEST_FIXTURE(Fixture, TSFP_AddMoney)
	{
		m_strScript = 
			"function %s(Player, nMoney)\n"
			"	return Player:AddSilver(nMoney)\n"
			"end\n";

		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		int nOriginalMoney = 1000;
		int nAddMoney = 300;
		m_pPlayer->GetPlayerInfo()->SetMoney(nOriginalMoney);

		bool bSuccess = false;
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), nAddMoney, &bSuccess);
		CHECK_EQUAL(nOriginalMoney + nAddMoney, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(true, bSuccess);
	}	

	TEST_FIXTURE(Fixture, TSFP_RemoveMoney)
	{
		m_strScript = 
			"function %s(Player, nMoney)\n"
			"	return Player:RemoveSilver(nMoney)\n"
			"end\n";

		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		int nOriginalMoney = 1000;
		int nRemoveMoney = 300;
		m_pPlayer->GetPlayerInfo()->SetMoney(nOriginalMoney);

		bool bSuccess = false;
		WCALL2(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), nRemoveMoney, &bSuccess);
		CHECK_EQUAL(nOriginalMoney - nRemoveMoney, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(true, bSuccess);
	}	

	TEST_FIXTURE(Fixture, TSFP_GetMoney)
	{
		m_strScript = 
			"function %s(Player)\n"
			"	return Player:GetSilver()\n"
			"end\n";

		if (!m_Helper.RunLuaString(m_strScript.c_str(), m_strFuncName.c_str()))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		int nOriginalMoney = 1000;
		m_pPlayer->GetPlayerInfo()->SetMoney(nOriginalMoney);

		bool bSuccess = false;
		WCALL1(WLUA, m_strFuncName.c_str(), m_pPlayer->GetDelegator(), &bSuccess);
		CHECK_EQUAL(nOriginalMoney, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(true, bSuccess);
	}	

	TEST_FIXTURE(Fixture, TSFP_Gate_ToSameSharedField)
	{
		const char* pszTestFuncName = "_Test_Player_Gate";
		const char* pszScript = 
			"function %s(Player)\n"
			"	Player:GateToMarker(100, 777);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 추가적인 다른 공유 필드 생성
		MARKER_INFO MarkerInfo;
		MarkerInfo.nID = 777;
		MarkerInfo.nFieldID = 100;
		MarkerInfo.vPoint = vec3(7737.0f, 3321.0f, 2134.0f);
		m_pFieldInfo->InsertMarkerInfo(MarkerInfo);

		// 플레이어 워프
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true); 
		WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), WNULL);
		CHECK_EQUAL(true, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		gmgr.pFieldMgr->Update(0.1f);
		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vec3(7737.0f, 3321.0f, 2134.0f)), true); 
	}

	TEST_FIXTURE(Fixture, TSFP_Gate_ToOtherSharedField)
	{
		const char* pszTestFuncName = "_Test_Player_Gate";
		const char* pszScript = "function %s(Player)\n"
			"	return Player:GateToMarker(101, 777);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 추가적인 다른 공유 필드 생성
		GFieldInfo* pOtherFieldInfo = new GFieldInfo;
		pOtherFieldInfo->m_nFieldID = 101;
		pOtherFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
		MARKER_INFO MarkerInfo;
		MarkerInfo.nID = 777;
		MarkerInfo.nFieldID = 101;
		MarkerInfo.vPoint = vec3(7737.0f, 3321.0f, 2134.0f);
		pOtherFieldInfo->InsertMarkerInfo(MarkerInfo);
		gmgr.pFieldInfoMgr->Insert(pOtherFieldInfo);

		GUTHelper_Field::NewSharedField(pOtherFieldInfo, 1);

		// 플레이어 워프
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		CHECK_EQUAL(pPlayer->GetField()->GetInfo()->m_nFieldID, 100); 
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true); 
		CHECK_EQUAL(pPlayer->GetReferenceCount(), TFT_SIZE+1);
		bool bLuaSuccess=false;			
		WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), &bLuaSuccess);
		if (!bLuaSuccess)	{ CHECK(FALSE && "루아 워프 함수 실패"); return; }
		CHECK_EQUAL(pPlayer->GetReferenceCount(), TFT_SIZE+1);
		CHECK_EQUAL(true, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		gmgr.pFieldMgr->Update(0.1f);
		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(pPlayer->GetField()->GetInfo()->m_nFieldID, 101); 
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vec3(7737.0f, 3321.0f, 2134.0f)), true); 
		CHECK_EQUAL(pPlayer->GetReferenceCount(), TFT_SIZE+1);
	}

	TEST_FIXTURE(Fixture, TSFP_Gate_ToDynamic)
	{
		const char* pszTestFuncName = "_Test_Player_Warp";
		const char* pszScript = 
			"function %s(Player)\n"
			"	return Player:GateToMarker(1100, 777);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
		info->nID = 10000;
		GFieldInfo* pFieldInfo = new GFieldInfo;
		pFieldInfo->m_bDynamic = true;
		pFieldInfo->m_nFieldID = 1100;
		pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
		{
			MARKER_INFO MarkerInfo;
			MarkerInfo.nID = 777;
			MarkerInfo.nFieldID = 1100;
			MarkerInfo.vPoint = vec3(7737.0f, 3321.0f, 2134.0f);
			pFieldInfo->InsertMarkerInfo(MarkerInfo);
		}
		info->vecFieldInfo.push_back(pFieldInfo);
		info->pStartFieldInfo = pFieldInfo;
		info->pStartMarkerInfo = pFieldInfo->FindMarker(777);
		gmgr.pFieldInfoMgr->Insert(pFieldInfo);
		gmgr.pFieldInfoMgr->InsertGroup(info);

		GEntityPlayer* pOwnerPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		if (!pOwnerPlayer->GetPlayerField().GateToTrial(10000))
		{
			CHECK(FALSE && "동적 필드 생성 실패"); 
			return;
		}
		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pOwnerPlayer);

		// 플레이어 워프
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		MARKER_INFO markerExit;
		markerExit.nFieldID = 100;
		markerExit.vPoint = vec3(0,0,0);
		pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerExit);

		pPlayer->GetPlayerField().SetDynamicFieldGroupUID(pOwnerPlayer->GetPlayerField().GetDynamicFieldGroupUID());		// 동적 필드 공유
		CHECK_EQUAL(pPlayer->GetField()->GetInfo()->m_nFieldID, 100); 
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true); 
		CHECK_EQUAL(pPlayer->GetReferenceCount(), TFT_SIZE+1);
		bool bLuaSuccess=false;
		WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), &bLuaSuccess);
		if (!bLuaSuccess)	{ CHECK(FALSE && "루아 워프 함수 실패"); return; }
		CHECK_EQUAL(pPlayer->GetReferenceCount(), TFT_SIZE+1);
		CHECK_EQUAL(true, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		gmgr.pFieldMgr->Update(0.1f);
		CHECK_EQUAL(false, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(pPlayer->GetField()->GetInfo()->m_nFieldID, 1100); 
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vec3(7737.0f, 3321.0f, 2134.0f)), true); 
		CHECK_EQUAL(pPlayer->GetReferenceCount(), TFT_SIZE+1);
	}

	TEST_FIXTURE(Fixture, TSFP_GateToActor)
	{
		const char* pszTestFuncName = "_Test_Player_GateToActor";
		const char* pszScript = 
			"function %s(Player,TargetActor)\n"
			"	return Player:GateToActor(TargetActor);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityPlayer* pTargetPlayer = GUTHelper::NewEntityPlayer(m_pField, vec3(100.0f, 100.0f, 0.0f));

		// 플레이어 워프
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true); 

		bool bLuaSuccess=false;
		WCALL2(WLUA, pszTestFuncName, pPlayer->GetDelegator(), pTargetPlayer->GetDelegator(), &bLuaSuccess);
		if (!bLuaSuccess)	{ CHECK(FALSE && "루아 워프 함수 실패"); return; }
		CHECK_EQUAL(true, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		gmgr.pFieldMgr->Update(0.1f);
		CHECK_EQUAL(false, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		CHECK_EQUAL(pPlayer->GetPos(), pTargetPlayer->GetPos());
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vec3(100.0f, 100.0f, 0.0f)), true);			
	}

	TEST_FIXTURE(Fixture, TSFP_Gate)
	{
		const char* pszTestFuncName = "_Test_Player_Gate";
		const char* pszScript = 
			"function %s(Player,FieldID,StartPos)\n"
			"	return Player:Gate(FieldID, StartPos);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		// 추가적인 다른 공유 필드 생성
		GFieldInfo* pOtherFieldInfo = new GFieldInfo;
		pOtherFieldInfo->m_nFieldID = 101;
		pOtherFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
		gmgr.pFieldInfoMgr->Insert(pOtherFieldInfo);

		GUTHelper_Field::NewSharedField(pOtherFieldInfo);

		// 플레이어 워프
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true); 

		vec3 vStartPos(100,200,300);

		bool bLuaSuccess=false;
		WCALL3(WLUA, pszTestFuncName, pPlayer->GetDelegator(), 101, vStartPos, &bLuaSuccess);
		if (!bLuaSuccess)	{ CHECK(FALSE && "루아 워프 함수 실패"); return; }
		CHECK_EQUAL(true, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		gmgr.pFieldMgr->Update(0.1f);
		CHECK_EQUAL(false, pPlayer->GetPlayerField().GetLazyGateway().IsScheduled());
		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

		CHECK_EQUAL(101, pPlayer->GetFieldID());			
		CHECK_EQUAL(true, pPlayer->GetPos().IsEqual(vStartPos));			
	}

	TEST_FIXTURE(Fixture, TSFP_IsCompleteQuest)
	{
		const char* pszTestFuncName = "_Test_Player_IsQuestComplete";
		const char* pszScript = 
			"function %s(Player, QuestID)\n"
			"	return Player:IsCompleteQuest(QuestID);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		// 플레이어 워프
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer);

		bool bQuestComplete = false;
		WCALL2(WLUA, pszTestFuncName, pPlayer->GetDelegator(), pPlayerQuest->GetInfo()->nID, &bQuestComplete);
		CHECK_EQUAL(false, bQuestComplete);

		pPlayerQuest->Complete();
		WCALL2(WLUA, pszTestFuncName, pPlayer->GetDelegator(), pPlayerQuest->GetInfo()->nID, &bQuestComplete);
		CHECK_EQUAL(true, bQuestComplete);
	}

	TEST_FIXTURE(Fixture, GetName)
	{
		wcscpy(m_pPlayer->GetPlayerInfo()->szName, L"TestName");
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Player)\n"
			"	TEST_VAR = Player:GetName()\n"
			"end\n"
			;

		CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		WCALL1(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), WNULL);

		TVALID(WLUA->IsExistGlobal("TEST_VAR"));
		CHECK_EQUAL(string("TestName"), WLUA->GetVar<const char*>("TEST_VAR"));
	}

	TEST_FIXTURE(Fixture, TSFP_Cutscene)
	{
		const char* pszTestFuncName = "_Test_Player_Cutscene";
		const char* pszScript = 
			"function %s(Player)\n"
			"	return Player:Cutscene(1);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		bool bComplete = false;
		WCALL1(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), &bComplete);
		CHECK_EQUAL(true, m_pPlayer->GetDoing().IsNowCutscening());
	}

	TEST_FIXTURE(Fixture, TSFP_PartyCutscene)
	{
		const char* pszTestFuncName = "_Test_Player_PartyCutscene";
		const char* pszScript = 
			"function %s(Player)\n"
			"	return Player:PartyCutscene(1);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer2 = m_Helper.NewEntityPlayer(m_pField);		
		GUTHelper_Party::CreateParty(m_pPlayer, pPlayer2);

		bool bComplete = false;
		WCALL1(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), &bComplete);
		CHECK_EQUAL(true, m_pPlayer->GetDoing().IsNowCutscening());
		CHECK_EQUAL(true, pPlayer2->GetDoing().IsNowCutscening());
	}

	TEST_FIXTURE(Fixture, TSFP_AddQuest)
	{
		const char* pszTestFuncName = "_Test_Player_AddQuest";
		const char* pszScript = 
			"function %s(Player, QuestID)\n"
			"	return Player:AddQuest(QuestID);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();

		CHECK_EQUAL(0, m_pPlayer->GetQuests().GetQuestContainer().size());

		WCALL2(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), pQuestInfo->nID, WNULL);

		CHECK_EQUAL(1, m_pPlayer->GetQuests().GetQuestContainer().size());
	}

	TEST_FIXTURE(Fixture, TSFP_RuinQuest)
	{
		const char* pszTestFuncName = "_Test_Player_RuinQuest";
		const char* pszScript = 
			"function %s(Player, QuestID)\n"
			"	return Player:RuinQuest(QuestID);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GPlayerQuest* pQuest = test.quest.GiveNewPlayerQuest(m_pPlayer);

		CHECK_EQUAL(QST_EXIST, pQuest->GetState());

		WCALL2(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), pQuest->GetInfo()->nID, WNULL);

		CHECK_EQUAL(QST_FAIL, pQuest->GetState());
	}

	TEST_FIXTURE(Fixture, AddAmity)
	{
		const char* pszTestFuncName = "_Test_Player_AddAmity";
		const char* pszScript =
			"function %s(Player, FactionID, Quantity)\n"
			"	Player:AddAmity(FactionID, Quantity);\n"
			"end";

		ASSERT_CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));


		WCALL3(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), m_nFactionID, 10, WNULL);

		
		GPlayerFactions& factions = m_pPlayer->GetPlayerFactions();
		CHECK_EQUAL(110, factions.GetQuantity(m_nFactionID));
	}

	TEST_FIXTURE(Fixture, RemoveAmity)
	{
		const char* pszTestFuncName = "_Test_Player_RemoveAmity";
		const char* pszScript = 
			"function %s(Player, FactionID, Quantity)\n"
			"	Player:RemoveAmity(FactionID, Quantity);\n"
			"end";

		ASSERT_CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));


		WCALL3(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), m_nFactionID, 10, WNULL);


		GPlayerFactions& factions = m_pPlayer->GetPlayerFactions();
		CHECK_EQUAL(90, factions.GetQuantity(m_nFactionID));
	}

	TEST_FIXTURE(Fixture, SetAmity)
	{
		const char* pszTestFuncName = "_Test_Player_SetAmity";
		const char* pszScript = 
			"function %s(Player, FactionID, Quantity)\n"
			"	Player:SetAmity(FactionID, Quantity);\n"
			"end";

		ASSERT_CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));


		WCALL3(WLUA, pszTestFuncName, m_pPlayer->GetDelegator(), m_nFactionID, 10, WNULL);


		GPlayerFactions& factions = m_pPlayer->GetPlayerFactions();
		CHECK_EQUAL(10, factions.GetQuantity(m_nFactionID));
	}
}
