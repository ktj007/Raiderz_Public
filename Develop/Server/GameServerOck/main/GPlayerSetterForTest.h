#pragma once

class GEntityPlayer;
class GPlayerEmblem;
class GCheatChecker;
class GPlayerPVPArea;

class GPlayerSetterForTest : public MTestMemPool<GPlayerSetterForTest>
{
public:
	GPlayerSetterForTest(GEntityPlayer* pOwner);

	void SetPlayerEmblem(GPlayerEmblem* pPlayerEmblem);
	void SetPlayerCheatChecker(GCheatChecker* pChecker);
	void SetPlayerPVPArea(GPlayerPVPArea* pPVPArea);

private:
	GEntityPlayer* m_pOwner;
};
