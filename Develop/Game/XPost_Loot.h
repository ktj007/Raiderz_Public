#ifndef _XPOST_LOOT_H
#define _XPOST_LOOT_H

#include "XNetClient.h"
#include "XCommandTable.h"


inline void XPostLoot_SeeItem(MUID uidNPC)
{
	XPOSTCMD1(MC_LOOT_SEE_ITEM_REQ, MCommandParameterUID(uidNPC));
}

inline void XPostLoot_LootItem(MUID uidNPC, vector<MUID> & vecDropItemUID)
{
	XPOSTCMD2(MC_LOOT_LOOT_ITEM_REQ, MCommandParameterUID(uidNPC), MCommandParameterBlob(vecDropItemUID));
}

inline void XPostLoot_MasterLootItem(MUID uidNPC, MUID nDropItemUID, uint8 nSelectedIndex, bool bRandom)
{
	XPOSTCMD4(MC_LOOT_MASTERLOOT_ITEM_REQ, MCommandParameterUID(uidNPC), MCommandParameterUID(nDropItemUID), MCommandParameterUChar(nSelectedIndex), MCommandParameterBool(bRandom));
}

inline void XPostLoot_End()
{
	XPOSTCMD0(MC_LOOT_END_REQ);
}
#endif//_XPOST_LOOT_H