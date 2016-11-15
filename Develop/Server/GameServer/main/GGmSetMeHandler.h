#ifndef _G_GM_SETME_HANDLER_H_
#define _G_GM_SETME_HANDLER_H_

class GEntityPlayer;
class GPresetInfo;

class GGmSetMeHandler
{
private:
	void Level(GEntityPlayer* pPlayer, int nLevel);
	void TP(GEntityPlayer* pPlayer, int nTP);
	void Money(GEntityPlayer* pPlayer, int nMoney);
	void Fatigue(GEntityPlayer* pPlayer, int nFatigue);
	void InnRoom(GEntityPlayer* pPlayer, int nInnRoomID);
	void HP(GEntityPlayer* pPlayer, int nHP);
	void Grade(GEntityPlayer* pPlayer, int nPlayerGrade);
	void Ghost( GEntityPlayer* pPlayer );
	void Exp(GEntityPlayer* pPlayer, int nExp);
	
	void Preset(GEntityPlayer* pPlayer, wstring strClass);
	void Talent(GEntityPlayer* pPlayer, wstring strTalent);

public:
	bool Handle(GEntityPlayer* pPlayer, wstring strType, wstring strValue);	
};

#endif
