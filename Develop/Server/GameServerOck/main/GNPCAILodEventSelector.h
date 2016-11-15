#pragma once

#include "GAIEventSelector.h"

class GEntityNPC;

class GNPCAILodEventSelector: public GAIEventSelector, public MTestMemPool<GNPCAILodEventSelector>
{
public:
	GNPCAILodEventSelector(GEntityNPC* pOwnerNPC);
	virtual ~GNPCAILodEventSelector();

	virtual bool IsSelectEvent(GAIEvent aiEvent);
	
	bool IsRelationField(MUID uidField);
	bool IsRelationSector(MUID uidField, int nSectorX, int nSectorY);
	bool IsRelationNPC(MUID uidNPC);
	bool IsPlayerEmptyInNeighborSector(void);

private:
	GEntityNPC* m_pOwnerNPC;
};
