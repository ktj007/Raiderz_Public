#pragma once

class GNPCEscortQuests;
class GEntityNPC;

class GNPCQuest : public MTestMemPool<GNPCQuest>
{
private:
	GEntityNPC*			m_pOwner;
	GNPCEscortQuests*	m_pNPCEscortQuests;

public:
	GNPCQuest(GEntityNPC* pOwner);
	~GNPCQuest();

	void Clear();
	GNPCEscortQuests& GetNPCEscortQuests() { _ASSERT(m_pNPCEscortQuests); return (*m_pNPCEscortQuests); }
};
