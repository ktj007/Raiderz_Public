#ifndef _G_INN_SYSTEM_H_
#define _G_INN_SYSTEM_H_

class GEntityPlayer;
class GInnRoomInfo;
class GDynamicField;

class GInnSystem : public MTestMemPool<GInnSystem>
{
private:
	void SpawnCraftingNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnBedNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnStorageNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnGuildStorageNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);

	void SpawnCarpetNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnFirePlaceNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnMuralNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnTableNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnFunitureNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);
	void SpawnStewardNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);

public:
	GInnSystem();
	~GInnSystem();

	bool BeginSleep(GEntityPlayer* pPlayer);
	bool EndSleep(GEntityPlayer* pPlayer);

	void SpawnInnRoomNPC(GEntityPlayer* pCreator, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField);

	// GM
	void SetInnRoom(GEntityPlayer* pPlayer, int nID);
};

#endif//_G_INN_SYSTEM_H_
