#ifndef _TTALENT_HIT_EFFECT_DIR_TEST_H
#define _TTALENT_HIT_EFFECT_DIR_TEST_H

#pragma once

class TTalentHitEffectDirTest
{
	struct TEST_DATA
	{
		float			m_fCheckTime;
		bool			m_bDone;

		TEST_DATA()
		{
			m_fCheckTime	= 0.0f;
			m_bDone			= false;
		}
	};

private:
	TTalentInfo*		m_pTalentInfo;

	float				m_fHitTime;
	float				m_fCheckTime;

	vector<TEST_DATA>	m_vecTestList;

public:
	TTalentHitEffectDirTest();
	virtual ~TTalentHitEffectDirTest();

	void				SetData(TTalentInfo* pTalentInfo);
	bool				Update(float fDelta);
	float				GetCheckTime() { return m_fCheckTime; }
};

#endif