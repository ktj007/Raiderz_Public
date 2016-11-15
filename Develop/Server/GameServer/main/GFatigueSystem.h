#ifndef _GFATIGUE_SYSTEM_H_
#define _GFATIGUE_SYSTEM_H_

enum INN_ROOM_TYPE;
class GEntityPlayer;

class GFatigueSystem : public MTestMemPool<GFatigueSystem>
{
private:
	void SetFatigue(GEntityPlayer* pPlayer, int nQuantity);
	void IncFatigue(GEntityPlayer* pPlayer, short nIncQuantity);	
	void DecFatigue(GEntityPlayer* pPlayer, short nDecQuantity);

public:
	GFatigueSystem();
	~GFatigueSystem();;

	FATIGUE_TYPE GetFatigueType(short nFatigueQuantity);

	void SetFatigueByCommand(GEntityPlayer* pPlayer, int nFatiguePercent);
	void IncFatigueByBlankTime(GEntityPlayer* pPlayer, int nBlankTime);
	void SetFatigueBySleep(GEntityPlayer* pPlayer, INN_ROOM_TYPE nInnRoomGrade);

	void DecFatigueByTime(GEntityPlayer* pPlayer);
};

#endif//_GFATIGUE_SYSTEM_H_
