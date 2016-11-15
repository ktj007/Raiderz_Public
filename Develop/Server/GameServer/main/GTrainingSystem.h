#ifndef _GTRAININIG_SYSTEM_H_
#define _GTRAININIG_SYSTEM_H_

#include "GNPCInfo.h"
#include "CSPalette.h"

class GEntityPlayer;
class GTalentInfo;
class GItem;
class GDBT_TALENT;
class GDBT_ITEM_LEARN_TALENT;

class GTrainingSystem : public MTestMemPool<GTrainingSystem>
{
public:
	GTrainingSystem();
	virtual ~GTrainingSystem();	
	
	bool Train(GEntityPlayer* pReqPlayer, int nTalentID);	
	bool TrainForDBTask(GDBT_TALENT& data);

	bool TrainByItem(GEntityPlayer* pPlayer, GItem* pItem);
	bool TrainByItemForDBTask(GDBT_ITEM_LEARN_TALENT& data);
	
	bool UntrainAll(GEntityPlayer* pReqPlayer, GItem* pItem);
	void UntrainAllForDBTask(GDBT_TALENT& data);

	bool ForceTrain(GEntityPlayer* pReqPlayer, int nTalentID);

private:
	bool Train_Check(GEntityPlayer* pReqPlayer, int nTalentID);
	bool Train_DB(GEntityPlayer* pReqPlayer, int nTalentID, bool bAsync=true);	
};

#endif //_GTRAININIG_SYSTEM_H_
