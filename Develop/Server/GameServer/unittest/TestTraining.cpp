#include "stdafx.h"
#include "SUnitTestUtil.h"
#include "MockField.h"
#include "MockDBManager.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GTrainingSystem.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "CCommandResultTable.h"
#include "GPathFinder.h"
#include "GGlobal.h"
#include "GTestWrapper.h"
#include "GConditionInfo.h"
#include "GInteractionSystem.h"
#include "GConditionsInfoMgr.h"
#include "GConditionsInfo.h"
#include "GDialogInfoMgr.h"
#include "GPlayerInteraction.h"
#include "GUTHelper.h"
#include "CCommandTable.h"
#include "GPalette.h"
#include "GPlayerTalent.h"
#include "GTalentInfoMgr.h"
#include "MockLink.h"
#include "FBaseMockLink.h"
#include "GTestForward.h"
#include "GItemHolder.h"

SUITE(TrainingSystem)
{
	struct FTrainingSystem : public FBaseMockLink
	{
		FTrainingSystem()
		{
			m_vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
			m_pCommonField = new MockField(SUnitTestUtil::NewUID());
			m_pCommonField->Create();

			m_pReqPlayer = GUTHelper::NewEntityPlayer();
			m_pReqPlayer->GetTalent().SetTP(10);
			m_pReqPlayer->GetPlayerInfo()->nLevel = 1;

			m_pReqPlayer->EnterToField(m_pCommonField, m_vCommonPos);			
		}

		~FTrainingSystem()
		{
			m_pCommonField->Destroy();
			SAFE_DELETE(m_pCommonField);

		}

		GUTHelper m_Helper;

		MockField*		m_pCommonField;
		vec3			m_vCommonPos;
		GEntityPlayer*	m_pReqPlayer;
	};

	// 레벨이 낮은 경우
	TEST_FIXTURE(FTrainingSystem, Train_LowLevel)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_bNeedTraining = true;
		pTalentInfo->m_nTNeedLevel = m_pReqPlayer->GetLevel()+1;
						
		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		vector<int> vecTalentID(1, pTalentInfo->m_nID);
		CHECK_EQUAL(false, gsys.pTrainingSystem->Train(m_pReqPlayer, pTalentInfo->m_nID));
		CHECK(false == m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP, m_pReqPlayer->GetTalent().GetTP());
	}

	// 선행 탤런트를 배우지 않은 경우
	TEST_FIXTURE(FTrainingSystem, Train_NotLearnedNeedTalentLine)
	{
		GTalentInfo* pTNeedLineTalentInfo = m_Helper.NewTalentInfo();
		pTNeedLineTalentInfo->m_bNeedTraining = true;
		pTNeedLineTalentInfo->m_nTalentLine = 100;
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_bNeedTraining = true;
		pTalentInfo->m_nTNeedTalent = pTNeedLineTalentInfo->m_nTalentLine;

		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		CHECK_EQUAL(false, gsys.pTrainingSystem->Train(m_pReqPlayer, pTalentInfo->m_nID));
		CHECK(false == m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP, m_pReqPlayer->GetTalent().GetTP());
	}

	// 투자한 StyleTP가 적은 경우
	TEST_FIXTURE(FTrainingSystem, Train_NotInvestedStyleTP)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_bNeedTraining = true;
		pTalentInfo->m_nStyle = TS_ASSASSIN;
		pTalentInfo->m_nTNeedStyleTP = 1;

		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		CHECK_EQUAL(false, gsys.pTrainingSystem->Train(m_pReqPlayer, pTalentInfo->m_nID));
		CHECK(false == m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP, m_pReqPlayer->GetTalent().GetTP());
	}

	// 같은 라인의 낮은 랭크 탤런트를 배우지 않은 경우
	TEST_FIXTURE(FTrainingSystem, Train_NotLearnedSameLineLowRank)
	{
		int nTalentLine = SUnitTestUtil::NewNumber();;
		GTalentInfo* pLowRankTalentInfo = m_Helper.NewTalentInfo(SUnitTestUtil::NewID(), nTalentLine, 1);
		pLowRankTalentInfo->m_bNeedTraining = true;
		GTalentInfo* pHighRankTalentInfo = m_Helper.NewTalentInfo(SUnitTestUtil::NewID(), nTalentLine, 2);
		pHighRankTalentInfo->m_bNeedTraining = true;

		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		CHECK_EQUAL(false, gsys.pTrainingSystem->Train(m_pReqPlayer, pHighRankTalentInfo->m_nID));
		CHECK(false == m_pReqPlayer->GetTalent().IsTrainedTalent(pHighRankTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP, m_pReqPlayer->GetTalent().GetTP());
	}

	// 이미 배운 탤런트인 경우
	TEST_FIXTURE(FTrainingSystem, Train_AlreadyLearned)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_bNeedTraining = true;
		m_pReqPlayer->GetTalent().Insert(pTalentInfo);			

		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		CHECK_EQUAL(false, gsys.pTrainingSystem->Train(m_pReqPlayer, pTalentInfo->m_nID));
		CHECK(true == m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP, m_pReqPlayer->GetTalent().GetTP());
	}

	// TP가 모자른 경우
	TEST_FIXTURE(FTrainingSystem, Train_NotEnoughTP)
	{
		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_bNeedTraining = true;
		m_pReqPlayer->GetTalent().SetTP(0);

		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		CHECK_EQUAL(false, gsys.pTrainingSystem->Train(m_pReqPlayer, pTalentInfo->m_nID));
		CHECK(false == m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP, m_pReqPlayer->GetTalent().GetTP());
	}
	
	TEST_FIXTURE(FTrainingSystem, Train)
	{
		GTalentInfo* pLearnedTalentInfo = m_Helper.LearnNewTalent(m_pReqPlayer, SUnitTestUtil::NewNumber(), 1);			 
		pLearnedTalentInfo->m_bNeedTraining = true;
		pLearnedTalentInfo->m_nStyle = TS_ASSASSIN;
		m_pReqPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_1, PIT_TALENT, pLearnedTalentInfo->m_nID);

		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo(0, pLearnedTalentInfo->m_nTalentLine, pLearnedTalentInfo->m_nRank + 1);
		pTalentInfo->m_bNeedTraining = true;
		pTalentInfo->m_nTNeedLevel = m_pReqPlayer->GetLevel();
		pTalentInfo->m_nTNeedTalent = pLearnedTalentInfo->m_nID;
		pTalentInfo->m_nStyle = pLearnedTalentInfo->m_nStyle;
		pTalentInfo->m_nTNeedStyleTP = 1;
		

		int nOrigianlRemainTP = m_pReqPlayer->GetTalent().GetTP();
		MockLink* pLink = NewLink(m_pReqPlayer);
		CHECK_EQUAL(true, gsys.pTrainingSystem->Train(m_pReqPlayer, pTalentInfo->m_nID));
		CHECK(true == m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo));
		CHECK_EQUAL(nOrigianlRemainTP - 1, m_pReqPlayer->GetTalent().GetTP());

		CHECK_EQUAL(2, pLink->GetCommandCount());
		CHECK_EQUAL(MC_PALETTE_PUTUP, pLink->GetCommand(0).GetID());
		CHECK(PALETTENUM_1 == pLink->GetParam<unsigned char>(0, 0));
		CHECK(PALETTESLOT_1 == pLink->GetParam<unsigned char>(0, 1));
		CHECK(PIT_TALENT == pLink->GetParam<unsigned char>(0, 2));
		CHECK(pTalentInfo->m_nID == pLink->GetParam<int>(0, 3));			
		CHECK_EQUAL(MC_TRAINING_TRAIN, pLink->GetCommand(1).GetID());
		CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(1, 0));			
	}


	TEST_FIXTURE(FTrainingSystem, UntrainAll)
	{
		GItem* pItem = test.item.GiveNewItem(m_pReqPlayer);
		pItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
		pItem->m_pItemData->m_nUsableType = USABLE_TALENT_UNTRAIN;

		GTalentInfo* pTalentInfo = m_Helper.NewTalentInfo();
		pTalentInfo->m_bNeedTraining = true;
		m_pReqPlayer->GetTalent().Insert(pTalentInfo);

		MockLink* pLink = NewLink(m_pReqPlayer);
		pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
		pLink->AddIgnoreID(MC_ITEM_REMOVE);
		CHECK_EQUAL(true, gsys.pTrainingSystem->UntrainAll(m_pReqPlayer, pItem));
		CHECK_EQUAL(false, m_pReqPlayer->GetTalent().IsTrainedTalent(pTalentInfo->m_nID));
		CHECK_EQUAL(0, m_pReqPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItem->m_pItemData->m_nID));
		CHECK_EQUAL(m_pReqPlayer->GetPlayerInfo()->nLevel, m_pReqPlayer->GetTalent().GetTP());

		CHECK_EQUAL(2, pLink->GetCommandCount());
		CHECK_EQUAL(MC_PALETTE_PUTDOWN_ALLTALENT, pLink->GetCommand(0).GetID());			
		CHECK_EQUAL(MC_TRAINING_UNTRAIN_ALL, pLink->GetCommand(1).GetID());			
	}
}