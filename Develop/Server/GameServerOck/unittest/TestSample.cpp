#include "stdafx.h"
#include "SUnitTest.h"
#include "GDef.h"
#include "GTalentInfo.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "GNPCMgr.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GModuleCombat.h"
#include "MockLink.h"
#include "GCommandTable.h"
#include "GTalentInfoMgr.h"

#include "MiNetHelper.h"
#include "GUTHelper_Field.h"
#include "GGlobal.h"
#include "GCommandCenter.h"

SUITE(Sample)
{
	TEST(Bird_Sample1)
	{
		bool const b = true;
		CHECK(b);

		int const x = 3;
		int const y = 3;
		CHECK_EQUAL(x, y);
	}


	TEST3(Bird_Sample2)
	{
		CHECK(true);
	}


	struct UseTalentSampleFixture
	{
		UseTalentSampleFixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_NPCInfo.nID = 1;
		}
		~UseTalentSampleFixture()
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GNPCInfo		m_NPCInfo;
		GTestMgrWrapper<GTalentInfoMgr>	m_TalentInfoMgrWrapper;
	};

	TEST_FIXTURE(UseTalentSampleFixture, UseTalentSample)
	{
		// 플레이어가 쓰는 탤런트
		GTalentInfo* pNewTalentInfo = m_Helper.NewMeleeTalentInfo(1);
		m_Helper.SetTalentDamage(pNewTalentInfo, 10);		// 데미지 10 설정
		m_Helper.AddSimpleHitInfo(pNewTalentInfo, 0.0f);	// 바로 판정하는 탤런트임
		
		vec3 pos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 dir = vec3(0.0f,1.0f,0.0f);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, pos, dir);
		GEntityPlayer* pAttackerPlayer = GUTHelper::NewEntityPlayer(m_pField, pos);

		m_Helper.SetEntityPlayerStatus(pAttackerPlayer, 100, 100, 100);

		// pc가 탤런트 사용하여 NPC를 즉사
		pAttackerPlayer->GetModuleCombat()->UseTalent(pNewTalentInfo, TALENT_TARGET_INFO::Invalid(), false);

		m_pField->Update(0.1f);
	}

	TEST(MockLinkSample)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();

		GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField, vec3(1000,1000,0));
		MockLink* pLink1 = GUTHelper::NewLink(pPlayer1);

		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField, vec3(1000,1000,0));
		MockLink* pLink2 = GUTHelper::NewLink(pPlayer2);

		pLink1->OnRecv(MC_MSG_REQ, 5, NEW_INT(MT_SAY), NEW_WSTR(L"Test"), NEW_BLOB(), NEW_BLOB(), NEW_INT());

		CHECK(pLink2->GetCommandCount() == 1);
		CHECK(pLink2->GetCommand(0).GetID() == MC_MSG_RES);

		const wchar_t* szMsg = pLink2->GetParam<const wchar_t*>(0, 1);
		CHECK(!wcscmp(szMsg, L"Test"));

		pField->Destroy();
		GUTHelper::ClearLinks();
	}

	TEST(TestMiNetHelper1)
	{
		USES_CONVERSION_EX;

		unsigned long nAddr = 0x7f000001;
		wstring strIP = A2W_EX(MiNetHelper::ConvertIPAddress(nAddr), 0);

		CHECK(L"1.0.0.127" == strIP);

		strIP = L"127.0.0.1";
		nAddr = MiNetHelper::ConvertIPAddress(W2A_EX(strIP.c_str(), 0));
		CHECK_EQUAL(0x100007f, nAddr);
	}

	TEST(MCommandSerialize)
	{
		static char buff[10240];
		MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(MC_NET_REPLY_CONNECT, 4);
		pNewCommand->AddParameter(new MCommandParameterUID(1));
		pNewCommand->AddParameter(new MCommandParameterUID(2));
		pNewCommand->AddParameter(new MCommandParameterUInt(3));
		int nCommandSize = pNewCommand->GetSize();
		pNewCommand->GetData(buff, nCommandSize);
		SAFE_DELETE(pNewCommand);

		MCommand cmd2;
		cmd2.SetData(buff, nCommandSize);
		MUID uid;
		cmd2.GetParameter(&uid, 0, MPT_UID);
		CHECK_EQUAL(1, uid);
		cmd2.GetParameter(&uid, 1, MPT_UID);
		CHECK_EQUAL(2, uid);
		uint32 test;
		cmd2.GetParameter(&test, 2, MPT_UINT);
		CHECK_EQUAL(3, test);
	}

	TEST(STL_unique)
	{
		vector<MUID> vecActors;
		vecActors.push_back(MUID(11));
		vecActors.push_back(MUID(22));
		vecActors.push_back(MUID(33));
		vecActors.push_back(MUID(44));
		vecActors.push_back(MUID(55));
		vecActors.push_back(MUID(11));
		vecActors.push_back(MUID(22));
		vecActors.push_back(MUID(33));
		vecActors.push_back(MUID(44));
		vecActors.push_back(MUID(55));
		CHECK_EQUAL(10U, vecActors.size());
		sort(vecActors.begin(), vecActors.end());
		vecActors.erase(unique(vecActors.begin(), vecActors.end()), vecActors.end());
		CHECK_EQUAL(5U, vecActors.size());
	}
}
