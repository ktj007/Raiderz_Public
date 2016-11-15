#include "stdafx.h"
#include "ZPlayerManager.h"
#include "ZGameServerPlayerMap.h"

SUITE(PlayerManager)
{
	struct FPlayerManager
	{
		FPlayerManager()
		{
			m_nAID = 1;
			m_strUserID = L"TestUser";
			m_nCID = 1;
			m_nGameServerID = 1;
			m_uidPlayer = MUID(1);
			m_strName = L"TestPlayer";
			m_nGID = 1;
		}
		virtual ~FPlayerManager() {}

		ZPlayerManager	m_pmgr;

		AID				m_nAID;
		wstring			m_strUserID;
		CID				m_nCID;
		int				m_nGameServerID;
		MUID			m_uidPlayer;
		wstring			m_strName;
		int				m_nGID;

	};
	TEST_FIXTURE(FPlayerManager, TestPlayerManager_NewPlayer)
	{
		/// Fixture 설치
		ZPlayer* pPlayer = m_pmgr.NewPlayer(m_uidPlayer, m_nAID, m_strUserID);
		pPlayer->SetWorldInfo(m_nCID, m_nGameServerID, m_uidPlayer, m_strName, m_nGID);

		/// SUT 실행
		m_pmgr.Add(pPlayer);

		/// 검증
		CHECK_EQUAL(1, m_pmgr.GetObjectCount());
		CHECK(m_nCID == pPlayer->GetCID());
		CHECK(m_nGameServerID == pPlayer->GetGameServerID());
		CHECK(m_uidPlayer == pPlayer->GetUID());
	}

	TEST_FIXTURE(FPlayerManager, TestGameServerPlayerMap_Add)
	{
		/// Fixture 설치
		ZGameServerPlayerMap playerMap;

		ZPlayer* pPlayer = m_pmgr.NewPlayer(m_uidPlayer, m_nAID, m_strUserID);
		pPlayer->SetWorldInfo(m_nCID, m_nGameServerID, m_uidPlayer, m_strName, m_nGID);
		m_pmgr.Add(pPlayer);
		CHECK_EQUAL(1, pPlayer->GetReferenceCount());

		/// SUT 실행
		playerMap.Add(pPlayer);	///< PlayerMap은 레퍼런스 카운터만 증가시킨다.

		/// 검증
		CHECK_EQUAL(2, pPlayer->GetReferenceCount());
		CHECK_EQUAL(1, playerMap.GetObjectCount());
	}

}