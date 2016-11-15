#ifndef _G_NPC_ICON_SELECTOR_H_
#define _G_NPC_ICON_SELECTOR_H_

class GEntityPlayer;
class GEntityNPC;
class GNPCInteractionElementInfo;
class GCondition_Leaf;

enum NPC_ICON_TYPE;
enum INTERACTION_TYPE;
enum CONDITION_TYPE;

struct NPC_ICON
{	
	NPC_ICON(GEntityNPC* pNPC, NPC_ICON_TYPE nNIT)
	: m_pNPC(pNPC), m_nIcon(nNIT) {}

	bool operator == (NPC_ICON& other)
	{
		if (m_pNPC != other.m_pNPC) return false;
		if (m_nIcon != other.m_nIcon) return false;

		return true;
	}

	GEntityNPC*		m_pNPC;
	NPC_ICON_TYPE	m_nIcon;
};

class GNPCIconSelector : public MTestMemPool<GNPCIconSelector>
{
public:
	vector<NPC_ICON> Select(GEntityPlayer* pPlayer, const vector<MUID>& vecUIDNPC, CONDITION_TYPE nConditionType);
	NPC_ICON_TYPE SingleSelect(GEntityPlayer* pPlayer, GEntityNPC* pNPC);	

protected:
	bool HasAutoCondition(const vector<GNPCInteractionElementInfo*>& vecIElementInfo, CONDITION_TYPE nConditionType);

	NPC_ICON_TYPE ClickIETypeToIconType(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo*pIEInfo);
	NPC_ICON_TYPE ClickIETypeToIconType_Loot(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIEInfo);
	NPC_ICON_TYPE ClickIETypeToIconType_Quest(GEntityPlayer* pPlayer, GNPCInteractionElementInfo* pIEInfo, bool bAutoCheckPass, const vector<GCondition_Leaf*>& vecFailConditionLeaf);

	NPC_ICON_TYPE ItemIETypeToIconType(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo*pIEInfo);	
};

#endif