#ifndef _G_AI_TEST_H_
#define _G_AI_TEST_H_

#include "MAutoTest.h"
#include "GFieldInfo.h"
#include "GDef.h"


struct GAITestInfo
{
	GAITestType	nTestType;
	int		nFieldID;
	int		nMaxNPCCount;

	GAITestInfo()
	{
		nTestType = AITEST_STRESS_COMBAT_ONE_FIELD;
		nFieldID = 0;
		nMaxNPCCount = 100;
	}
};

class GField;


class GAITest : public MAutoTest, public MTestMemPool<GAITest>
{
private:
	GField*			m_pField;
	vector<int>		m_vecNPCIDs;

	void			ChangeNPCInfoForTest();
	int				GetRandomNPCID();
	const SPAWN_INFO*		GetRandomSpawnInfo() const;
	void			InitFieldTestSpawns();
	void			InitFieldSpawnInfo();
	void			Update_Type1();
	void			Update_Type2();	
		
public:
	GAITestInfo		m_Info;

	GAITest() : m_pField(NULL)
	{

	}
	virtual void OnCreate();

	void PickUPField();

	void InitInfo();

	virtual void OnUpdate(float fDelta);
	virtual const char* GetName() { return "AITest"; }
};

#endif