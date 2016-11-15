#ifndef _XUNIT_TEST_UTIL_H
#define _XUNIT_TEST_UTIL_H

#include "XSystem.h"

class XUnitTestUtil
{
private:
	static int		m_nID;
	static UIID		m_nGenUIID;
public:
	static int NewID()
	{
		m_nID++;
		return m_nID;
	}

	static MUID NewMUID()
	{
		return global.system->GenerateLocalUID();
	}

	static UIID NewUIID()
	{
		m_nGenUIID++;
		return m_nGenUIID;
	}
};

#endif // _XUNIT_TEST_UTIL_H
