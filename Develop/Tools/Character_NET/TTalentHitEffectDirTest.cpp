#include "stdafx.h"
#include "TTalentHitEffectDirTest.h"

TTalentHitEffectDirTest::TTalentHitEffectDirTest()
{
	m_pTalentInfo = NULL;
	m_fHitTime = 0.0f;
	m_fCheckTime = 0.0f;
}

TTalentHitEffectDirTest::~TTalentHitEffectDirTest()
{

}

void TTalentHitEffectDirTest::SetData( TTalentInfo* pTalentInfo )
{
	m_pTalentInfo = NULL;
	m_vecTestList.clear();
	m_fHitTime = 0.0f;

	if(pTalentInfo == NULL)
		return;

	m_pTalentInfo = pTalentInfo;

	if(pTalentInfo->m_HitInfo.m_nReferenceID > 0)
	{
		m_pTalentInfo = TTalentHelper::GetTalentInfo(pTalentInfo->m_HitInfo.m_nReferenceID, pTalentInfo->m_HitInfo.m_nReferenceMode);

		if(m_pTalentInfo == NULL)
			return;
	}

	m_vecTestList.clear();

	for(vector<CSHitSegment>::iterator it = m_pTalentInfo->m_HitInfo.m_vSegments.begin(); it != m_pTalentInfo->m_HitInfo.m_vSegments.end(); ++it)
	{
		TEST_DATA datatest;
		datatest.m_fCheckTime = it->m_fCheckTime;
		m_vecTestList.push_back(datatest);
	}
}

bool TTalentHitEffectDirTest::Update( float fDelta )
{
	if(m_pTalentInfo == NULL)
		return false;

	m_fHitTime += fDelta;

	for(vector<TEST_DATA>::iterator it = m_vecTestList.begin(); it != m_vecTestList.end(); ++it)
	{
		if(it->m_fCheckTime <= m_fHitTime &&
			it->m_bDone == false)
		{
			it->m_bDone = true;
			m_fCheckTime = it->m_fCheckTime;

			return true;
		}
	}

	return false;
}