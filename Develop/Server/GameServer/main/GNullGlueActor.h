#pragma once

#include "GGlueActor.h"

class GNullGlueActor : public GGlueActor, public MTestMemPool<GNullGlueActor>
{
public:
	GNullGlueActor(void);
	virtual ~GNullGlueActor(void);

	static void Init();
	static void FIni();

	static GNullGlueActor* GetNull();

	virtual bool IsNull() const { return true; }

private:
	static GNullGlueActor* m_pNullObj;
};

