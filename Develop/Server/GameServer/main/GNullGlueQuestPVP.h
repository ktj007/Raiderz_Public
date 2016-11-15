#pragma once

#include "GGlueQuestPVP.h"

class GNullGlueQuestPVP : public GGlueQuestPVP
{
public:
	GNullGlueQuestPVP(void);
	virtual ~GNullGlueQuestPVP(void);

	static void Init();
	static void FIni();

	static GNullGlueQuestPVP* GetNull();

	virtual bool IsNull() const;

private:
	static GNullGlueQuestPVP* m_pNullObj;
};
