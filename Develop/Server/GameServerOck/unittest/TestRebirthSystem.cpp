#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GRebirthSystem.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GFactionInfo.h"
#include "GModuleBuff.h"

SUITE(RebirthSystem)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			test.buff.NewBuffInfo(GConst::REBIRTH_SOULBINDING_DEBUFF);
			test.buff.NewBuffInfo(GConst::REBIRTH_CHECKPOINT_DEBUFF);
			test.buff.NewBuffInfo(GConst::REBIRTH_QUESTPVP_DEBUFF);
			test.buff.NewBuffInfo(GConst::REBIRTH_QUESTPVP_BUFF);

			m_pPlayer->GetPlayerInfo()->nLevel = GConst::REBIRTH_DEBUFF_TARGET_LEVEL + 1;
		}

		GUTHelper m_Helper;
		GTestMgrWrapper<GSoulBindingInfoMgr>	m_SoulBindingInfoMgr;
	};

	TEST_FIXTURE(Fixture, RebirthToPlace)
	{
		int nBeforeField = m_pPlayer->GetFieldID();
		vec3 vBeforePos = m_pPlayer->GetPos();
		float fRecoverRate = 0.5f;
		m_pPlayer->doDie();
		
		CHECK_EQUAL(true, gsys.pRebirthSystem->RebirthToPlace(m_pPlayer, m_pPlayer, fRecoverRate));

		CHECK_EQUAL(false, m_pPlayer->IsDead());
		CHECK_EQUAL(nBeforeField, m_pPlayer->GetFieldID());
		CHECK_EQUAL(vBeforePos, m_pPlayer->GetPos());
		CHECK_EQUAL(fRecoverRate*100.0f, m_pPlayer->GetHPPercent());
	}

	TEST_FIXTURE(Fixture, RebirthToSoulBinding)
	{
		MARKER_INFO* pMarkerInfo = test.field.InsertNewMarkerInfo(m_pField->GetInfo());
		GSoulBindingInfo* pSoulBindingInfo =  m_Helper.NewSoulBindingInfo(pMarkerInfo);
		m_pPlayer->GetPlayerInfo()->nSoulBindingID = pSoulBindingInfo->m_nID;
		m_pPlayer->doDie();

		CHECK_EQUAL(true, gsys.pRebirthSystem->RebirthToSoulBinding(m_pPlayer));

		CHECK_EQUAL(false, m_pPlayer->IsDead());
		CHECK_EQUAL(pMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
		CHECK_EQUAL(pMarkerInfo->vPoint, m_pPlayer->GetPos());
		CHECK_EQUAL(GConst::REBIRTH_SOULBINDING_RECOVER*100.0f, m_pPlayer->GetHPPercent());
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsGained(GConst::REBIRTH_SOULBINDING_DEBUFF));
	}

	TEST_FIXTURE(Fixture, RebirthToNearSoulBinding)
	{
		vec3 vFarPos = vec3(m_pPlayer->GetPos().x + 1000.0f, m_pPlayer->GetPos().y + 1000.0f, m_pPlayer->GetPos().z);
		vec3 vNearPos = vec3(m_pPlayer->GetPos().x + 500.0f, m_pPlayer->GetPos().y + 500.0f, m_pPlayer->GetPos().z);

		MARKER_INFO* pFarMarkerInfo = test.field.InsertNewMarkerInfo(m_pField->GetInfo());
		pFarMarkerInfo->vPoint = vFarPos;
		GSoulBindingInfo* pFarSoulBindingInfo =  m_Helper.NewSoulBindingInfo(pFarMarkerInfo);
		MARKER_INFO* pNearEnemyMarkerInfo = test.field.InsertNewMarkerInfo(m_pField->GetInfo());
		pNearEnemyMarkerInfo->vPoint = vNearPos;
		GSoulBindingInfo* pNearEnemySoulBindingInfo =  m_Helper.NewSoulBindingInfo(pNearEnemyMarkerInfo);
		GFactionInfo* pFactionInfo = test.faction.NewFactionInfo();
		pNearEnemySoulBindingInfo->m_nFactionID = pFactionInfo->m_nID;
		test.faction.SetBadFaction(m_pPlayer, pFactionInfo->m_nID);
		MARKER_INFO* pNearMarkerInfo = test.field.InsertNewMarkerInfo(m_pField->GetInfo());
		pNearMarkerInfo->vPoint = vNearPos;
		GSoulBindingInfo* pNearSoulBindingInfo =  m_Helper.NewSoulBindingInfo(pNearMarkerInfo);		
		gmgr.pSoulBindingInfoMgr->Cooking();
		m_pPlayer->doDie();

		CHECK_EQUAL(true, gsys.pRebirthSystem->RebirthToNearSoulBinding(m_pPlayer));

		CHECK_EQUAL(false, m_pPlayer->IsDead());
		CHECK_EQUAL(pNearMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
		CHECK_EQUAL(pNearMarkerInfo->vPoint, m_pPlayer->GetPos());
		CHECK_EQUAL(GConst::REBIRTH_SOULBINDING_RECOVER*100.0f, m_pPlayer->GetHPPercent());
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsGained(GConst::REBIRTH_SOULBINDING_DEBUFF));
	}	

	TEST_FIXTURE(Fixture, RebirthToQPVP)
	{
		MARKER_INFO* pMarkerInfo1 = test.field.InsertNewMarkerInfo(m_pField->GetInfo());
		MARKER_INFO* pMarkerInfo2 = test.field.InsertNewMarkerInfo(m_pField->GetInfo());
		const_cast<GFieldInfo*>(m_pField->GetInfo())->m_nQPVPRebirthMarkerID1 = pMarkerInfo1->nID;
		const_cast<GFieldInfo*>(m_pField->GetInfo())->m_nQPVPRebirthMarkerID2 = pMarkerInfo2->nID;
		test.player.SetMockPlayerPVPArea_LocateAndTeamOne(m_pPlayer);		
		m_pPlayer->doDie();

		CHECK_EQUAL(true, gsys.pRebirthSystem->RebirthToQPVP(m_pPlayer));

		CHECK_EQUAL(false, m_pPlayer->IsDead());
		CHECK_EQUAL(pMarkerInfo1->nFieldID, m_pPlayer->GetFieldID());
		CHECK_EQUAL(pMarkerInfo1->vPoint, m_pPlayer->GetPos());
		CHECK_EQUAL(GConst::REBIRTH_QUESTPVP_RECOVER*100.0f, m_pPlayer->GetHPPercent());
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsGained(GConst::REBIRTH_QUESTPVP_BUFF));
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsGained(GConst::REBIRTH_QUESTPVP_DEBUFF));
	}

	TEST_FIXTURE(Fixture, ApplyRebirthBuff_BuffTargetLevel)
	{
		m_pPlayer->GetPlayerInfo()->nLevel = GConst::REBIRTH_DEBUFF_TARGET_LEVEL;
		gsys.pRebirthSystem->ApplyRebirthBuff(m_pPlayer, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsGained(GConst::REBIRTH_SOULBINDING_DEBUFF));

		m_pPlayer->GetPlayerInfo()->nLevel = GConst::REBIRTH_DEBUFF_TARGET_LEVEL+1;
		gsys.pRebirthSystem->ApplyRebirthBuff(m_pPlayer, GConst::REBIRTH_SOULBINDING_DEBUFF, 0, GConst::REBIRTH_DEBUFF_TARGET_LEVEL);
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsGained(GConst::REBIRTH_SOULBINDING_DEBUFF));
	}
}
