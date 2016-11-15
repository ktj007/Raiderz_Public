#pragma once

class GEntityNPC;
class GNPCRewarder;
class GNPCLoot;
class GDropList;

class GNPCSetterForTest : public MTestMemPool<GNPCSetterForTest>
{
public:
	GNPCSetterForTest(GEntityNPC* pOwner);

	void SetNPCRewarder(GNPCRewarder* pNPCRewarder);
	void SetNPCLoot(GNPCLoot* pNPCLoot);
	void SetDropList(GDropList* pDropList);

private:
	GEntityNPC* m_pOwner;
};
