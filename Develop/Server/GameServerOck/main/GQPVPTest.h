#pragma once

#include "MAutoTest.h"

class GField;

struct GQPVPInfo
{
	GQPVPInfo()
	: nFieldID(0), nQPVPEventID(0) {	}

	int		nFieldID;
	int		nQPVPEventID;
};

class GQPVPTest : public MAutoTest, public MTestMemPool<GQPVPTest>
{
public:
	GQPVPTest(void);
	~GQPVPTest(void);

	virtual void OnCreate();

	void InitInfo();
	void PickUPField();	

	virtual void OnUpdate(float fDelta);
	virtual const char* GetName() { return "QPVPTest"; }

public:
	GQPVPInfo	m_Info;

private:
	GField*			m_pField;
};
