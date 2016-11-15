#pragma once

class GEntityNPC;
class GEntityPlayer;

class GLootEnder : public MTestMemPool<GLootEnder>
{
public:
	bool End(GEntityPlayer* pPlayer);
	bool End(GEntityNPC* pNPC);

private:
	void RemoveGarbageDropItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC);
};
