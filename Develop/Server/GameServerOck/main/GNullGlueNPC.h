#pragma once

#include "GGlueNPC.h"

class GNullNPCGlueDelegator;

class GNullGlueNPC : public GGlueNPC
{
public:
	GNullGlueNPC(void);
	virtual ~GNullGlueNPC(void);

	static void Init();
	static void FIni();

	static GNullGlueNPC* GetNull();

	virtual bool IsNull() const { return true; }

private:
	static GNullGlueNPC* m_pNullObj;
};
