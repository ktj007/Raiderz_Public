#pragma once

#include "CSChrInfo.h" // NPC_MODE

class GEntityNPC;

class GNPCMode : public MTestMemPool<GNPCMode>
{
public:
	GNPCMode(GEntityNPC* pOwner);
	~GNPCMode(void);

	void Init(NPC_MODE nMode);	
	void ChangeMode(NPC_MODE nNextMode);
	void SetMode(NPC_MODE nNextMode);
	NPC_MODE GetMode() const;

private:
	GEntityNPC* m_pOwner;
	NPC_MODE	m_nMode;
};
