#include "stdafx.h"
#include "XCaptionEffect.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "MockPlayer.h"
#include "FTestGame.h"
#include "XCharacterCaption.h"
#include "XModuleActorControl.h"
#include "XCaptionMgr.h"
#include "XStrings.h"

SUITE(CaptionEffect)
{
	class FCaptionEffect : public FTestGame
	{
	public:
		FCaptionEffect() : FTestGame()
		{
			NewNPCInfo(1);

			m_pMyPlayer = m_TestGame.GetMyPlayer();
			m_pMyPlayer->GetActor()->SetAnimationUpdated(true);		// 이미 로딩이 되어 있다고 하자.
			m_pNetPlayer = NewNetPlayer();
			m_pNPC = NewNonPlayer(1);

			m_pCaptionMgr = global.ui->GetCaptionMgr();

		}

		virtual ~FCaptionEffect()
		{

		}

		XCaption* GetLastCaption()
		{
			XCaptionMgr::CAP_NODE_MAP& caption_node_map = m_pCaptionMgr->GetNodeMap();
			if (caption_node_map.empty()) return NULL;

			XCaptionMgr::CAP_NODE_MAP::iterator itor = caption_node_map.end();
			--itor;

			XCaption* pCaption = (*itor).second;

			return pCaption;
		}

		XCaptionEffect m_CaptionEffect;
		XMyPlayer* m_pMyPlayer;
		XNetPlayer* m_pNetPlayer;
		XNonPlayer* m_pNPC;
		XCaptionMgr*	m_pCaptionMgr;
	};

	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowHeal)
	{
		m_CaptionEffect.ShowHeal(m_pMyPlayer, 100);

		// 1차 테스트
		XCaption* pCaption = GetLastCaption();
		CHECK(wstring(L"+100") == pCaption->GetText());
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_HEAL, pCaption->GetColor());

		// 2차 테스트
		m_CaptionEffect.ShowHeal(m_pMyPlayer, 64);

		pCaption = GetLastCaption();
		CHECK(wstring(L"+64") == pCaption->GetText());

		// NetPlayer의 힐링은 보이지 않는다.
		m_CaptionEffect.ShowHeal(m_pNetPlayer, 50);

		pCaption = GetLastCaption();
		CHECK(wstring(L"+50") != pCaption->GetText());
	}


	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowGuardDefence)
	{
		m_CaptionEffect.ShowGuardDefense(m_pMyPlayer, 0, GT_PERFECT);

		// 완전 방어는 보여주는 것이 없다.
		//// 1차 테스트
		//XCaption* pCaption = GetLastCaption();
		//CHECK_EQUAL(string("방어 성공!"), pCaption->GetText());
		//CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_GUARD_DEFENSE, pCaption->GetColor());

		// NetPlayer의 방어 메세지는 보이지 않는다.
		m_CaptionEffect.ShowGuardDefense(m_pNetPlayer, 0, GT_PERFECT);
	}
	
	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowPartialGuardDefence)
	{
		m_CaptionEffect.ShowGuardDefense(m_pMyPlayer, 10, GT_PARTIAL);

		// 1차 테스트
		XCaption* pCaption = GetLastCaption();
		CHECK(wstring(L"10") == pCaption->GetText());
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_DAMAGE, pCaption->GetColor());

		// NetPlayer의 방어 메세지는 보이지 않는다.
		m_CaptionEffect.ShowGuardDefense(m_pNetPlayer, 0, GT_PERFECT);
	}

	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowDamage)
	{
		_DamageInfo damage;
		damage.nDamageFlag = DF_HITBYTALENT;
		damage.nAttackerUIID = m_pNPC->GetUIID();
		damage.nDamage = 55;

		// 1차 테스트
		m_CaptionEffect.ShowDamage(m_pMyPlayer, &damage, vec3::ZERO);

		XCaption* pCaption = GetLastCaption();
		CHECK(wstring(L"55") == pCaption->GetText());

		// 2차 테스트
		damage.nDamage = 89;
		m_CaptionEffect.ShowDamage(m_pMyPlayer, &damage, vec3::ZERO);

		pCaption = GetLastCaption();
		CHECK(wstring(L"89") == pCaption->GetText());

		// Deflect이고 0이면 출력하지 않는다.
		damage.nDamage = 0;
		damage.nMFState = MF_DEFLECT;
		m_CaptionEffect.ShowDamage(m_pMyPlayer, &damage, vec3::ZERO);

		pCaption = GetLastCaption();
		CHECK(wstring(L"0") != pCaption->GetText());


		// CTR_ZERO_TALENT_DAMAGE 플래그가 있으면 출력하지 않는다.
		damage.nDamage = 0;
		SetBitSet(damage.nFlags, CTR_ZERO_TALENT_DAMAGE);

		m_CaptionEffect.ShowDamage(m_pMyPlayer, &damage, vec3::ZERO);

		pCaption = GetLastCaption();
		CHECK(wstring(L"0") != pCaption->GetText());

	}

	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowDamage_MyColor)
	{
		_DamageInfo damage;
		damage.nDamageFlag = DF_HITBYTALENT;
		damage.nAttackerUIID = m_pNPC->GetUIID();
		damage.nDamage = 55;

		// 1차 테스트
		m_CaptionEffect.ShowDamage(m_pMyPlayer, &damage, vec3::ZERO);

		XCaption* pCaption = GetLastCaption();
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_DAMAGE, pCaption->GetColor());


		// 2차 테스트 - critical
		SetBitSet(damage.nFlags, CTR_CRITICAL);
		m_CaptionEffect.ShowDamage(m_pMyPlayer, &damage, vec3::ZERO);

		pCaption = GetLastCaption();
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_CRITICAL, pCaption->GetColor());

	}

	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowDamage_EnemyColor)
	{
		_DamageInfo damage;
		damage.nDamageFlag = DF_HITBYTALENT;
		damage.nAttackerUIID = m_pMyPlayer->GetUIID();
		damage.nDamage = 55;

		// 1차 테스트
		m_CaptionEffect.ShowDamage(m_pNPC, &damage, vec3::ZERO);

		XCaption* pCaption = GetLastCaption();
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_ENEMY_DAMAGE, pCaption->GetColor());


		// 2차 테스트 - critical
		SetBitSet(damage.nFlags, CTR_CRITICAL);
		m_CaptionEffect.ShowDamage(m_pNPC, &damage, vec3::ZERO);

		pCaption = GetLastCaption();
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_ENEMY_CRITICAL, pCaption->GetColor());
	}

	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_ShowBuff)
	{
		wstring strBuffName = L"다리 부상";

		// 1차 테스트 - 디버프
		m_CaptionEffect.ShowBuff(m_pMyPlayer, strBuffName.c_str(), false);

		XCaption* pCaption = GetLastCaption();
		CHECK(strBuffName == pCaption->GetText());
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_BUFF, pCaption->GetColor());


		// 2차 테스트 - 버프
		strBuffName = L"회복";
		m_CaptionEffect.ShowBuff(m_pMyPlayer, strBuffName.c_str(), true);

		pCaption = GetLastCaption();
		CHECK(strBuffName == pCaption->GetText());
		CHECK_EQUAL(XCONST::CAPTION_EFFECT_COLOR_MY_DEBUFF, pCaption->GetColor());

	}

	TEST_FIXTURE(FCaptionEffect, XCaptionEffect_GetDamageFlag)
	{
		_DamageInfo testDamageInfo;
		testDamageInfo.nFlags = 0;
		testDamageInfo.bHitDodge = true;
		CHECK(wstring(XGetStr( L"CAPTION_DODGE")) == m_CaptionEffect.GetDamageString(&testDamageInfo));


		testDamageInfo.bHitDodge = false;
		testDamageInfo.nFlags=0; SetBitSet(testDamageInfo.nFlags, CTR_RIPOSTE);
		CHECK(wstring(XGetStr( L"CAPTION_RIPOSTE")) == m_CaptionEffect.GetDamageString(&testDamageInfo));

		testDamageInfo.nFlags=0; SetBitSet(testDamageInfo.nFlags, CTR_DRAIN);
		CHECK(wstring(XGetStr( L"CAPTION_ABSORB")) == m_CaptionEffect.GetDamageString(&testDamageInfo));

		testDamageInfo.nFlags=0; SetBitSet(testDamageInfo.nFlags, CTR_IMMUNE);
		CHECK(wstring(XGetStr( L"CAPTION_DISPEL")) == m_CaptionEffect.GetDamageString(&testDamageInfo));

		testDamageInfo.nFlags=0; SetBitSet(testDamageInfo.nFlags, CTR_MISS);
		CHECK(wstring(XGetStr( L"CAPTION_MISS")) == m_CaptionEffect.GetDamageString(&testDamageInfo));

		testDamageInfo.nFlags=0; SetBitSet(testDamageInfo.nFlags, CTR_AVOID);
		CHECK(wstring(XGetStr( L"CAPTION_AVOID")) == m_CaptionEffect.GetDamageString(&testDamageInfo));

	}

}