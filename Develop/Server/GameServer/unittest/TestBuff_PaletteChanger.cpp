#include "stdafx.h"
#include "GTestForward.h"
#include "GModuleBuff.h"
#include "GBuffPaletteChanger.h"
#include "FBaseGame.h"
#include "GPalette.h"
#include "GPaletteItem.h"

SUITE(Buff_PaletteChanger)
{
	struct FBuff : public FBaseGame
	{
		FBuff()
		{
		}

		virtual ~FBuff()
		{
		}

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
	};

	TEST_FIXTURE(FBuff, Change)
	{
		m_pPlayer->GetPalette().Set(PALETTENUM_1, PALETTESLOT_5, PIT_TALENT, 123);
		CHECK_EQUAL(false, m_pPlayer->GetPalette().IsLocked());
		CHECK_EQUAL(123, m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_5)->m_nItemIDorTalentID);

		GBuffInfo* pBuff1=test.buff.NewBuffInfo(123);
		pBuff1->m_fDuration = 5.0f;
		pBuff1->m_bUseChangePalette = true;
		pBuff1->m_arrPalettes[PALETTESLOT_5] = 777;
		test.buff.GainBuffSelf(m_pPlayer, pBuff1);
		CHECK_EQUAL(true, m_pPlayer->GetPalette().IsLocked());
		CHECK_EQUAL(777, m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_5)->m_nItemIDorTalentID);

		m_pPlayer->GetModuleBuff()->Update(pBuff1->m_fDuration);
		CHECK_EQUAL(false, m_pPlayer->GetPalette().IsLocked());
		CHECK_EQUAL(123, m_pPlayer->GetPalette().GetPaletteItem(PALETTENUM_1, PALETTESLOT_5)->m_nItemIDorTalentID);
	}
}
