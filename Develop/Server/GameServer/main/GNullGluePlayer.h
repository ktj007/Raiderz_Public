#pragma once

#include "GGluePlayer.h"

class GNullGluePlayer : public GGluePlayer
{
public:
	GNullGluePlayer(void);
	virtual ~GNullGluePlayer(void);

	static void Init();
	static void FIni();

	static GNullGluePlayer* GetNull();

	virtual bool IsNull() const	{ return true; }

private:
	static GNullGluePlayer* m_pNullObj;
};
