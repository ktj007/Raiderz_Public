#pragma once

#include "GGlueQuest.h"

class GNullGlueQuest : public GGlueQuest
{
public:
	GNullGlueQuest(void);
	virtual ~GNullGlueQuest(void);

	static void Init();
	static void FIni();

	static GNullGlueQuest* GetNull();

	virtual bool IsNull() const	{ return true; }

private:
	static GNullGlueQuest* m_pNullObj;
};
