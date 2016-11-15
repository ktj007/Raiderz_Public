#pragma once

#include "XSystem.h"

class MockSystem : public XSystem
{
protected:
	unsigned int			m_nExpecedNowTime;
public:
	MockSystem() : XSystem(), m_nExpecedNowTime(UINT_MAX) {}
	virtual ~MockSystem() {}
	virtual void ShowCursor( bool bShow ) {}

	virtual unsigned int GetNowTime();
	void ExpectNowTime(unsigned int nNowTime) { m_nExpecedNowTime = nNowTime; }
};

inline void XExpectNowTime(unsigned int nExpectTime)
{
	MockSystem* pMockSystem = dynamic_cast<MockSystem*>(global.system);
	if (pMockSystem) pMockSystem->ExpectNowTime(nExpectTime);
}