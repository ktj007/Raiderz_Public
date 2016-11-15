#include "stdafx.h"
#include "GUnitTest.h"
#include "GDBManager.h"
#include "GUTHelper.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GTestWrapper.h"
#include "GInteractionSystem.h"
#include "GInnSystem.h"
#include "GInnInfo.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
#include "UTestDatabaseContainer.h"
#include "UTestDatabase.h"
#include "CSChrInfo.h"
#include "CCommandTable.h"
#include "MockLink.h"



SUITE(DBTesstInnSystem)
{
	class MockInteractionSystem : public GInteractionSystem
	{
	public :
		MockInteractionSystem() {}

		bool CheckIProgress(GEntityPlayer* pPlayer, INTERACTION_ELEMENT_TYPE nIET, int& outnIElementAct) override 
		{ 
			outnIElementAct = m_pInnInof->m_nID;
			return true; 
		}

	public :
		static GInnInfo* m_pInnInof;
	};

	GInnInfo* MockInteractionSystem::m_pInnInof = NULL;


	class DBTestInnSysteFixture
	{
	public :
		DBTestInnSysteFixture() : m_nInnPrice(10), m_nCharGold(1000)
		{
			m_pPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(GUnitTestUtil::NewUID());
			gmgr.pPlayerObjectManager->AddPlayer(m_pPlayerObject);

			m_pPlayerObject->GetAccountInfo().nAID				= 1;
			m_pPlayerObject->GetEntity()->GetPlayerInfo()->nCID	= 1;
			m_pPlayerObject->GetEntity()->GetPlayerInfo()->nGold = m_nCharGold;

			m_pLink = m_Helper.NewLink(m_pPlayerObject->GetEntity());

			m_pInnInfo = m_Helper.NewInnInfo();
			m_pInnInfo->m_nPrice = m_nInnPrice;

			MockInteractionSystem::m_pInnInof = m_pInnInfo;
		}

		~DBTestInnSysteFixture()
		{
			gmgr.pPlayerObjectManager->DeleteAllPlayer();
			m_Helper.ClearLinks();
			m_InnInfoMgr.Get()->Clear();
		}


	public :
		typedef GTestSysWrapper2<GInteractionSystem, MockInteractionSystem> InteractionSys;
		typedef GTestMgrWrapper<GInnInfoMgr>								InnInfoMgr;

		GPlayerObject*	m_pPlayerObject;
		MockLink*		m_pLink;
		GUTHelper		m_Helper;
		GInnInfo*		m_pInnInfo;

		const int		m_nInnPrice;
		const int		m_nCharGold;

		InnInfoMgr		m_InnInfoMgr;
		InteractionSys	m_InteractionSys;
	};


	DBTEST_FIXTURE(DBTestInnSysteFixture, DBTestRegistInn)
	{
		UTestDB.Seed("DefaultSeedData.xml");

		CHECK(gsys.pInnsystem->Regist(m_pPlayerObject->GetEntity()));

		while(!gsys.pDBManager->IsAllTaskCompleted())
		{
			gsys.pDBManager->Update();
		}		

		CHECK_EQUAL(2, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_UPDATE_GOLD, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_INN_REGIST, m_pLink->GetCommand(1).GetID());

		CHECK_EQUAL(m_nCharGold - m_nInnPrice, m_pLink->GetParam<int>(0, 0));

		int nCharInnID	= static_cast<int>(m_pInnInfo->m_nID);
		int nInnID		= static_cast<int>(m_pPlayerObject->GetEntity()->GetPlayerInfo()->nInnID);

		CHECK_EQUAL(nCharInnID, nInnID);
		CHECK_EQUAL(m_nCharGold - m_nInnPrice, m_pPlayerObject->GetEntity()->GetPlayerInfo()->nGold);

		CString strSQL;
		strSQL.Format("SELECT COUNT(*) AS cnt FROM dbo.Character WHERE CID = %d AND InnID = %d AND Gold = %d;"
			, m_pPlayerObject->GetEntity()->GetPlayerInfo()->nCID
			, m_pInnInfo->m_nID
			, m_nCharGold - m_nInnPrice);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, strSQL));
		CHECK_EQUAL(1, rs.GetFetchedCount());
		CHECK_EQUAL(1, rs.Field("cnt").AsInt());
	}
}