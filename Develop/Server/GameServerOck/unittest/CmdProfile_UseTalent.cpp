#include "stdafx.h"
#include "GUnitTestRunner.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "FBaseMockLink.h"
#include "FBasePlayer.h"
#include "GUTHelper.h"
#include "GTestForward.h"

SUITE(Cmd_UseTalent)
{
	class Fixture : public FBaseMockLink, public FBasePlayer, public FBaseFieldInfo, public FBaseFieldMgr
	{
	public:
		Fixture() 
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3::ZERO);

			// -- link
			MUID uidLink = m_pPlayer->GetUID();
			m_pLink = NewLink(m_pPlayer);
		}
		virtual ~Fixture()
		{
			m_pField->Destroy();
		}

		MockField* m_pField;
		MockLink*		m_pLink;
		GEntityPlayer*	m_pPlayer;
	};

	TEST_FIXTURE4(Fixture, RandomUseTalent)
	{
		vector<int> vecTalents = gmgr.pTalentInfoMgr->GetPlayerTalentIDs();
		for (int i=0; i<100; ++i)
		{
			for each (int nTalentID in vecTalents)
			{
				GTalentInfo* pTalentInfo = test.talent.NewMeleeTalentInfo(nTalentID);	

				{
					AutoTestCaseTimer(AutoTestCaseTimer::SFormat(L"talent_id: %d", 
						nTalentID
						));

					m_pLink->OnRecv(MC_ACTION_USE_TALENT_REQ, 3, 
						NEW_INT(nTalentID),
						NEW_SVEC(vec3(0,1,0)),
						NEW_VEC(m_pPlayer->GetPos()));
				}

				m_pPlayer->doCancelTalentForce(false);
			}
		}
	}
}