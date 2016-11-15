#pragma once

class GQuestSystem;
class GQObjUpdater;

class GQuestSystemSetterForTest : public MTestMemPool<GQuestSystemSetterForTest>
{
public:
	GQuestSystemSetterForTest(GQuestSystem* pOwner);	

	void SetQObjUpdater(GQObjUpdater* pQObjUpdater);

private:
	GQuestSystem* m_pOwner;
};
